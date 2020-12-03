#ifndef EXACTO_DATA_STORAGE_H
#define EXACTO_DATA_STORAGE_H
#include <errno.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>

#include <stdint.h>
#include "project/base_project_defs.h"
typedef enum{
    APPEND = 0,
    GET,
    CHECK
}function_list_t;
// THREADS
typedef struct {
    struct lthread thread;                          // поток исполнения запросов
    struct mutex mx;                             // контрольный мьютекс для контроля окончания потока
    uint8_t databuffer[THREAD_CONTROL_BUFFER_SZ];   // буффер хранения данных
    uint8_t datalen;
    thread_control_result_t result;
    function_list_t fun_type;
}thread_control_t;
extern uint8_t appendDataToExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
extern uint8_t getDataFromExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
extern uint8_t checkExactoDataStorage( const uint8_t id);
extern uint8_t addAppenderExactoDataStorage(void);
#endif //EXACTO_DATA_STORAGE_H
