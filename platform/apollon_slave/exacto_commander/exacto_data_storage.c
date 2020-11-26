#include "exacto_data_storage.h"
#include "project/base_project_defs.h"
#include <errno.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>


typedef struct{
    uint8_t isEmpty;
    struct mutex dtmutex;
}exactodatastorage;
// static struct mutex MutexOfExactoDataStorage;
static exactodatastorage ExDtStorage = {
    .isEmpty = 0,
};



typedef struct{
    struct lthread thread;
    uint8_t data[MAIL_SZ_EXACTO_COMMANDER];
    uint8_t datalen;
    uint8_t data2copylen;
}mail4lthread;

static mail4lthread AppenderData = {
    .datalen = 0,
};
static mail4lthread GetterData = {
    .datalen = 0,
};

static int appenderData_handler(struct lthread *self)
{
    goto *lthread_resume(self, &&start);
start:
//     /* инициализация */
//     result = 0;
mutex_retry:
    if (mutex_trylock_lthread(self, &ExDtStorage.dtmutex ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    // do something
    mutex_unlock_lthread(self, &ExDtStorage.dtmutex);
    return 0;
}

static int getterData_handler(struct lthread *self)
{
    goto *lthread_resume(self, &&start);
start:
//     /* инициализация */
//     result = 0;
mutex_retry:
    if (mutex_trylock_lthread(self, &ExDtStorage.dtmutex ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    // do something
    mutex_unlock_lthread(self, &ExDtStorage.dtmutex);
    return 0;
}

EMBOX_UNIT_INIT(initExactoDataStorage);
static int initExactoDataStorage(void)
{
    mutex_init_schedee(&ExDtStorage.dtmutex);
    lthread_init(&AppenderData.thread, &appenderData_handler);
    lthread_init(&GetterData.thread, &getterData_handler);
    return 0;
}

uint8_t appendDataToExactoDataStorage(uint8_t * data, uint16_t datacount)
{
    // copy here in storage
    lthread_launch(&AppenderData.thread);
    return 0;
}
uint8_t getDataFromExactoDataStorage(uint8_t * data, uint8_t datacount)
{
    return 0;
}
uint8_t checkExactoDataStorage(void)
{
    uint8_t res = 0;
    return res;
}
