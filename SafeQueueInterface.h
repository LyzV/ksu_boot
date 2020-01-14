#ifndef SAFE_TEMPLATED_QUEUE_INTERFACE
#define SAFE_TEMPLATED_QUEUE_INTERFACE

template <class T>
class ISafeQueuePost
{
public:
    virtual bool Post(const T &msg, int timeout=0)=0;
};	

template <class T>
class ISafeQueuePend
{
public:
    virtual bool Pend(T  &msg, int timeout=0)=0;
};

template <class T>
class ISafeQueue: public ISafeQueuePost<T>, 
                  public ISafeQueuePend<T>
{
};

#endif	
