#ifndef QBOOTSTRAP_H
#define QBOOTSTRAP_H

#include <QObject>
#include <QTextCodec>
#include <QString>
#include <QStringList>
#include <QList>

class QBootstrap: public QObject
{
    Q_OBJECT

    class QBootInfo
    {
    public:
        QBootInfo()
        {
            correct=false;
            bootDirectoryPath="/home/root/ksu_boot1";
            startupFileName="ksu_boot.strip";
            checksumFileName="ksu_boot.strip.md5";
            checksumString="";
        }
        bool correct;
        int cnt;
        QString bootDirectoryPath;
        QString startupFileName;
        QString checksumFileName;
        QString checksumString;
    };

    QTextCodec *codec=nullptr;
    QStringList bootPathList;
    QString workDirectoryPath;
    QList<QBootInfo> bootInfoList;
    int currentCounter=0;
    int targetIndex=-1;//index of bootInfoList for target program path
    int bootIndex=-1;//index of bootInfoList for bootstrap


    bool extractChecksumString(const QString &text, QString &checksumString) const;
    bool getChecksumString(const QString &checksumFileName, QString &checksumString) const;
    bool getCheckSum(const QString &fileName, const QString &checksumString) const;
    bool getCnt(const QString &cntFileName, int &cnt) const;
    bool getBootInfo(const QString &bootPath, QBootInfo &bootInfo) const;
    int cmpCnt(int one, int two) const;
    int incCnt(int cnt) const;
    void buildBootInfo();
    bool buildAllBootDirectory(const QStringList &pathList) const;

public:
    QBootstrap(QObject *parent=nullptr): QObject(parent){ codec=QTextCodec::codecForName("CP1251"); }
    bool create(const QStringList &bootPathList, const QString &workDirectoryPath);
    void destroy();

    bool bootstrap() const;
    bool programSoft(const QString &soft, QString &errorString);

signals:
    void programProgressSignal(int progress, QString stringProgress);
};

#endif // QBOOTSTRAP_H
