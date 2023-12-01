#include "thread.h"

int thread_mutex_create(pthread_mutex_t *mtx)
{
    int err;
    pthread_mutexattr_t attr;

    err = pthread_mutexattr_init(&attr);
    if (err != 0)
    {
        fprintf(stderr, "pthread_mutexattr_init() failed, reason: %s\n", strerror(errno));
        return T_ERROR;
    }
    /**
     * PTHREAD_MUTEX_ERRORCHECK_NP, 检错锁，如果同一个线程请求同一个锁
     * 返回EDEADLK，否则与PTHREAD_MUTEX_TIME_NP类型动作相同
     * 这样就保证当不允许多次加锁时，不会出现最简单情况的死锁
     * ---------------------------------------------------------
     * PTHREAD_MUTEX_TIME_NP, 缺省值，普通锁
     * 当一个线程加锁以后，其余请求锁的现场将形成一个等待队列
     * 并在解锁按优先级获得锁，保证资源分配公平性
     */
    err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    if (err != 0)
    {
        fprintf(stderr, "pthread_mutexattr_settype() failed, reason: %s\n", strerror(errno));
        return T_ERROR;
    }

    err = pthread_mutex_init(mtx, &attr);
    if (err != 0)
    {
        fprintf(stderr, "pthread_mutex_init() failed, reason: %s\n", strerror(errno));
        return T_ERROR;
    }

    err = pthread_mutexattr_destroy(&attr);
    if (err != 0)
    {
        fprintf(stderr, "pthread_mutexattr_destroy() failed, reason: %s\n", strerror(errno));
    }

    return T_OK;
}

int thread_mutex_destroy(pthread_mutex_t *mtx)
{
    int err;

    err = pthread_mutex_destroy(mtx);
    if (err != 0)
    {
        fprintf(stderr, "pthread_mutex_destroy() failed, reason: %s\n", strerror(errno));
        return T_ERROR;
    }

    return T_OK;
}

int thread_mutex_lock(pthread_mutex_t *mtx)
{
    int err;

    err = pthread_mutex_lock(mtx);
    if (err == 0)
    {
        return T_OK;
    }

    fprintf(stderr, "pthread_mutex_lock() failed, reason: %s\n", strerror(errno));

    return T_ERROR;
}

int thread_mutex_unlock(pthread_mutex_t *mtx)
{
    int err;

    err = pthread_mutex_unlock(mtx);

    if (err == 0)
    {
        return T_OK;
    }

    fprintf(stderr, "pthread_mutex_unlock() failed, reason: %s\n", strerror(errno));

    return T_ERROR;
}
