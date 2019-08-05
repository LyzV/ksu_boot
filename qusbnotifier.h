#ifndef QUSBNOTIFIER_H
#define QUSBNOTIFIER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include "task.hpp"

//Errors
#define USBN_FILE_EXISTS  (TASK_OTHER_ERR+1)
#define USBN_OPEN_FILE    (TASK_OTHER_ERR+2)


class QUsbWorker: public IWorker
{
    Q_OBJECT

    QFile file;// /proc/mounts
    QStringList MountPoints;
    void CheckMount();
    bool Find(const QString &s, const QStringList &list) const;

public:
    QUsbWorker();
    virtual ~QUsbWorker();
    virtual bool Init(void *data, int &err) override;
    virtual void Work(void) override;

signals:
    void MountSignal(const QString &mpoint, bool mount_flag);
};

class QUsbNotifier: public QObject
{
    Q_OBJECT

    QUsbWorker *Worker=nullptr;

private slots:
    void MountSlot(const QString &mpoint, bool mount_flag);

public:
    explicit QUsbNotifier();
    virtual ~QUsbNotifier();
    bool Start(int &err);

signals:
    void MountSignal(const QString &mpoint, bool mount_flag);
};

#endif // QUSBNOTIFIER_H
