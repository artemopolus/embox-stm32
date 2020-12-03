#include "exacto_data_storage.h"
#include <embox/unit.h>

#define CHECK_ID_THREAD_CONTROL(ID)    if (ID >= THREAD_CONTROL_CNT )    return 1;

typedef struct{
    uint8_t isEmpty;
    struct mutex dtmutex;
}exactodatastorage;
// static struct mutex MutexOfExactoDataStorage;
static exactodatastorage ExDtStorage = {
    .isEmpty = 0,
};

typedef struct{
    uint8_t id_count;
    uint8_t id_max;
    struct mutex mx;
}data_controller_t;

static uint8_t DataControllerIDCnt = 0;

typedef enum{
    WAIT = 0,
    UNKNOWN_ERROR,
    NO_RESULT = 0xFF
}thread_control_result_t;


//LIST OF FUNCTONS

void appendThreadControl(thread_control_t * thread);
void getThreadControl(thread_control_t * thread);
void checkThreadControl(thread_control_t * thread);


static thread_control_t DataController[THREAD_CONTROL_CNT];

// LIST OF THREADS

static int operateDataController(struct lthread * self)
{
    return 0;
}

static int functionForExDtStorageHandler(struct lthread *self)
{
    thread_control_t *_trg_lthread;
    goto *lthread_resume(self, &&start);
start:
     /* инициализация */

mutex_retry:
    // do       something
    if (mutex_trylock_lthread(self, &_trg_lthread->mx ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    //===============================================================
    if (mutex_trylock_lthread(self, &ExDtStorage.dtmutex ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    _trg_lthread = (thread_control*)&self;
    _trg_lthread->result = NO_RESULT;
    switch (_trg_lthread->fun_type)
    {
    case APPEND:
        /* code */
        break;
    case GET:
        break;
    case CHECK:
        break;
    default:
        break;
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
    lthread_launch(&DataController[i].thread);
    return 0;
}
uint8_t getDataFromExactoDataStorage( const uint8_t id, uint8_t * data, const uint8_t datacount)
{
    return 0;
}
uint8_t checkExactoDataStorage( const uint8_t id)
{
    return res;
}
