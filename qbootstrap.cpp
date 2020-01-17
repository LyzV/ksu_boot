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
#include <inttypes.h>

#define TU(s) codec->toUnicode(s)
#define MAX_CNT 7

bool QBootstrap::extractChecksumString(const QString &text, QString &checksumString) const
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

bool QBootstrap::getChecksumString(const QString &checksumFileName, QString &checksumString) const
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

bool QBootstrap::getCheckSum(const QString &fileName, const QString &checksumString) const
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

bool QBootstrap::getCnt(const QString &cntFileName, int &cnt) const
{
    QFile cntFile(cntFileName);
    if(false==cntFile.exists())
        return false;

    if(false==cntFile.open(QIODevice::ReadOnly))
        return false;
    QTextStream stream(&cntFile);
    QString allText=stream.readAll();
    cntFile.close();

    allText=allText.trimmed();
    bool ok;
    cnt=allText.toInt(&ok);
    if(false==ok)
        return false;

    return true;
}

bool QBootstrap::getBootInfo(const QString &bootPath, QBootstrap::QBootInfo &bootInfo) const
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
        bootInfo.checksumFileName=bootInfo.startupFileName + ".md5";
        QFile checksumFile(bootPath + "/" + bootInfo.checksumFileName);
        if(false==checksumFile.exists())
            continue;
        if(false==getChecksumString(checksumFile.fileName(), bootInfo.checksumString))
            continue;
        if(false==getCheckSum(startupFileInfo.absoluteFilePath(), bootInfo.checksumString))
            continue;
        QString cntFileName=startupFileInfo.absolutePath() + "/cnt";
        if(false==getCnt(cntFileName, bootInfo.cnt))
            continue;

        //Yes! I find it!
        bootInfo.correct=true;
        return true;
    }
    return false;
}

int QBootstrap::cmpCnt(int one, int two) const
{
    if(one==two) return 0;

    if(one>two)
    {
        if((one-two)>(MAX_CNT>>1))
            return -1;
        else
            return 1;
    }
    else
    {
        if((two-one)>(MAX_CNT>>1))
            return 1;
        else
            return -1;
    }
}

int QBootstrap::incCnt(int cnt) const
{
    ++cnt;
    if(cnt>=MAX_CNT)
        cnt=0;
    return cnt;
}

void QBootstrap::buildBootInfo()
{
    int maxCntIndex=-1;
    int minCntIndex=-1;
    bootInfoList.clear();
    for(int i=0; i<bootPathList.count(); ++i)
    {
        QBootInfo bootInfo;
        if(true==getBootInfo(bootPathList.at(i), bootInfo))
        {
            if(true==bootInfo.correct)
            {
                if(0<=maxCntIndex)
                {
                    //if(bootInfo.cnt > bootInfoList.at(maxCntIndex).cnt)
                    if(0 < cmpCnt(bootInfo.cnt, bootInfoList.at(maxCntIndex).cnt))
                        maxCntIndex=i;
                }
                else
                    maxCntIndex=i;

                if(0<=minCntIndex)
                {
                    //if(bootInfo.cnt < bootInfoList.at(minCntIndex).cnt)
                    if(0 > cmpCnt(bootInfo.cnt, bootInfoList.at(minCntIndex).cnt))
                        minCntIndex=i;
                }
                else
                    minCntIndex=i;
            }
            else
            {
                targetIndex=i;
            }
        }
        else
        {
            targetIndex=i;
        }
        bootInfoList.append(bootInfo);
    }
    bootIndex=maxCntIndex;
    if(0>bootIndex) currentCounter=0;
    else            currentCounter=bootInfoList.at(bootIndex).cnt;
    if(0>targetIndex)
        targetIndex=minCntIndex;
    if(0>targetIndex)
        targetIndex=0;
}

bool QBootstrap::buildAllBootDirectory(const QStringList &pathList) const
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

bool QBootstrap::create(const QStringList &bootPathList, const QString &workDirectoryPath)
{
    destroy();
    if(false==buildAllBootDirectory(bootPathList))
        return false;
    this->workDirectoryPath=workDirectoryPath;
    QDir workDirectory(workDirectoryPath);
    if(false==workDirectory.exists())
        return false;
    this->bootPathList.append(bootPathList);
    buildBootInfo();
    return true;
}

void QBootstrap::destroy()
{
    bootPathList.clear();
    bootInfoList.clear();
    currentCounter=0;
    targetIndex=-1;
    bootIndex=-1;
}

bool QBootstrap::bootstrap() const
{
    if(0>bootIndex)
        return false;

    QProcess bootProgram;
    QString programFullName=bootInfoList.at(bootIndex).bootDirectoryPath + "/" +
                            bootInfoList.at(bootIndex).startupFileName;
    return bootProgram.startDetached(programFullName, QStringList(), workDirectoryPath);
}

bool QBootstrap::programSoft(const QString &soft, QString &errorString)
{
    QString processString;
    errorString=TU("");

    processString=TU("œÓ‰„ÓÚÓ‚Í‡ ...");
    emit programProgressSignal(0, processString);

    if(0 > targetIndex)
        targetIndex=0;

    QString bootPath=bootPathList.at(targetIndex);

    // rm -rf bootPath
    QDir bootDir(bootPath);
    bootDir.removeRecursively();

    // mkdir bootPath
    if(false==bootDir.mkpath(bootPath))
    {
        errorString=TU("Õ≈ ÃŒ√” «¿œ»—¿“‹ œ–Œÿ»¬ ”!");
        return false;
    }

    emit programProgressSignal(50, TU("–‡ÒÔ‡ÍÓ‚Í‡ ÔÓ¯Ë‚ÍË ..."));
    // tar -xf soft -C bootInfo.bootDirectoryPath
    QProcess process;
    QStringList arguments;
    QProcess::ExitStatus exitStatus;
    arguments << "-xf" << soft << "-C" << bootPath;
    process.start("tar", arguments);
    if(false==process.waitForFinished())
    {
        errorString=TU("Õ≈»—œ–¿¬Õ¿ﬂ œ–Œÿ»¬ ¿!");
        return false;
    }
    exitStatus=process.exitStatus();
    if(QProcess::NormalExit!=exitStatus)
    {
        errorString=TU("Õ≈»—œ–¿¬Õ¿ﬂ œ–Œÿ»¬ ¿!");
        return false;
    }
    int exitCode=process.exitCode();
    if(0!=exitCode)
    {
        errorString=TU("Õ≈»—œ–¿¬Õ¿ﬂ œ–Œÿ»¬ ¿!");
        return false;
    }

    QFile counterFile(bootPath + "/cnt");
    if(false==counterFile.open(QIODevice::WriteOnly))
    {
        errorString=TU("Õ≈ ÃŒ√” «¿œ»—¿“‹ œ–Œÿ»¬ ”!");
        return false;
    }
    QTextStream stream(&counterFile);
    stream << QString::number(incCnt(currentCounter));
    counterFile.close();


    emit programProgressSignal(80, TU("œÓ‚ÂÍ‡ ..."));
    buildBootInfo();
    if(0>bootIndex)
    {
        errorString=TU("Õ≈ «¿œ»—¿À¿—‹ œ–Œÿ»¬ ¿!");
        return false;
    }
    if(bootPath!=bootInfoList.at(bootIndex).bootDirectoryPath)
    {
        errorString=TU("Õ≈ «¿œ»—¿À¿—‹ œ–Œÿ»¬ ¿!");
        return false;
    }

    //chmod +x startupFileName
    QBashCmd::set_file_execute_mode(bootInfoList.at(bootIndex).bootDirectoryPath + "/" +
                                    bootInfoList.at(bootIndex).startupFileName);
    // sync; drop_caches
    QBashCmd::flush_storage();

    emit programProgressSignal(90, TU("«¿œ–Œ√–¿ÃÃ»–Œ¬¿ÕŒ ”—œ≈ÿÕŒ!"));

    return true;
}

