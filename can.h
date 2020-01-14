#ifndef CAN_DRIVER
#define CAN_DRIVER

#include "vtypes.h"
#include "CanPrimitives.h"
#include <QObject>


#define LEN_CANBUF	256//Длина буфера в байтах
// структура буфера CAN
typedef struct
{
    uint16_t len;
    uint8_t  data[LEN_CANBUF];
} BUF_CAN;

extern bool can_open(QObject *parent, int &err);
extern uint16_t ReqComMeter(uint8_t dest_addr, const BUF_CAN *p_tx, BUF_CAN *p_rx);

#endif//CAN_DRIVER
