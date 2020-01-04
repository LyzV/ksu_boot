#include "qusbnotifier.h"
#include <QFile>
#include <QDir>
#include <QStorageInfo>
#include <QByteArray>
#include <poll.h>
#include <QDebug>


void QUsbWorker::addPoint(const QString mpoint, const QString device)
{
    MountPoints .append(mpoint);
    MountDevices.append(device);
}
void QUsbWorker::removePoint(int pos)
{
    if(pos>=MountPoints .count()) return;
    if(pos>=MountDevices.count()) return;
    MountPoints .removeAt(pos);
    MountDevices.removeAt(pos);
}

bool QUsbWorker::CheckStorage(const QString &mpoint, const QString &device)
{
    QDir dir(mpoint);
    if(!dir.isAbsolute())
        return false;
    if(!dir.exists())
        return false;
    QFile dev_file(device);
    if(!dev_file.exists())
        return false;

    QList<QStorageInfo> list=QStorageInfo::mountedVolumes();
    for(int i=0; i<list.count(); ++i)
    {
        QStorageInfo sinfo=list.at(i);
        if(
           (sinfo.isValid()) &&
           (sinfo.device()==device) &&
           (sinfo.rootPath()==mpoint) &&
           (sinfo.isReadOnly()==false) &&
           (sinfo.fileSystemType()=="vfat")
          )
        {
            return true;
        }
    }
    return false;
}

void QUsbWorker::CheckMount()
{
    QRegExp exp("/media/usb-sd[a-z][0-9]");

    //find mount points
    file.seek(0);
    QByteArray bytes=file.readAll();
    QString text(bytes);
    QStringList lines=text.split("\n");
    QStringList mpoints;
    QStringList devices;
    for(int i=0; i<lines.count(); ++i)
    {
        QStringList fields=lines.at(i).split(" ");
        if(2>fields.count()) continue;
        QString mpoint=fields.at(1);
        if(true==exp.exactMatch(mpoint))
        {
            QString device=fields.at(0);
            mpoints.append(mpoint);
            devices.append(device);
        }
    }
    //check mount
    for(int i=0; i<mpoints.count(); ++i)
    {
        if(
           (false==Find(mpoints.at(i), devices.at(i), MountPoints, MountDevices))&&
           (true ==CheckStorage(mpoints.at(i), devices.at(i)))
          )
        {
            addPoint(mpoints.at(i), devices.at(i));
            emit MountSignal(mpoints.at(i), devices.at(i), true);
        }
    }
    //check umount
_again:
    for(int i=0; i<MountPoints.count(); ++i)
    {
        if(false==Find(MountPoints.at(i), MountDevices.at(i), mpoints, devices))
        {
            emit MountSignal(MountPoints.at(i), MountDevices.at(i), false);
            removePoint(i);
            goto _again;
        }
    }
}



bool QUsbWorker::Find(const QString     &mp     , const QString     &dev,
                      const QStringList &mpoints, const QStringList &devices) const
{
    for(int i=0; i<mpoints.count(); ++i)
    {
        if(
           (0==mp .compare(mpoints.at(i))) &&
           (0==dev.compare(devices.at(i)))
          )
            return true;
    }
    return false;
}

QUsbWorker::QUsbWorker(void):
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
        err=TASK_OPENFILE_ERR;
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

