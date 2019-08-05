#include "task.hpp"
#include <QtDebug>

CTask::CTask(void (*task)(void *), void *pdata)
{
    this->task=task;
    if(NULL==this->task) qDebug("CTask. Error: task==NULL");
    this->pdata=pdata;
}

void CTask::Start(void *pdata, Priority prio)
{
    this->pdata=pdata;
    start(prio);
}

bool StartPeriodicallyTask(QObject *parent, PWorker *worker, QThread::Priority prio, void *pdata, int &err)
{
    if(nullptr==worker){ err=TASK_MEMORY_ERR; return false; }
    Controller *ctrl=new Controller(parent);
    if(nullptr==ctrl){ err=TASK_MEMORY_ERR; return false; }
    if(false==ctrl->Create(worker, err)){ return false; }
    return ctrl->WorkerStart(prio, pdata, err);
}
