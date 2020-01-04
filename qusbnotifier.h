#ifndef QUSBNOTIFIER_H
#define QUSBNOTIFIER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include "task.hpp"

class QUsbWorker: public IWorker
{
    Q_OBJECT

    QFile file;// /proc/mounts
    QStringList MountPoints;
    QStringList MountDevices;
    void addPoint(const QString mpoint, const QString device);
    void removePoint(int pos);
    bool CheckStorage(const QString &mpoint, const QString &device);
    void CheckMount();
    bool Find(const QString     &mp     , const QString     &dev,
              const QStringList &mpoints, const QStringList &devices) const;

public:
    QUsbWorker();
    virtual ~QUsbWorker();
    virtual bool Init(void *data, int &err) override;
    virtual void Work(void) override;

signals:
    void MountSignal(const QString &mpoint, const QString &device, bool mount_flag);
};

#endif // QUSBNOTIFIER_H
