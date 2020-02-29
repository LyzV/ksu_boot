#ifndef QINTELHEXPARCER_H
#define QINTELHEXPARCER_H

#include <QtCore>
#include <QString>
#include <QByteArray>
#include <QVector>

//This is parser of Intel MSC-86 Object Format. This format generate hex2000.exe utility with option --intel

//.\hex2000.exe --w olength 8 -o .\Output\Release.dat .\Output\Release\Release.out
//.\hex2000.exe --intel --swapbytes --romwidth=16 --outfile=.\Output\Release\Release.hex .\Output\Release\Release.out

//--w
//--intel           Select Intel.               ����� Intel �������: IntelMSC Object Format
//--swapbytes
//--romwidth=value  Specify the ROM device width (default depends on format used). ��������� ������ ���� ������ ROM-����������.
//--outfile -o      Specify an output filename. ��������� ��� ��������� �����.

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
        UnknownError
    };
    struct FileRecord
    {
        quint8 byteCount;
        quint16 address;
        quint8 recordType;
        QVector<quint16> data;
        quint8 checksum;
    };
    struct ParseResult
    {
        ParseErrorCode parseErrorCode;
        int errorLine;
        QVector<FileRecord> records;
    };

private:
    static bool parseLine(const QString &line, FileRecord &record, ParseErrorCode &errorCode);
public:
    static bool parse(const QString &fileName, ParseResult &parseResult);

};

#endif // QINTELHEXPARCER_H
