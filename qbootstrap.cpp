#include "qbootstrap.h"
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QRegularExpression>
#include <QByteArray>
#include "bash_cmd.h"

bool QBootstrap::findInList(const QStringList &list, const QString &findString)
{
    for(int i=0; i<list.count(); ++i)
    {
        if(list.at(i)==findString)
            return true;
    }
    return false;
}

int QBootstrap::choiceLastCreated(const QStringList &pathList, const QStringList &exceptPathList, QDateTime &lastCreated)
{
    int returnValue=-1;
    lastCreated=QDateTime(QDate(1, 1, 1), QTime(0, 0));
    for(int i=0; i<pathList.count(); ++i)
    {
        QDir bootDirectory(pathList.at(i));
        if(false==bootDirectory.exists())
            continue;
        if(true==findInList(exceptPathList, pathList.at(i)))
            continue;

        QFileInfoList fileInfoList=bootDirectory.entryInfoList();
        for(int j=0; j<fileInfoList.count(); ++j)
        {
            QFileInfo fileInfo(fileInfoList.at(j));
            if(false==fileInfo.exists())
                continue;
            if(false==fileInfo.isFile())
                continue;
            if(false==fileInfo.isExecutable())
                continue;
            QDateTime created=fileInfo.created();
            if(created>lastCreated)
            {
                lastCreated=created;
                returnValue=i;
            }
        }
    }
    return returnValue;
}

int QBootstrap::choiceFirstCreated(const QStringList &pathList, const QStringList &exceptPathList, QDateTime &firstCreated)
{
    int returnValue=-1;
    firstCreated=QDateTime::currentDateTime();
    for(int i=0; i<pathList.count(); ++i)
    {
        QDir bootDirectory(pathList.at(i));
        if(false==bootDirectory.exists())
            continue;
        if(true==findInList(exceptPathList, pathList.at(i)))
            continue;

        QFileInfoList fileInfoList=bootDirectory.entryInfoList();
        for(int j=0; j<fileInfoList.count(); ++j)
        {
            QFileInfo fileInfo(fileInfoList.at(j));
            if(false==fileInfo.exists())
                continue;
            if(false==fileInfo.isFile())
                continue;
            if(false==fileInfo.isExecutable())
                continue;
            QDateTime created=fileInfo.created();
            if(created<firstCreated)
            {
                firstCreated=created;
                returnValue=i;
            }
        }
    }
    return returnValue;
}

bool QBootstrap::extractChecksumString(const QString &text, QString &checksumString)
{
    QStringList words=text.split(" ");// (QRegExp(" \t")); //(QRegularExpression(" \t"));
    checksumString=words.at(0);
    checksumString=checksumString.trimmed();
    if(0==checksumString.count())
        return false;
    //a4243103d878ea8b1b09f7d602f9459b
    for(int i=0; i<checksumString.count(); ++i)
    {
        QChar ch=checksumString.at(i);
        if(true==ch.isLetter())
        {
            if(!(('a'<=ch)&&('f'>=ch)))
                return false;
        }
        else if(false==ch.isNumber())
            return false;
    }
    return true;
}

bool QBootstrap::getChecksumString(const QString &checksumFileName, QString &checksumString)
{
    QFile checksumFile(checksumFileName);
    if(false==checksumFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream stream(&checksumFile);
    QString allText=stream.readAll();
    checksumFile.close();
    if(false==extractChecksumString(allText, checksumString))
        return false;
    return true;
}

bool QBootstrap::checkSum(const QString &fileName, const QString &checksumString)
{
    QProcess md5sum;
    QStringList programArguments;
    programArguments << fileName;
    md5sum.start("md5sum", programArguments);
    if(false==md5sum.waitForFinished())
        return false;
    QProcess::ExitStatus exitStatus=md5sum.exitStatus();
    if(QProcess::NormalExit!=exitStatus)
        return false;
    QString allOutput(md5sum.readAllStandardOutput());
    QString md5sumChecksumString;
    if(false==extractChecksumString(allOutput, md5sumChecksumString))
        return false;
    if(md5sumChecksumString!=checksumString)
        return false;
    return true;
}

bool QBootstrap::getBootInfo(const QString &bootPath, QBootstrap::QBootInfo &bootInfo)
{
    bootInfo.correct=false;

    QDir bootDirectory(bootPath);
    if(false==bootDirectory.exists())
        return false;
    bootInfo.bootDirectoryPath=bootDirectory.absolutePath();

    QFileInfoList fileInfoList=bootDirectory.entryInfoList();
    bootInfo.startupFileName="";
    for(int i=0; i<fileInfoList.count(); ++i)
    {
        QFileInfo startupFileInfo(fileInfoList.at(i));
        if(false==startupFileInfo.exists())
            continue;
        if(false==startupFileInfo.isFile())
            continue;
        if(false==startupFileInfo.isExecutable())
            continue;
        bootInfo.startupFileName=startupFileInfo.fileName();
        bootInfo.createdDateTime=startupFileInfo.created();
        bootInfo.checksumFileName=bootInfo.startupFileName + ".md5";
        QFile checksumFile(bootPath + "/" + bootInfo.checksumFileName);
        if(false==checksumFile.exists())
            continue;
        if(false==getChecksumString(checksumFile.fileName(), bootInfo.checksumString))
            continue;
        if(false==checkSum(startupFileInfo.absoluteFilePath(), bootInfo.checksumString))
            continue;

        //Yes! I find it!
        bootInfo.correct=true;
        return true;
    }
    return false;
}

void QBootstrap::printBootInfo(const QBootstrap::QBootInfo &bootInfo)
{
    Q_UNUSED(bootInfo);
//    traceObject.trace("\n*** " + bootInfo.bootDirectoryPath + " ***");
//    traceObject.trace("correct= " + bootInfo.correct);
//    traceObject.trace("startupFileName= " + bootInfo.startupFileName);
//    traceObject.trace("createdDateTime= " + bootInfo.createdDateTime.toString());
//    traceObject.trace("checksumFileName= " + bootInfo.checksumFileName);
//    traceObject.trace("checksumString= " + bootInfo.checksumString);
}

bool QBootstrap::buildAllBootDirectory(const QStringList &pathList)
{
    for(int i=0; i<pathList.count(); ++i)
    {
        QDir directory(pathList.at(i));
        if(true==directory.exists())
            continue;

        if(false==directory.mkpath(pathList.at(i)))
            return false;
    }
    return true;
}

void QBootstrap::printBootPath(const QString &bootPath)
{
    Q_UNUSED(bootPath);
//    QBootInfo bootInfo;
//    if(true==getBootInfo(bootPath, bootInfo))
//    {
//        printBootInfo(bootInfo);
//    }
//    else
//    {
//        traceObject.trace("\n*** " + bootPath + " ***");
//        traceObject.trace("It is not correct boot path!");
//    }
}

void QBootstrap::printBootPathList(const QStringList &bootPathList)
{
    Q_UNUSED(bootPathList);
//    for(int i=0; i<bootPathList.count(); ++i)
//    {
//        QString bootPath=bootPathList.at(i);
//        printBootPath(bootPath);
//    }
}

bool QBootstrap::bootstrap(const QString &bootPath, const QString &workDirectory)
{
    QBootInfo bootInfo;
    if(false==getBootInfo(bootPath, bootInfo))
    {
        //printBootPath(bootPath);
        return false;
    }
    //printBootInfo(bootInfo);

    QProcess bootProgram;
    QString programFullName=bootInfo.bootDirectoryPath + "/" +bootInfo.startupFileName;
    return bootProgram.startDetached(programFullName, QStringList(), workDirectory);
}

bool QBootstrap::bootstrap(const QStringList &bootPathList, const QString &workDirectory)
{
    //printBootPathList(bootPathList);

    QDateTime lastCreated;
    int lastCreatedIndex=choiceLastCreated(bootPathList, QStringList(), lastCreated);
    if(0>lastCreatedIndex)
        return false;
    if(lastCreatedIndex>=bootPathList.count())
        return false;
    return bootstrap(bootPathList.at(lastCreatedIndex), workDirectory);

}

bool QBootstrap::programSoft(const QStringList &bootPathList, const QString &soft)
{
    if(1>bootPathList.count())
    {
        return false;
    }
    if(false==buildAllBootDirectory(bootPathList))
    {
        return false;
    }

    QDateTime firstCreated;
    int firstCreatedIndex=choiceFirstCreated(bootPathList, QStringList(), firstCreated);
    if(0>firstCreatedIndex)
        firstCreatedIndex=0;
    else
    {
        QDateTime lastCreated;
        int lastCreatedIndex=choiceLastCreated(bootPathList, QStringList(), lastCreated);
        if(lastCreatedIndex==firstCreatedIndex)
        {//get next index
            ++firstCreatedIndex;
            if(firstCreatedIndex>=bootPathList.count())
                firstCreatedIndex=0;
        }
    }

    QString bootPath=bootPathList.at(firstCreatedIndex);

    // rm -rf bootPath
    QDir bootDir(bootPath);
    bootDir.removeRecursively();

    // mkdir bootPath
    if(false==bootDir.mkpath(bootPath))
    {
        return false;
    }

    // tar -xf soft -C bootInfo.bootDirectoryPath
    QProcess process;
    QStringList arguments;
    QProcess::ExitStatus exitStatus;
    arguments << "-xf" << soft << "-C" << bootPath;
    process.start("tar", arguments);
    if(false==process.waitForFinished())
    {
        return false;
    }
    exitStatus=process.exitStatus();
    if(QProcess::NormalExit!=exitStatus)
    {
        return false;
    }

    // verify
    QBootInfo bootInfo;
    if(false==getBootInfo(bootPath, bootInfo))
    {
        return false;
    }

    //chmod +x startupFileName
    QBashCmd::set_file_execute_mode(bootInfo.bootDirectoryPath + "/" + bootInfo.startupFileName);
    // sync; drop_caches
    QBashCmd::flush_storage();

    return true;
}
