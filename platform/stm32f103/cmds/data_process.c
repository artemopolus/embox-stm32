#include <errno.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>
#include <kernel/lthread/lthread_sched_wait.h>
static struct lthread data_process_main_lth;
static struct mutex data_process_main_mx;
static int data_process_not_empty;

static int data_process_run_lth(struct lthread *self)
{
    goto *lthread_resume(self, &&start);
start:
    /* инициализация */
    data_process_not_empty = 0;
update:
    /* проверка наличия данных */
	if ( SCHED_WAIT_TIMEOUT_LTHREAD(self, data_process_not_empty, SCHED_TIMEOUT_INFINITE) == -EAGAIN)
    {
        return lthread_yield(&&start, &&update);
    }
mutex_retry:
    /* заняты ли данные */
    if (mutex_trylock_lthread(self, &data_process_main_mx) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    /* работа с данными */

    mutex_unlock_lthread(self, &data_process_main_mx);
    return update;
}

int main(int argc, char **argv)
{

    mutex_init_schedee(&data_process_main_mx);
    lthread_init(&data_process_main_lth, data_process_run_lth);
}