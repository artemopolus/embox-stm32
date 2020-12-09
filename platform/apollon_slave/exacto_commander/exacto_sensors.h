#ifndef EXACTO_SENSORS_H
#define EXACTO_SENSORS_H
#include <stdint.h>
enum {
    LSM303AH = 0,
    ISM330DLC,
    BMP280
}exacto_sensors_list_t;
extern void sendDataToLsm303ah(uint8_t * data, const uint8_t datalen);
extern void getDataFromLsm303ah(uint8_t * data, const uint8_t datalen);
#endif //EXACTO_SENSORS_H