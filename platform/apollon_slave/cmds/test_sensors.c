#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "exacto_commander/exacto_data_storage.h"
#include "exacto_commander/exacto_sensors.h"
#include "project/base_project_defs.h"
#include "spi_gen/spi1_generated.h"

static spi_pack_t PackageToSend = {
    .result = EXACTO_OK,
};
static spi_pack_t PackageToGett = {
    .result = EXACTO_WAITING,
};
typedef struct{
    struct lthread thread;
    uint8_t data[SPI_PACK_SZ];
    uint8_t datalen;
}thread_container_t;

uint8_t Marker = 0;

thread_container_t AppendDataToSendThread;
thread_container_t AppendDataToSendAndReceiveThread;
thread_container_t CheckDataFormGettThread;
thread_container_t CheckDataFromSendThread;

static int appendDataToSensAndReceive(struct lthread *self)
{
    thread_container_t * _trg;
    _trg = (thread_container_t *) self;
    for (uint8_t i = 0; i < _trg->datalen; i++)
    {
        PackageToSend.data[i] = _trg->data[i];
    }
    PackageToSend.datalen = _trg->datalen;
    PackageToSend.result = EXACTO_WAITING;
    PackageToSend.type = SPI_DT_TRANSMIT_RECEIVE;
    sendSpi1Half(&PackageToSend);

    return 0;
}

static int appendDataToSend(struct lthread * self)
{
    thread_container_t * _trg;
    _trg = (thread_container_t *)self;
    for (uint8_t i = 0; i < _trg->datalen; i++)
    {
        PackageToSend.data[i] = _trg->data[i];
    }
    PackageToSend.datalen = _trg->datalen;
    PackageToSend.result = EXACTO_WAITING;
    PackageToSend.type = SPI_DT_TRANSMIT;
    sendSpi1Half(&PackageToSend);
    return  0;
}
static int checkDataFromSend( struct lthread * self)
{
    PackageToSend.type = SPI_DT_CHECK;
    if (PackageToSend.result == EXACTO_OK)
    {
        Marker = 1;
    }
    else{
        sendSpi1Half(&PackageToSend);
    }
    
    return 0;
}
static int checkDataFromGet(struct lthread * self)
{
    thread_container_t * _trg;
    _trg = (thread_container_t *)self;
    PackageToGett.type = SPI_DT_RECEIVE;
    PackageToGett.datalen = _trg->datalen;
    if (PackageToGett.result == EXACTO_OK)
    {
        Marker = 1;
    }
    else{
        waitSpi1Half(&PackageToGett);
    }
    return 0;
}

void sendAndReceive( const uint8_t address)
{
    AppendDataToSendAndReceiveThread.data[0] = address | 0x80;
    AppendDataToSendAndReceiveThread.datalen = 1;
    lthread_launch(&AppendDataToSendAndReceiveThread.thread);
    CheckDataFormGettThread.datalen = 1;
    Marker = 0;
    while(!Marker)
    {
        lthread_launch(&CheckDataFormGettThread.thread);
    }
    uint8_t ctrl_value = PackageToGett.data[0];
    printf("Get some data [ %#04x = %d ]\n",ctrl_value, ctrl_value);

}

int main(int argc, char *argv[]) {

    const uint8_t adr_mask = 0x7F;

    const uint8_t lsm303ah_3wire_adr = 0x21;
    const uint8_t lsm303ah_3wire_val = 0x05;
    const uint8_t lsm303ah_whoami_xl_adr = 0x0f;
    // const uint8_t lsm303ah_whoami_xl_val = 0x43;

    const uint8_t lsm303ah_whoami_mg_adr = 0x4f;
    // const uint8_t lsm303ah_whoami_mg_val = 0x40;

    // //0x12, 0x0C

    const uint8_t ism330dlc_3wire_adr = 0x12;
    const uint8_t ism330dlc_3wire_val = 0x0C;

    const uint8_t ism330dlc_whoami_adr = 0x0F;
    // const uint8_t ism330dlc_whoami_val = 0x6A;

    // uint8_t data_mas[2] = {0};
    AppendDataToSendThread.data[0] = lsm303ah_3wire_adr & adr_mask;
    AppendDataToSendThread.data[1] = lsm303ah_3wire_val;
    AppendDataToSendThread.datalen = 2;
    

    lthread_init(&AppendDataToSendThread.thread, appendDataToSend);
    lthread_init(&CheckDataFormGettThread.thread, checkDataFromGet);
    lthread_init(&CheckDataFromSendThread.thread, checkDataFromSend);
    lthread_init(&AppendDataToSendAndReceiveThread.thread, appendDataToSensAndReceive);

    printf("Start send data throw spi\n");
    enableExactoSensor(LSM303AH);
    sleep(1);

    lthread_launch(&AppendDataToSendThread.thread);

    while(!Marker)
    {
        // sleep(1);
        lthread_launch(&CheckDataFromSendThread.thread);
    }
    disableExactoSensor(LSM303AH);
    printf("Options are sended!\n");

    AppendDataToSendAndReceiveThread.data[0] = lsm303ah_whoami_xl_adr | 0x80;
    AppendDataToSendAndReceiveThread.datalen = 1;
    enableExactoSensor(LSM303AH);

    lthread_launch(&AppendDataToSendAndReceiveThread.thread);
    Marker = 0;
    while(!Marker)
    {
        // sleep(1);
        lthread_launch(&CheckDataFromSendThread.thread);
    }
    printf("Wait whoami xl value\n");
    CheckDataFormGettThread.datalen = 1;
    Marker = 0;
    while(!Marker)
    {
        // sleep(1);
        lthread_launch(&CheckDataFormGettThread.thread);
    }
    disableExactoSensor(LSM303AH);
    uint8_t ctrl_value = 0;
    ctrl_value = PackageToGett.data[0];
    printf("Get some data [ %#04x = %d ]\n",ctrl_value, ctrl_value);


   //check mg data 
    printf("Check whoami lsm303 mg value");
    enableExactoSensor(LSM303AH);
    sendAndReceive(lsm303ah_whoami_mg_adr);
    disableExactoSensor(LSM303AH);
    
    //set option for second sensor
    printf("Setup ism330 sensor\n");
    AppendDataToSendThread.data[0] = ism330dlc_3wire_adr & adr_mask;
    AppendDataToSendThread.data[1] = ism330dlc_3wire_val;
    AppendDataToSendThread.datalen = 2;
    enableExactoSensor(ISM330DLC);
    lthread_launch(&AppendDataToSendThread.thread);
    while(!Marker)
    {
        lthread_launch(&CheckDataFromSendThread.thread);
    }
    disableExactoSensor(ISM330DLC);
    printf("Options are setted\n");

    printf("Get whoami ism330 value\n");
    enableExactoSensor(ISM330DLC);
    sendAndReceive(ism330dlc_whoami_adr);
    disableExactoSensor(ISM330DLC);

    return 0;
}
