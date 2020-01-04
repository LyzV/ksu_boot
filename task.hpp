#ifndef TASK_HPP
#define TASK_HPP

#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QTimer>
#include <QString>

//Кода ошибок при создании задач
#define TASK_NO_ERR         0
#define TASK_MEMORY_ERR     1//Нехватка сободной памяти
#define TASK_NULLPTR_ERR    2//Нулевой указатель
#define TASK_RECREATE_ERR   3//Повторно создаётся рабочий
#define TASK_WORKPTR_ERR    4//Нулевой указатель на рабочего
#define TASK_CONNECT_ERR    5//Не удалось установить QObject::connection
#define TASK_CAST_ERR       6//Ошибка приведения типа
#define TASK_OPENFILE_ERR   7//Не могу открыть файл
#define TASK_READFILE_ERR   8//Не могу прочесть файл
#define TASK_RESOURCE_ERR   9//Аппаратный ресурс отсутствует

#define TASK_OTHER_ERR(n)   (100+(n))

#define TASK_SND_ERR        TASK_OTHER_ERR(1)//Ошибка инициализации sound
#define TASK_GPIO_ERR       TASK_OTHER_ERR(2)//Ошибка инициализации GPIO
#define TASK_ADC_ERR        TASK_OTHER_ERR(3)//Ошибка инициализации АЦП
#define TASK_CLOCK_ERR      TASK_OTHER_ERR(4)//Ошибка инициализации RTC
#define TASK_MEASJRNL_ERR   TASK_OTHER_ERR(5)//Ошибка создания журнала измерений
#define TASK_GDIJRNL_ERR    TASK_OTHER_ERR(6)//Ошибка создания журнала ГДИ
#define TASK_MAP_ERR        TASK_OTHER_ERR(7)//Ошибка создания карт памяти
#define TASK_ZNK_ERR        TASK_OTHER_ERR(8)//Ошибка создания модуля ZNK
#define TASK_MEI_ERR        TASK_OTHER_ERR(9)//Ошибка создания модуля MEI
#define TASK_ETHER_ERR      TASK_OTHER_ERR(10)//Ошибка создания Ethernet сервера

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
    IWorker(QString name=""): ExitFlag(false), Data(nullptr){ setObjectName(name); }
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
        WorkerThread.setObjectName(Worker->objectName());
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
    PWorker(QString name, int period=100): IWorker(name) { this->period=period; }
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
