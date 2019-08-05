#include "qusbnotifier.h"
#include <QFile>
#include <QByteArray>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <QDebug>


void QUsbWorker::CheckMount()
{
    QRegExp exp("/media/usb-sd[a-z][0-9]");

    //find mount points
    file.seek(0);
    QByteArray bytes=file.readAll();
    QString text(bytes);
    QStringList lines=text.split("\n");
    QStringList mpoints;
    for(int i=0; i<lines.count(); ++i)
    {
        QStringList fields=lines.at(i).split(" ");
        if(2>fields.count()) continue;
        QString mpoint=fields.at(1);
        if(true==exp.exactMatch(mpoint))
        {
            mpoints.append(mpoint);
        }
    }
    //check mount
    for(int i=0; i<mpoints.count(); ++i)
    {
        if(false==Find(mpoints.at(i), MountPoints))
        {
            MountPoints.append(mpoints.at(i));
            emit MountSignal(mpoints.at(i), true);
        }
    }
    //check umount
    for(int i=0; i<MountPoints.count(); ++i)
    {
        if(false==Find(MountPoints.at(i), mpoints))
        {
            emit MountSignal(MountPoints.at(i), false);
            MountPoints.removeAt(i);
            --i;
        }
    }
}

bool QUsbWorker::Find(const QString &s, const QStringList &list) const
{
    for(int i=0; i<list.count(); ++i)
    {
        if(0==s.compare(list.at(i)))
            return true;
    }
    return false;
}

QUsbWorker::QUsbWorker():
    file("/proc/mounts")
{
}

QUsbWorker::~QUsbWorker()
{
    if(true==file.isOpen())
        file.close();
}

bool QUsbWorker::Init(void *data, int &err)
{
    Q_UNUSED(data);
    if(false==file.open(QFile::ReadOnly))
    {
        err=USBN_OPEN_FILE;
        return false;
    }
    CheckMount();
    return true;
}

void QUsbWorker::Work()
{
    struct pollfd pfd;
    pfd.fd=file.handle();
    pfd.events=POLLERR;

    while(true)
    {
        if(true==checkExit())
            break;

        pfd.revents=0;
        if(0<=poll(&pfd, 1, 100))
        {
            if(POLLERR & pfd.revents)
            {
                //mount points changed
                CheckMount();
            }
        }
    }
    if(true==file.isOpen())
        file.close();
}



void QUsbNotifier::MountSlot(const QString &mpoint, bool mount_flag)
{
    emit MountSignal(mpoint, mount_flag);
    if(true==mount_flag) qDebug() << "mount " << mpoint;
    else                 qDebug() << "unmount " << mpoint;
}

QUsbNotifier::QUsbNotifier()
{

}

QUsbNotifier::~QUsbNotifier()
{

}

bool QUsbNotifier::Start(int &err)
{
    Worker=new QUsbWorker;
    if(nullptr==Worker){ err=TASK_MEMORY_ERR; return false; }
    Controller *ctrl=new Controller(this);
    if(nullptr==ctrl)
    {
        delete Worker; Worker=nullptr;
        err=TASK_MEMORY_ERR;
        return false;
    }
    bool ret;
    ret=connect(Worker, SIGNAL(MountSignal(QString,bool)), this, SLOT(MountSlot(QString,bool)));
    if(false==ret)
    {
        delete ctrl; ctrl=nullptr;
        delete Worker; Worker=nullptr;
        err=TASK_CONNECT_ERR;
        return false;
    }
    if(false==ctrl->Create(Worker, err)){ return false; }
    return ctrl->WorkerStart(QThread::NormalPriority, nullptr, err);

}

