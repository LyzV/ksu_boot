#include "cpio.h"
#include <QtDebug>
#include <QFile>
#include <QDir>

static const char gpio_dir_name[]="/sys/class/gpio/";
static const char export_file_name[]="/sys/class/gpio/export";
static const char unexport_file_name[]="/sys/class/gpio/unexport";

static bool export_pin(int num)
{
    QString str;
    //QByteArray array;
    QFile export_file(export_file_name);

    //Открываю файл export
    if(!export_file.open(QFile::WriteOnly|QFile::Text))
    {
        str="export_pin. Error: Can't open file \""; str+=export_file_name; str+="\"";
        qDebug(str.toStdString().c_str());
        return false;
    }
    //Экспортирую
    //str=QString::number(num);
    //array.append(str);
    //export_file.write(array);
    QTextStream export_file_stream(&export_file);
    str=QString::number(num);
    export_file_stream << str;
    //Закрываю файл export
    export_file.close();

    str=gpio_dir_name; str+="gpio"; str+=QString::number(num);
    QDir gpio_num_dir(str);
    if(!gpio_num_dir.exists())
    {
        str="COPin::_Export. Error: Can't export gpio"; str+=QString::number(num);
        qDebug(str.toStdString().c_str());
        return false;
    }
    return true;
}

static void unexport_pin(int num)
{
    QString str;
    //Открываю файл unexport
    QFile unexport_file(unexport_file_name);
    if(!unexport_file.open(QFile::WriteOnly|QFile::Text))
    {
        str="unexport_pin. Error: Can't open file \""; str+=unexport_file_name; str+="\"";
        qDebug(str.toStdString().c_str());
        return;
    }
    //Делаю unexport
    QTextStream unexport_file_stream(&unexport_file);
    str=QString::number(num);
    unexport_file_stream<<str;
    //Закрываю файл unexport
    unexport_file.close();
}

COPin::COPin(GPIONUM gnum, BITNUM bnum, CPIO_VALUE init_value)
{
    gnum=gnum;
    bnum=bnum;
    num=(int)gnum*32+(int)bnum;
    ivalue=init_value;
    //Строим имя файла /sys/class/gpio/gpioNN/value
    value_file_name=gpio_dir_name;
    value_file_name+="gpio";
    value_file_name+=QString::number(num);
    value_file_name+="/value";
    exported=false;
}
CIPin::CIPin(GPIONUM gnum, BITNUM bnum)
{
    gnum=gnum;
    bnum=bnum;
    num=(int)gnum*32+(int)bnum;
    //Строим имя файла /sys/class/gpio/gpioNN/value
    value_file_name=gpio_dir_name;
    value_file_name+="gpio";
    value_file_name+=QString::number(num);
    value_file_name+="/value";
    exported=false;
}
COPin::~COPin(void)
{
    Unexport();
}
CIPin::~CIPin(void)
{
    Unexport();
}

bool COPin::Export(void)
{
    exported=export_pin(num);
    if(false==exported)
        return false;

    //Открываю файл direction
    QString str;
    str=gpio_dir_name; str+="gpio"; str+=QString::number(num)+'/';
    QFile direction_file(str+"direction");
    if(!direction_file.open(QFile::WriteOnly|QFile::Text))
    {
        str="COPin::Export. Error: Can't open file \""; str+=direction_file.fileName(); str+="\".";
        qDebug(str.toStdString().c_str());
        return false;
    }
    //Задаём направление
    QTextStream direction_file_stream(&direction_file);
    direction_file_stream.setCodec("Windows-1251");
    direction_file_stream << "out";
    //Закрываю файл direction
    direction_file.close();
    //Задаю начальное значение
    Set(ivalue);

    return true;
}
bool CIPin::Export(void)
{
    exported=export_pin(num);
    if(false==exported)
        return false;

    //Открываю файл direction
    QString str;
    str=gpio_dir_name; str+="gpio"; str+=QString::number(num)+'/';
    QFile direction_file(str+"direction");
    if(!direction_file.open(QFile::WriteOnly|QFile::Text))
    {
        str="CIPin::Export. Error: Can't open file \""; str+=direction_file.fileName(); str+="\".";
        qDebug(str.toStdString().c_str());
        return false;
    }
    //Задаём направление
    QTextStream direction_file_stream(&direction_file);
    direction_file_stream.setCodec("Windows-1251");
    direction_file_stream << "in";
    //Закрываю файл direction
    direction_file.close();

    return true;
}

void COPin::Unexport(void)
{
    if(true==exported)
    {
        unexport_pin(num);
        exported=false;
    }
}

void CIPin::Unexport(void)
{
    if(true==exported)
    {
        unexport_pin(num);
        exported=false;
    }
}

bool COPin::Set(CPIO_VALUE value)
{
    QString str;

    if(false==exported)
    {
        str="COPin::Set. Pin did not exported.";
        qDebug(str.toStdString().c_str());
        return false;
    }

    //Открываем файл value
    QFile value_file(value_file_name);
    if(false==value_file.open(QFile::WriteOnly|QFile::Text))
    {
        str="COPin::Set. Error: Can't open file \""; str+=value_file_name; str+="\".";
        qDebug(str.toStdString().c_str());
        return false;
    }
    //Задаём значение
    QTextStream value_file_stream(&value_file);
    value_file_stream.setCodec("Windows-1251");
    sts=value;
    if(CPIO_ON==sts) value_file_stream << '1';
    else             value_file_stream << '0';
    //Закрываем файл value
    value_file.close();

    return true;
}
CPIO_VALUE COPin::Get(void)
{
    return sts;
}

bool CIPin::Get(CPIO_VALUE &value)
{
    QString str;
    //Открываем файл value
    QFile value_file(value_file_name);
    if(false==value_file.open(QFile::ReadOnly|QFile::Text))
    {
        QString str;
        str="CPio::Set. Error: Can't open file \""; str+=value_file_name; str+="\".";
        qDebug(str.toStdString().c_str());
        return false;
    }
    //Получаем значение
    QTextStream value_file_stream(&value_file);
    value_file_stream.setCodec("Windows-1251");
    value_file_stream >> str;
    //Закрываем файл value
    value_file.close();
    //Возвращаем знчение
    if(0==QString::compare("1", str)) value=CPIO_ON;
    else                              value=CPIO_OFF;
    return true;
}

