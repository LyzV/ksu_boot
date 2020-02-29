#include "gpio.h"
#include "cpio.h"
#include <QtDebug>
#include <QString>
#include <unistd.h>

static COPin gpio4_17(GPIO4, BIT17, CPIO_ON);//gpio_wdt_reset  // TPL5010

//#include <linux/gpio.h>
bool gpio_open(void)
{
    //Аппаратная сторожевая собака TPL5010
    if(!gpio4_17.Export()){ qDebug("gpio_open. Error: Can't export gpio4_17"); goto _failue_exit; }//gpio_wdt_reset

    return true;

_failue_exit:
    gpio_close();
    return false;
}

void gpio_close(void)
{
    //Аппаратная сторожевая собака
    gpio4_17.Unexport();
}

//Сброс аппаратной сторожевой собаки
void gpio_wdt_reset(void)
{
static bool state=0;
    if(state){ gpio4_17.Set(CPIO_OFF); state=false; }
    else     { gpio4_17.Set(CPIO_ON ); state=true ; }
}
