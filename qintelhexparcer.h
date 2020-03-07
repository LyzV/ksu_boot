#ifndef QINTELHEXPARCER_H
#define QINTELHEXPARCER_H

#include <QtCore>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QFile>
#include <QTextStream>

//This is parser of Intel MSC-86 Object Format. This format generate hex2000.exe utility with option --intel

//.\hex2000.exe --w olength 8 -o .\Output\Release.dat .\Output\Release\Release.out
//.\hex2000.exe --intel --swapbytes --romwidth=16 --outfile=.\Output\Release\Release.hex .\Output\Release\Release.out

//--w
//--intel           Select Intel.               Выбор Intel формата: IntelMSC Object Format
//--swapbytes
//--romwidth=value  Specify the ROM device width (default depends on format used). Указывает ширину шины данных ROM-устройства.
//--outfile -o      Specify an output filename. Указывает имя выходного файла.

class QIntelHexParcer
{
public:
    enum ParseErrorCode
    {
        Ok=0,
        OpenFileError,
        LineError,
        ByteCountError,
        AddressError,
        TypeError,
        DataError,
        CheckSumError,
        FileEnd,
        UnknownError
    };
    struct FileRecord
    {
        quint8 byteCount;
        quint16 address;
        quint8 recordType;
        QByteArray data;
        quint8 checksum;
    };
    struct ParseResult
    {
        ParseErrorCode parseErrorCode;
        int lineCount;
    };

private:
    QFile file;
    QTextStream stream;
    int lineCount;
    bool parseLine(const QString &line, FileRecord &record, ParseErrorCode &errorCode) const;
public:
    ~QIntelHexParcer();
    bool verifyHexFile(const QString &fileName, ParseResult &parseResult) const;
    bool open(const QString &fileName);
    void close();
    bool firstRecord(FileRecord &record, ParseResult &parseResult);
    bool nextRecord(FileRecord &record, ParseResult &parseResult);
};

#endif // QINTELHEXPARCER_H
