#ifndef LIBUTIL_QUEUE_H
#define LIBUTIL_QUEUE_H 1

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <libutil/log.h>
#include <libutil/arch.h>
#include <libutil/util.h>
#include <libutil/3rd/utarray.h>

struct data_holder__
{
    void * data;
};

typedef struct data_holder__ queue_data_t;
typedef struct data_holder__ * queue_data_array_t;


struct lu_queue
{
    pthread_cond_t cond;
    pthread_mutex_t lock;
    UT_array * msgs;
};


#define lu_queue_init(q)                            \
do {                                                \
    UT_icd data_holder_icd = {                      \
        sizeof(struct data_holder__),               \
        NULL, NULL, NULL                            \
    };                                              \
    utarray_new((q)->msgs, &data_holder_icd);       \
    SYS(pthread_mutex_init(&(q)->lock, NULL));      \
    SYS(pthread_cond_init(&(q)->cond, NULL));       \
} while(0)


inline static
void
lu_queue_destroy(struct lu_queue * q)
{
    assert(q != NULL);
    utarray_free(q->msgs);
    SYS(pthread_mutex_destroy(&q->lock));
    SYS(pthread_cond_destroy(&q->cond));
}


inline static
void
lu_queue_product(struct lu_queue * q, void * data)
{
    assert(q != NULL);
    struct data_holder__ item = { data };
    SYS(pthread_mutex_lock(&q->lock));
    utarray_push_back(q->msgs, &item);
    SYS(pthread_mutex_unlock(&q->lock));
    SYS(pthread_cond_signal(&q->cond));
}


inline static
size_t
lu_queue_timed_consume(struct lu_queue * q,
    queue_data_array_t items, size_t size, size_t sec)
{
    assert(q != NULL);
    int rc;
    struct timespec ts;
    size_t consumed_items = 0;
    struct data_holder__ * it;

    SYS(pthread_mutex_lock(&q->lock));
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
    ts.tv_sec += sec;
    rc = pthread_cond_timedwait(&q->cond, &q->lock, &ts);
    if (unlikely(rc != ETIMEDOUT && rc != 0))
        DIE("pthread_cond_timedwait %s", strerror(errno));
    else {
        while ((it = (struct data_holder__ *)utarray_back(q->msgs)))
        {
            if (unlikely(consumed_items == size))
                break;
            items[consumed_items].data = it->data;
            ++consumed_items;
            utarray_pop_back(q->msgs);
        }
    }
    SYS(pthread_mutex_unlock(&q->lock));
    return consumed_items;
}


#endif

