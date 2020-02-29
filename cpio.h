#ifndef CPIO_H
#define CPIO_H

#include <QString>

typedef enum
{
    GPIO1=0,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5
} GPIONUM;

typedef enum
{
    BIT0=0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7,
    BIT8, BIT9, BIT10, BIT11, BIT12, BIT13, BIT14, BIT15,
    BIT16, BIT17, BIT18, BIT19, BIT20, BIT21, BIT22, BIT23,
    BIT24, BIT25, BIT26, BIT27, BIT28, BIT29, BIT30, BIT31
} BITNUM;

typedef enum
{
    CPIO_OFF=0,
    CPIO_ON
} CPIO_VALUE;

class COPin
{
    bool exported;
    GPIONUM gnum;
    BITNUM bnum;
    int num;//num=gnum*32+bnum
    CPIO_VALUE ivalue;
    CPIO_VALUE sts;//Последнее записанное значение
    QString value_file_name;

public:
    COPin(GPIONUM gnum, BITNUM bnum, CPIO_VALUE init_value);
    ~COPin(void);
    bool Export(void);
    void Unexport(void);
    bool Set(CPIO_VALUE value);
    CPIO_VALUE Get(void);
};

class CIPin
{
    bool exported;
    GPIONUM gnum;
    BITNUM bnum;
    int num;//num=gnum*32+bnum
    QString value_file_name;

public:
    CIPin(GPIONUM gnum, BITNUM bnum);
    ~CIPin(void);
    bool Export(void);
    void Unexport(void);
    bool Get(CPIO_VALUE &value);
};

#endif // CPIO_H
