#ifndef EXACTO_DATA_STORAGE_H
#define EXACTO_DATA_STORAGE_H
#include <stdint.h>
extern uint8_t appendDataToExactoDataStorage(uint8_t * data, uint16_t datacount);
extern uint8_t getDataFromExactoDataStorage(uint8_t * data, uint8_t datacount);
extern uint8_t checkExactoDataStorage(void);
#endif //EXACTO_DATA_STORAGE_H
