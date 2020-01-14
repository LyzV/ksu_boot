#ifndef PRIVATE_CAN_H
#define PRIVATE_CAN_H

#include "task.hpp"
#include <linux/can.h>// struct can_frame
#include "can.h"// BUF_CAN
#include "SafeQueue.h"

struct CanDrvData
{
    int s;//socket
    ISafeQueuePost<struct can_frame> *isq_post;//for post into queue
};

class CanDrv: public IWorker
{
    CanDrvData *Data;
    virtual void Work(void);
public:
    virtual bool Init(void *pdata, int &err);
};


class CanDrvController: public Controller
{
    CSafeQueue<struct can_frame, 32> Frames;
    CanDrvData DrvData;

public:
    CanDrvController(QObject *parent):
        Controller(parent)
    {
        DrvData.s=-1;
        DrvData.isq_post=&Frames;
    }
    virtual bool Create(CanDrv *worker, int &err);
    virtual bool WorkerStart(QThread::Priority prio, void *data, int &err);

    bool Send(quint8 dest_addr, const BUF_CAN *p_rx);
    bool Recv(quint8 from_addr,       BUF_CAN *p_rx, int timeout);
    void Clear(void);
};



#endif // PRIVATE_CAN_H
