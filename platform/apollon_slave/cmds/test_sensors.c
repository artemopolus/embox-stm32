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
thread_container_t CheckDataFormGettThread;
thread_container_t CheckDataFromSendThread;

static int appendDataToSend(struct lthread * self)
{
    thread_container_t * _trg;
    _trg = (thread_container_t *)self;
    for (uint8_t i = 0; i < _trg->datalen; i++)
    {
        PackageToSend.data[i] = _trg->data[i];
    }
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
    // thread_container_t * _trg;
    // _trg = (thread_container_t *)self;
    if (PackageToGett.result == EXACTO_OK)
    {
        Marker = 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    const uint8_t adr_mask = 0x7F;

    const uint8_t lsm303ah_3wire_adr = 0x21;
    const uint8_t lsm303ah_3wire_val = 0x05;
    const uint8_t lsm303ah_whoami_xl_adr = 0x0f;
    // const uint8_t lsm303ah_whoami_xl_val = 0x43;

    // const uint8_t lsm303ah_whoami_mg_adr = 0x4f;
    // const uint8_t lsm303ah_whoami_mg_val = 0x40;

    // //0x12, 0x0C

    // const uint8_t ism330dlc_3wire_adr = 0x12;
    // const uint8_t ism330dlc_3wire_val = 0x0C;

    // const uint8_t ism330dlc_whoami_adr = 0x0F;
    // const uint8_t ism330dlc_whoami_val = 0x6A;

    // uint8_t data_mas[2] = {0};
    AppendDataToSendThread.data[0] = lsm303ah_3wire_adr & adr_mask;
    AppendDataToSendThread.data[1] = lsm303ah_3wire_val;
    AppendDataToSendThread.datalen = 2;
    

    lthread_init(&AppendDataToSendThread.thread, appendDataToSend);
    lthread_init(&CheckDataFormGettThread.thread, checkDataFromGet);
    lthread_init(&CheckDataFromSendThread.thread, checkDataFromSend);

    printf("Start send data throw spi\n");
    enableExactoSensor(LSM303AH);

    lthread_launch(&AppendDataToSendThread.thread);

    while(!Marker)
    {
        sleep(1);
        lthread_launch(&CheckDataFromSendThread.thread);
    }
    // disableExactoSensor(LSM303AH);
    printf("Options are sended!\n");

    AppendDataToSendThread.data[0] = lsm303ah_whoami_xl_adr | 0x80;
    AppendDataToSendThread.datalen = 1;
    // enableExactoSensor(LSM303AH);

    lthread_launch(&AppendDataToSendThread.thread);
    Marker = 0;
    while(!Marker)
    {
        sleep(1);
        lthread_launch(&CheckDataFromSendThread.thread);
    }
    printf("Wait whoami data!\n");
    Marker = 0;
    while(!Marker)
    {
        sleep(1);
        lthread_launch(&CheckDataFormGettThread.thread);
    }
    disableExactoSensor(LSM303AH);
    printf("Get some data\n");

    return 0;
}
