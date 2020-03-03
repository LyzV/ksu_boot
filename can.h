#ifndef CAN_DRIVER
#define CAN_DRIVER

#include "vtypes.h"
#include <QObject>

#define PRIM_NO_ERR			0 //все в норме
#define PRIM_NULL_PTR       131 //нулевой указатель
#define PRIM_SEND_ERR       133 //ошибка при отправке кадра
#define PRIM_RECV_ERR       134 //ошибка при приёме кадра


#define LEN_CANBUF	256//Длина буфера в байтах
// структура буфера CAN
typedef struct
{
    uint16_t len;
    uint8_t  data[LEN_CANBUF];
} BUF_CAN;

extern bool can_open(int &err);
extern void can_close(void);
extern uint16_t ReqComMeterOne(uint8_t dest_addr, const BUF_CAN *p_tx, BUF_CAN *p_rx, int timeout=10);
extern uint16_t ReqComMeter   (uint8_t dest_addr, const BUF_CAN *p_tx, BUF_CAN *p_rx, int timeout=10);
extern uint16_t ReqOnly       (uint8_t dest_addr, const BUF_CAN *p_tx);

#endif//CAN_DRIVER
