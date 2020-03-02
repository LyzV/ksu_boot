#include "qintelhexparcer.h"

#include <QStringRef>

//Record Field Positions
#define START_CHARACTER_POSITION        0
#define START_CHARACTER_SIZE            1
#define BYTE_COUNT_POSITION             (START_CHARACTER_POSITION+START_CHARACTER_SIZE)
#define BYTE_COUNT_SIZE                 2
#define ADDRESS_POSITION                (BYTE_COUNT_POSITION+BYTE_COUNT_SIZE)
#define ADDRESS_SIZE                    4
#define RECORD_TYPE_POSITION            (ADDRESS_POSITION+ADDRESS_SIZE)
#define RECORD_TYPE_SIZE                2
#define DATA_POSITION                   (RECORD_TYPE_POSITION+RECORD_TYPE_SIZE)
#define DATA_SIZE                       4
#define CHECKSUM_POSITION(byteCount)    (DATA_POSITION+byteCount*2)
#define CHECKSUM_SIZE                   2

//Digit Base
#define DIGIT_BASE  16

bool QIntelHexParcer::parseLine(const QString &line, FileRecord &record, ParseErrorCode &errorCode) const
{
    quint8 sum=0;

    errorCode=ParseErrorCode::UnknownError;

    if(':'!=line.at(START_CHARACTER_POSITION))
    {
        errorCode=ParseErrorCode::LineError;
        return false;
    }
    //Extract Byte Count
    QStringRef byteCountRef(&line, BYTE_COUNT_POSITION, BYTE_COUNT_SIZE);
    bool ok;
    record.byteCount=(quint8)byteCountRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::ByteCountError;
        return false;
    }
    sum+=record.byteCount;

    //Extract Address
    QStringRef addressRef(&line, ADDRESS_POSITION, ADDRESS_SIZE);
    record.address=(quint16)addressRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::AddressError;
        return false;
    }
    sum+=(quint8)(record.address>>8);
    sum+=(quint8)(record.address);

    //Extract Record Type
    QStringRef recordTypeRef(&line, RECORD_TYPE_POSITION, RECORD_TYPE_SIZE);
    record.recordType=(quint8)recordTypeRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::TypeError;
        return false;
    }
    sum+=record.recordType;

    //Extract Data
    quint8 wordCount=record.byteCount>>1;
    record.data.clear();
    record.data.resize(wordCount);
    for(quint8 i=0; i<wordCount; ++i)
    {
        qint8 position=DATA_POSITION + i*DATA_SIZE;
        QStringRef wordData(&line, position, DATA_SIZE);
        record.data[i]=(quint16)wordData.toUInt(&ok, DIGIT_BASE);
        if(false==ok)
        {
            errorCode=ParseErrorCode::DataError;
            return false;
        }
        sum+=(quint8)(record.data[i]>>8);
        sum+=(quint8)(record.data[i]);
    }
    //Extract Checksum
    QStringRef checksumRef(&line, CHECKSUM_POSITION(record.byteCount), CHECKSUM_SIZE);
    record.checksum=(quint8)checksumRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::CheckSumError;
        return false;
    }
    sum+=record.checksum;
    if(0!=sum)
    {
        errorCode=ParseErrorCode::CheckSumError;
        return false;
    }

    errorCode=ParseErrorCode::Ok;
    return true;
}

QIntelHexParcer::~QIntelHexParcer()
{
    if(file.isOpen())
        file.close();
}

bool QIntelHexParcer::verifyHexFile(const QString &fileName, QIntelHexParcer::ParseResult &parseResult) const
{
    parseResult.parseErrorCode=ParseErrorCode::UnknownError;
    parseResult.lineCount=0;

    QFile hexFile(fileName);
    if(false==hexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        parseResult.parseErrorCode=ParseErrorCode::OpenFileError;
        return false;
    }
    QTextStream textStream(&hexFile);

    parseResult.lineCount=0;
    for(//through all lines
        QString line=textStream.readLine(256);
        false==line.isEmpty();
        line=textStream.readLine(256)
       )
    {
        FileRecord fileRecord;
        bool ret=parseLine(line, fileRecord, parseResult.parseErrorCode);
        ++parseResult.lineCount;
        if(false==ret)
            return false;
    }

    parseResult.parseErrorCode=ParseErrorCode::Ok;
    return true;
}

bool QIntelHexParcer::open(const QString &fileName)
{
    if(file.isOpen())
        file.close();
    file.setFileName(fileName);
    if(false==file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    stream.setDevice(&file);
    return true;
}

void QIntelHexParcer::close()
{
    if(file.isOpen())
        file.close();
}

bool QIntelHexParcer::firstRecord(QIntelHexParcer::FileRecord &record, QIntelHexParcer::ParseResult &parseResult)
{
    QString line=stream.readLine(256);
    lineCount=1;
    parseResult.lineCount=1;
    return parseLine(line, record, parseResult.parseErrorCode);
}

bool QIntelHexParcer::nextRecord(QIntelHexParcer::FileRecord &record, QIntelHexParcer::ParseResult &parseResult)
{
    QString line=stream.readLine(256);
    if(line.isEmpty())
    {
        parseResult.lineCount=lineCount;
        parseResult.parseErrorCode=ParseErrorCode::FileEnd;
        return false;
    }
    ++lineCount;
    parseResult.lineCount=lineCount;
    return parseLine(line, record, parseResult.parseErrorCode);
}
