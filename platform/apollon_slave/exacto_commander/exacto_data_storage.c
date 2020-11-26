#include "exacto_data_storage.h"
#include "project/base_project_defs.h"
#include <errno.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>

#define CHECK_ID    if (id >= MAIL_ID_CNT_EXACTO_COMMANDER )    return 1;

typedef struct{
    uint8_t isEmpty;
    struct mutex dtmutex;
}exactodatastorage;
// static struct mutex MutexOfExactoDataStorage;
static exactodatastorage ExDtStorage = {
    .isEmpty = 0,
};

static uint8_t CntIDMail4Thread = 0;

typedef struct{
    struct lthread thread;
    uint8_t data[MAIL_SZ_EXACTO_COMMANDER];
    uint8_t datalen;
    uint8_t data2copylen;
    uint8_t result; // 0 - not used 1 - waiting 2 - working
}mail4lthread;

static mail4lthread AppenderData[MAIL_ID_CNT_EXACTO_COMMANDER];
static mail4lthread GetterData[GETT_ID_CNT_EXACTO_COMMANDER];

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
    struct mail4thread *_trg_lthread;
    _trg_lthread = self;
    _trg_lthread->result = 0;
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
    for (uint8_t i = 0; i < MAIL_ID_CNT_EXACTO_COMMANDER; i++)
    {
        AppenderData[i].datalen = 0;
        AppenderData[i].data2copylen = 0;
        AppenderData[i].result = 0xFF;
        lthread_init(&AppenderData[i].thread, &appenderData_handler);
    }
    for (uint8_t i = 0; i < GETT_ID_CNT_EXACTO_COMMANDER; i++)
    {
        GetterData[i].datalen = 0;
        GetterData[i].data2copylen = 0;
        GetterData[i].result = 0xFF;
        lthread_init(&GetterData[i].thread, &getterData_handler);

    }
    
    lthread_init(&GetterData.thread, &getterData_handler);
    return 0;
}
/* главный косяк тут в том, что запросить ID это необязательное мероприятие. Его можно миновать и получить жесть в работе */
uint8_t addAppenderExactoDataStorage(void)
{
    if (CntIDMail4Thread >= MAIL_ID_CNT_EXACTO_COMMANDER)
        return 0xFF;
    uint8_t res = CntIDMail4Thread;
    CntIDMail4Thread++;
    return res;
}
uint8_t appendDataToExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
{
    CHECK_ID
    // copy here in storage
    AppenderData[id].result = 0xFF;
    lthread_launch(&AppenderData[id].thread);
    return 0;
}
uint8_t getDataFromExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount);
{
    CHECK_ID
    return 0;
}
uint8_t checkExactoDataStorage( const uint8_t id);
{
    CHECK_ID
    uint8_t res = 0;
    GetterData[id].result = 0xFF;
    GetterData[id].data2copylen = 0;
    lthread_launch(&GetterData[id].thread);
    lthread_join(&GetterData[id].thread);
    res = GetterData[id].result;
    return res;
}
