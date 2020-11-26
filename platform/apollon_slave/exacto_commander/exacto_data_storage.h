#ifndef EXACTO_DATA_STORAGE_H
#define EXACTO_DATA_STORAGE_H
#include <stdint.h>
extern uint8_t appendDataToExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
extern uint8_t getDataFromExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
extern uint8_t checkExactoDataStorage( const uint8_t id);
extern uint8_t addAppenderExactoDataStorage(void);
#endif //EXACTO_DATA_STORAGE_H
