#include "qintelhexparcer.h"

#include <QFile>
#include <QTextStream>
#include <QStringRef>

//Record Field Positions
#define START_CHARACTER_POSITION        0
#define START_CHARACTER_SIZE            1
#define BYTE_COUNT_POSITION             (START_CHARACTER_POSITION+START_CHARACTER_SIZE)
#define BYTE_COUNT_SIZE                 2
#define ADDRESS_POSITION                (BYTE_COUNT_POSITION+BYTE_COUNT_SIZE)
#define ADDRESS_COUNT_SIZE              4
#define RECORD_TYPE_POSITION            (BYTE_COUNT_POSITION+BYTE_COUNT_SIZE)
#define RECORD_TYPE_SIZE                2
#define DATA_POSITION                   (RECORD_TYPE_POSITION+RECORD_TYPE_SIZE)
#define DATA_SIZE                       4
#define CHECKSUM_POSITION(byteCount)    (DATA_POSITION+byteCount*2)
#define CHECKSUM_SIZE                   2

//Digit Base
#define DIGIT_BASE  16

bool QIntelHexParcer::parseLine(const QString &line, FileRecord &record, ParseErrorCode &errorCode)
{
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
    //Extract Address
    QStringRef addressRef(&line, ADDRESS_POSITION, ADDRESS_COUNT_SIZE);
    record.address=(quint16)addressRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::AddressError;
        return false;
    }
    //Extract Record Type
    QStringRef recordTypeRef(&line, RECORD_TYPE_POSITION, RECORD_TYPE_SIZE);
    record.recordType=(quint8)recordTypeRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::TypeError;
        return false;
    }
    //Extract Data
    quint8 wordCount=record.byteCount>>1;
    record.data.clear();
    record.data.reserve(wordCount);
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
    }
    //Extract Checksum
    QStringRef checksumRef(&line, CHECKSUM_POSITION(record.byteCount), CHECKSUM_SIZE);
    record.checksum=(quint8)checksumRef.toUInt(&ok, DIGIT_BASE);
    if(false==ok)
    {
        errorCode=ParseErrorCode::CheckSumError;
        return false;
    }

    errorCode=ParseErrorCode::Ok;
    return true;
}

bool QIntelHexParcer::parse(const QString &fileName, ParseResult &parseResult)
{
    parseResult.parseErrorCode=ParseErrorCode::UnknownError;
    parseResult.errorLine=0;

    QFile hexFile(fileName);
    if(false==hexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        parseResult.parseErrorCode=ParseErrorCode::OpenFileError;
        return false;
    }
    QTextStream textStream(&hexFile);

    parseResult.errorLine=1;
    for(//through all lines
        QString line=textStream.readLine(256);
        false==line.isEmpty();
        line=textStream.readLine(256)
       )
    {
        FileRecord fileRecord;
        if(false==parseLine(line, fileRecord, parseResult.parseErrorCode))
            return false;

        ++parseResult.errorLine;
    }

    parseResult.parseErrorCode=ParseErrorCode::Ok;
    parseResult.errorLine=0;
    return true;
}
