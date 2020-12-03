#include "exacto_data_storage.h"
#include <embox/unit.h>

#define CHECK_ID_THREAD_CONTROL(ID)    if (ID >= THREAD_CONTROL_CNT )    return 1;

// static struct mutex MutexOfExactoDataStorage;
exactodatastorage ExDtStorage = {
    .isEmpty = 1,
};

typedef struct{
    uint8_t id_count;
    uint8_t id_max;
    struct mutex mx;
}data_controller_t;

static uint8_t DataControllerIDCnt = 0;



//LIST OF FUNCTONS

void appendThreadControl(thread_control_t * thread);
void getThreadControl(thread_control_t * thread);
void checkThreadControl(thread_control_t * thread);


static thread_control_t DataController[THREAD_CONTROL_CNT];

// LIST OF THREADS



static int functionForExDtStorageHandler(struct lthread *self)
{
    thread_control_t *_trg_lthread;
    goto *lthread_resume(self, &&start);
start:
     /* инициализация */
    _trg_lthread = (thread_control_t*)&self;

mutex_retry:
    // do       something
    if (mutex_trylock_lthread(self, &_trg_lthread->mx ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    //===============================================================
    _trg_lthread->result = NO_RESULT;
    if (mutex_trylock_lthread(self, &ExDtStorage.dtmutex ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }

    if (!ExDtStorage.isEmpty) 
    {
        _trg_lthread->result = OK;
    }

    mutex_unlock_lthread(self, &ExDtStorage.dtmutex);
    //===============================================================
    mutex_unlock_lthread(self, &_trg_lthread->mx);

    // after    something
    return 0;
}



EMBOX_UNIT_INIT(initExactoDataStorage);
static int initExactoDataStorage(void)
{
    mutex_init_schedee(&ExDtStorage.dtmutex);
    for (uint8_t i = 0; i < THREAD_CONTROL_CNT; i++)
    {
        lthread_init(&DataController[i].thread, &functionForExDtStorageHandler);
    }
   
    // lthread_init(&GetterData.thread, &getterData_handler);
    return 0;
}
/* главный косяк тут в том, что запросить ID это необязательное мероприятие. Его можно миновать и получить жесть в работе */
uint8_t addAppenderExactoDataStorage(void)
{
    uint8_t res = DataControllerIDCnt;
    DataControllerIDCnt++;
    return res;
}
uint8_t appendDataToExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount)
{
    CHECK_ID_THREAD_CONTROL(id)
    // copy here in storage
    lthread_launch(&DataController[id].thread);
    return 0;
}
uint8_t getDataFromExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount)
{
    return 0;
}
uint8_t checkExactoDataStorage( thread_control_t * base)
{
    lthread_launch(&base->thread);
    return 0;
}
uint8_t initThreadExactoDataStorage( thread_control_t * base )
{
    mutex_init_schedee(&base->mx);
    lthread_init(&base->thread, &functionForExDtStorageHandler);
    base->result = WAIT;
    return 0;
}
