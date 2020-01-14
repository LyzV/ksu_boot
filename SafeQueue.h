#ifndef SAFE_TEMPLATED_QUEUE
#define SAFE_TEMPLATED_QUEUE

#include <QSemaphore>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include "SafeQueueInterface.h"


template <class T, int size>
class CSafeQueue: public ISafeQueue<T>
{
    T buf[size];
    int post_index, pend_index;
    QSemaphore freeElem, usedElem;

    inline int IncIndex(int idx)
    {
        if(idx>=(size-1)) return 0;
        return ++idx;
    }
public:
    CSafeQueue(): freeElem(size), usedElem(0){ post_index=pend_index=0; }

    virtual bool Pend(T &msg, int timeout=0)
    {
        if(false==usedElem.tryAcquire(1, timeout))
            return false;
        msg=buf[pend_index];
        pend_index=IncIndex(pend_index);
        freeElem.release();
        return true;
    }
    virtual bool Post(const T &msg, int timeout=0)
    {
        if(false==freeElem.tryAcquire(1, timeout))
            return false;
        buf[post_index]=msg;
        post_index=IncIndex(post_index);
        usedElem.release();
        return true;
    }
    int Qnty(void)
    {
        return usedElem.available();
    }
    void Clear(void)
    {
        T msg;
        while(true==Pend(msg, 1));
    }
};

#endif//SAFE_TEMPLATED_QUEUE
