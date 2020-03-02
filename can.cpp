#include "can.h"
#include "private_can.h"
#include "task.hpp"

#include <QThread>

#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>


#define CAN_KSU_ADDR	((uint8_t)1)
#define CAN_KI_ADDR		((uint8_t)5)
#define CAN_KPT_ADDR	((uint8_t)3)

// 0001 1111   1110 0000   1111 1111   0000 0000
#define CAN_ADDR_MASK	((uint32_t)0x1FE0FF00)
// 0000 0000   1010 0000   0000 0001   0000 0000  - От КИ в КСУ
#define CAN_KI_ID		((uint32_t)0x00A00100)
// 0001 1111   1110 0000   1111 1111   0000 0000
// 0000 0000   0110 0000   0000 0001   0000 0000  - От КТМ в КСУ
#define CAN_KTM_ID		((uint32_t)0x00600100)

#define CAN_ERR_ALL \
       (CAN_ERR_TX_TIMEOUT| \
        CAN_ERR_LOSTARB   | \
        CAN_ERR_CRTL      | \
        CAN_ERR_PROT      | \
        CAN_ERR_TRX       | \
        CAN_ERR_ACK       | \
        CAN_ERR_BUSOFF    | \
        CAN_ERR_BUSERROR  | \
        CAN_ERR_RESTARTED )


//-------------------------------------
// Инициализация драйвера CAN
//-------------------------------------
static CanDrvController *Ctrl=nullptr;
bool can_open(int &err)
{
    CanDrv *worker=new CanDrv;
    if(nullptr==worker){ err=TASK_MEMORY_ERR; return false; }
    Ctrl=new CanDrvController(nullptr);
    if(nullptr==Ctrl){ err=TASK_MEMORY_ERR; return false; }
    if(false==Ctrl->Create(worker, err)){ return false; }
    return Ctrl->WorkerStart(QThread::HighPriority, nullptr, err);
}
void can_close(void)
{
    if(nullptr!=Ctrl)
    {
        delete Ctrl;
        Ctrl=nullptr;
    }
}

bool CanDrv::Init(void *pdata, int &err)
{
    Data=(CanDrvData *)pdata;
    if(
       (nullptr==Data)||
       (-1==Data->s)||
       (nullptr==Data->isq_post)
      )
    {
        err=1;
        return false;
    }
    err=0;
    return true;
}

void CanDrv::Work(void)
{
    ssize_t recv_qnty;
    struct can_frame frame;

    while(true)
    {
        if(true==checkExit())
        {
            break;
        }
        recv_qnty=read(Data->s, &frame, sizeof(frame));
        if(-1==recv_qnty)
        {//timeout
            continue;
        }
        else if(sizeof(frame)==recv_qnty)
        {
            if(false==Data->isq_post->Post(frame))
            {//error - queue full
                continue;
            }
        }
        else
        {//error
            continue;
        }
    }
}

bool CanDrvController::Create(CanDrv *worker, int &err)
{
    struct timeval tv;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_filter filter[2];
    can_err_mask_t err_mask;
    const char *ifname = "can0";

    if(false==Controller::Create(worker, err))
        return false;

    //Создаём сокет
    if((DrvData.s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        qDebug("can_open. Can't open socket. errno: %i", errno);
        return false;
    }

    //Сброс фильтров
    if(-1==setsockopt(DrvData.s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0))
    {
        qDebug("can_open. Can't setsockopt socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }

    //Установка фильтров
    filter[0].can_id=CAN_KI_ID;
    filter[0].can_mask=CAN_ADDR_MASK;
    filter[1].can_id=CAN_KTM_ID;
    filter[1].can_mask=CAN_ADDR_MASK;
    if(-1==setsockopt(DrvData.s, SOL_CAN_RAW, CAN_RAW_FILTER, filter, sizeof(filter)))
    {
        qDebug("can_open. Can't setsockopt socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }
    //Установка фильтра ошибок
    err_mask=CAN_ERR_ALL;
    if(-1==setsockopt(DrvData.s, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask)))
    {
        qDebug("can_open. Can't setsockopt socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }

    //Отключаем loopback
    int loopback=0;// 0 = disabled, 1 = enabled (default)
    setsockopt(DrvData.s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));

    //Настройка таймаута на чтение и запись
    tv.tv_sec = 0;
    tv.tv_usec = 100000;//10mS
    if(-1==setsockopt(DrvData.s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
    {
        qDebug("can_open. Can't setsockopt socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }
    if(-1==setsockopt(DrvData.s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)))
    {
        qDebug("can_open. Can't setsockopt socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }

    //Запрос на получение индекса интерфейса
    strcpy(ifr.ifr_name, ifname);
    if(-1==ioctl(DrvData.s, SIOCGIFINDEX, &ifr))
    {
        qDebug("can_open. Can't ioctl socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }

    //Привязка сокета к интерфейсу
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if(0>bind(DrvData.s, (const struct sockaddr *)&addr, sizeof(addr)))
    {
        qDebug("can_open. Can't bind socket. errno: %i, %s", errno, strerror(errno));
        close(DrvData.s);
        DrvData.s=-1;
        return false;
    }

    return true;
}
bool CanDrvController::WorkerStart(QThread::Priority prio, void *data, int &err)
{
    Q_UNUSED(data);
    return Controller::WorkerStart(prio, &DrvData, err);
}

/*
|31 29|28    21|20 16|15      8|7 |6    0|
+-----+--------+-----+---------+--+------+
|     |        |     |         |  |      |
|     |SRC_ADDR|     |DEST_ADDR|EF| FNUM |
|     |        |     |         |  |      |
+-----+--------+-----+---------+--+------+
Где,
SRC_ADDR - адрес КСУ==1
DEST_ADDR - адрес КИ==5
EF (end frame) - признак последнего кадра (1)
FNUM - номер кадра
*/
static uint32_t _pack_id(uint8_t src_addr, uint8_t dest_addr, uint8_t fnum, bool end_frame)
{
    uint32_t id=0;

    id =CAN_EFF_FLAG;//Расширенный формат кадра
    id+=(uint32_t) src_addr<<21;//Адрес источника
    id+=(uint32_t)dest_addr<<8 ;//Адрес назначения
    id+=0x7 & fnum;//Номер can-кадра
    if(end_frame) id|=0x80;//Последний can-кадр

    return id;
}

static void _unpack_id(uint32_t id, uint8_t &src_addr, uint8_t &dest_addr, uint8_t &fnum, bool &end_frame)
{
     src_addr=0xff & (id>>21);//Адрес источника
    dest_addr=0xff & (id>>8 );//Адрес назначения
    fnum=0x7 & id;//Номер can-кадра
    if(0x80 & id) end_frame=true ;//Последний can-кадр
    else          end_frame=false;
}

bool CanDrvController::Send(uint8_t dest_addr, const BUF_CAN *p_tx)
{
    struct can_frame frame;//can-кадр
    int frame_qnty;//Количество can-кадров с полным количеством байт - 8
    ssize_t write_qnty;//Количество отправленных байт
    int rest;//Количество байт в последем can-кадре

    //Передаём логический кадр (больше 8 байт)
    frame_qnty=p_tx->len/CAN_MAX_DLC;//Количество can-кадров с полным количеством байт - 8
    rest=p_tx->len%CAN_MAX_DLC;//Последний can-кадр имеет это количество байт
    if(0!=rest) ++frame_qnty;//С учётом остатка
    int pos=0;//Позиция передаваемых данных в буфере p_tx
    int fnum;//Номер can-кадра
    for(fnum=0; fnum<frame_qnty; ++fnum, pos+=CAN_MAX_DLC)
    {//Передаём кановский кадр (до 8 байт)
        //Подготавливаем can-кадр
            //Подготавливаем ID
        if(fnum==frame_qnty-1)
        {//Последний кадр
            frame.can_id=_pack_id(CAN_KSU_ADDR, dest_addr, fnum, true );
            //Определяем длину
            if(0==rest) frame.can_dlc=CAN_MAX_DLC;//Все кадры полные, по CAN_MAX_DLC байт.
            else        frame.can_dlc=rest;//Остаток.
        }
        else
        {
            frame.can_id=_pack_id(CAN_KSU_ADDR, dest_addr, fnum, false);
            frame.can_dlc=CAN_MAX_DLC;//Все кадры полные
        }

        memcpy(frame.data, &p_tx->data[pos], frame.can_dlc);
        //Отправляем can-кадр
        write_qnty=send(DrvData.s, &frame, sizeof(frame), 0);
        if(-1==write_qnty)
        {
            qDebug("_frame_send. Can't send to socket: Timeout");
            return false;
        }
        if(sizeof(frame)!=write_qnty)
        {
            qDebug("_frame_send. Can't send to socket: incorrect write_qnty");
            return false;
        }
    }
    return true;
}

bool CanDrvController::Recv(uint8_t from_addr, BUF_CAN *p_rx, int timeout)
{
    struct can_frame frame;
    uint8_t src_addr, dest_addr, fnum;
    bool end_frame;
    bool received;

    p_rx->len=0;
    received=false;
    while(true)
    {
        if(false==Ctrl->Frames.Pend(frame, timeout))
        {//Ошибка
            qDebug("_frame_recv. Can't read from socket: Timeout");
            break;
        }
        _unpack_id(frame.can_id, src_addr, dest_addr, fnum, end_frame);
        if(
           (0== src_addr)&&
           (0==dest_addr)&&
           (0!=(frame.can_id & CAN_ERR_ALL))
          )
        {//Кадр ошибок от драйвера
            uint32_t err=frame.can_id & CAN_ERR_ALL;
            end_frame=0;
                 if(CAN_ERR_TX_TIMEOUT & err){ qDebug("CAN_ERR_TX_TIMEOUT"); break; }
            else if(CAN_ERR_LOSTARB    & err){ qDebug("CAN_ERR_LOSTARB"); break; }
            else if(CAN_ERR_CRTL       & err){ qDebug("CAN_ERR_CRTL"); break; }
            else if(CAN_ERR_PROT       & err){ qDebug("CAN_ERR_PROT"); break; }
            else if(CAN_ERR_TRX        & err){ qDebug("CAN_ERR_TRX"); break; }
            else if(CAN_ERR_ACK        & err){ qDebug("CAN_ERR_ACK"); break; }
            else if(CAN_ERR_BUSOFF     & err){ qDebug("CAN_ERR_BUSOFF"); break; }
            else if(CAN_ERR_BUSERROR   & err){ qDebug("CAN_ERR_BUSERROR"); break; }
            else if(CAN_ERR_RESTARTED  & err){ qDebug("CAN_ERR_RESTARTED"); break; }

        }
        else if(from_addr!=src_addr)
        {
            qDebug("_frame_recv. Bad source addres.");
            break;
        }
        else if((p_rx->len+frame.can_dlc)>sizeof(p_rx->data))
        {
            qDebug("_frame_recv. Can't receive message - override.");
            break;
        }
        else
        {
            memcpy(p_rx->data+p_rx->len, frame.data, frame.can_dlc);
            p_rx->len+=frame.can_dlc;
        }

        if(end_frame)
        {
            received=true;
            break;
        }
    }

    return received;
}

void CanDrvController::Clear(void)
{
    struct can_frame frame;
    while(true==Frames.Pend(frame, 1));
}

//--------------------------------------------------
// Запрос транзакции по CAN
//--------------------------------------------------
uint16_t ReqComMeter(uint8_t dest_addr, const BUF_CAN *p_tx, BUF_CAN *p_rx, int timeout)
{
    if(0==p_tx)
    {
        qDebug("ReqComMeter. Error - null \"p_tx\" buffer.");
        return PRIM_NULL_PTR;
    }

    if(0==p_rx)
    {
        qDebug("ReqComMeter. Error - null \"p_rx\" buffer.");
        return PRIM_NULL_PTR;
    }
    if(false==Ctrl->Send(dest_addr, p_tx))
    {
        Ctrl->Clear();
        return PRIM_SEND_ERR;
    }
    if(false==Ctrl->Recv(dest_addr, p_rx, timeout))
    {
        Ctrl->Clear();
        return PRIM_RECV_ERR;
    }

    return PRIM_NO_ERR;
}



