#ifndef TASK_HPP
#define TASK_HPP

#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QTimer>

//Кода ошибок при создании задач
#define TASK_NO_ERR         0
#define TASK_MEMORY_ERR     1//Нехватка сободной памяти
#define TASK_RECREATE_ERR   2//Повторно создаётся рабочий
#define TASK_WORKPTR_ERR    3//Нулевой указатель на рабочего
#define TASK_CONNECT_ERR    4//Не удалось установить QObject::connetion
#define TASK_CAST_ERR       5//Ошибка приведения типа

#define TASK_OTHER_ERR      100//Начиная с этого номера специфические ошибки инициализации задач


class CTask: public QThread
{
    Q_OBJECT

    void (*task)(void *pdata);
    void *pdata;
    void run(void) override
    {
        if(NULL!=task)
            (*task)(pdata);
    }
public:
    CTask(void (*task)(void *), void *pdata);
    void Start(void *pdata, Priority prio);
};


/*************************************************************************
 * КЛАСС ЗАДАЧИ, У КОТОРОЙ МОЖНО ПЕРЕОПРЕДЕЛИТЬ ФУНКЦЮ РАБОТЫ (Work)
**************************************************************************/
class Controller;
class IWorker: public QObject
{
    Q_OBJECT

private:
friend class Controller;
    bool ExitFlag;
    QSemaphore Sema;
    QMutex Mutex;
    bool Result;
    int ErrorCode;

    void Exit(void)
    {
        Mutex.lock();
        ExitFlag=true;
        Mutex.unlock();
    }

protected:
    void *Data;//Extra thread data
    void setResult(bool result, int err)
    {
        Result=result;
        ErrorCode=err;
        Sema.release();//++Sema;
    }

public:
    IWorker(void): ExitFlag(false), Data(nullptr){}
    virtual ~IWorker(void){}

    bool getResult(int &err)
    {
        Sema.acquire();//--Sema
        err=ErrorCode;
        return Result;
    }
    virtual bool checkExit(void)
    {
        bool ret;
        Mutex.lock();
        ret=ExitFlag;
        Mutex.unlock();
        return ret;
    }
    virtual bool Init(void *data, int &err){ Data=data; err=0; return true; }
    virtual void Work(void){}

public slots:
    void InitSlot(void *data)
    {
        int err;
        bool result=Init(data, err);
        setResult(result, err);
        if(true==result)
            Work();
    }
};

class Controller: public QObject
{
    Q_OBJECT

    QThread WorkerThread;
protected:
    IWorker *Worker;
public:
     Controller(QObject *parent=nullptr): QObject(parent){ Worker=nullptr; }
    virtual ~Controller(void){ WorkerStop(); }

    virtual bool Create(IWorker *worker, int &err)
    {
        bool con_result;

        if(nullptr!=Worker){ err=TASK_RECREATE_ERR; return false; }
        if(nullptr==worker){ err=TASK_WORKPTR_ERR ; return false; }
        Worker=worker;

        Worker->moveToThread(&WorkerThread);
        con_result=connect(&WorkerThread, SIGNAL(finished()), Worker, SLOT(deleteLater()));
        if(false==con_result){ err=TASK_CONNECT_ERR; return false; }
        con_result=connect(this, SIGNAL(InitSignal(void *)), Worker, SLOT(InitSlot(void *)));
        if(false==con_result){ err=TASK_CONNECT_ERR; return false; }
        err=TASK_NO_ERR;
        return true;
    }
    virtual bool WorkerStart(QThread::Priority prio, void *data, int &err)
    {
        if(nullptr==Worker){ err=TASK_WORKPTR_ERR ; return false; }
        emit InitSignal(data);
        WorkerThread.start(prio);
        return Worker->getResult(err);
    }
    virtual void WorkerStop(void)
    {
        if(false==WorkerThread.isFinished())
        {
            Worker->Exit();
            WorkerThread.quit();
            WorkerThread.wait();
            Worker=nullptr;
        }
    }

signals:
    void InitSignal(void *data);
};

/*************************************************************************
 * КЛАСС ЗАДАЧИ, КОТОРАЯ РЕГУЛЯРНО ВЫЗЫВАЕТСЯ С ЗАДАННЫМ ПЕРИОДОМ
**************************************************************************/
class PWorker: public IWorker
{
    Q_OBJECT

    QTimer *timer;
    int period;

private slots:
    void OnTimerSlot(void){ OnTimer(); }
public:
    PWorker(int period=100){ this->period=period; }
    virtual bool Init(void *pdata, int &err)
    {
        bool con_result;
        Data=pdata;

        timer=new QTimer(this);
        if(nullptr==timer){ err=TASK_MEMORY_ERR; return false; }
        con_result=connect(timer, SIGNAL(timeout()), this, SLOT(OnTimerSlot()));
        if(false==con_result){ err=TASK_CONNECT_ERR; return false; }
        timer->start(period);
        err=TASK_NO_ERR;
        return true;
    }
    virtual void OnTimer(void){}
};

//Функция для запуска периодической задачи PWorker
extern
bool StartPeriodicallyTask(
                            QObject *parent         ,//Родитель
                            PWorker *worker         ,//Период задачи
                            QThread::Priority prio  ,//Приоритет задачи
                            void *pdata             ,//Дополнительные данные задачи
                            int &err                 //Код ошибки при инициализации задачи
                          );


#endif // TASK_HPP
