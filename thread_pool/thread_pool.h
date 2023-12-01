#ifndef _THREAD_POOL_H_INCLUDED_
#define _THREAD_POOL_H_INCLUDED_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "thread.h"

#define DEFAULT_THREADS_NUM 4
#define DEFAULT_QUEUE_NUM 65535

typedef unsigned long atomic_uint_t;
typedef struct thread_task_s thread_task_t;
typedef struct thread_pool_s thread_pool_t;

struct thread_task_s
{
    thread_task_t *next;
    uint_t id;
    void *ctx; // 上下文
    void (*handler)(void *data);
};

typedef struct
{
    thread_task_t *first;
    thread_task_t **last;
} thread_pool_queue_t;

#define thread_pool_queue_init(q) \
    (q)->first = NULL;            \
    (q)->last = &(q)->first

struct thread_pool_s
{
    pthread_mutex_t mtx;
    thread_pool_queue_t queue; // 任务队列
    int_t waiting;             // 待处理任务数量
    pthread_cond_t cond;

    char *name;
    uint_t threads;  // 线程池中线程数量
    int_t max_queue; // 最大队列长度
};

thread_task_t *thread_task_alloc(size_t size);
void thread_task_free(thread_task_t* task);
int_t thread_task_post(thread_pool_t *tp, thread_task_t *task);
thread_pool_t *thread_pool_init();
void thread_pool_destroy(thread_pool_t *tp);

#ifdef __cplusplus
}
#endif

#endif /* _NGX_THREAD_POOL_H_INCLUDED_ */
