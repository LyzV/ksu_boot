#include "kiprotocol.h"

#include "can.h"
#include "qintelhexparcer.h"
#include "packing.h"

#define KI_DEVICE_ADDRESS   5

#define CMD_CONNECT		0x1234
#define CMD_DISCONNECT	0x4321
#define CMD_ERASE		0x4444
#define CMD_ERASEPOLL	0x4441
#define CMD_PROGRAM		0x1144
#define CMD_READTYPE	0x6789
#define CMD_RESET		0xF000

#define STATUS_OK		0x0000

#define ERASE_TIMEOUT   30//30 seconds

#define PROGRAM_LENGTH	0x10


void KiProtocol::copy(const QByteArray &array, uint8_t start, uint8_t qnty, uint8_t *buf) const
{
    for(uint8_t i=0; i<qnty; ++i)
    {
        buf[i]=array.at(i+start);
    }
}

KiProtocol::~KiProtocol()
{
    can_close();
}

bool KiProtocol::create(uint8_t addressWidth, int &err)
{
    this->addressWidth=addressWidth;
    return can_open(err);
}

bool KiProtocol::connect()
{
    uint16_t cmd=CMD_RESET;
    uint8_t  qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    txBuffer.len=qnty;
    ReqOnly(KI_DEVICE_ADDRESS, &txBuffer);

    QThread::msleep(1000);

    cmd=CMD_CONNECT;
    qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    txBuffer.len=qnty;
    if(PRIM_NO_ERR!=ReqComMeter(KI_DEVICE_ADDRESS, &txBuffer, &rxBuffer))
        return false;
    uint16_t status;
    if((sizeof(cmd)+sizeof(status))>rxBuffer.len)
        return false;
    qnty=0;
    qnty+=CPacking::Unpack(cmd, rxBuffer.data+qnty);
    if(CMD_CONNECT!=cmd)
        return false;
    qnty+=CPacking::Unpack(status, rxBuffer.data+qnty);
    if(STATUS_OK!=status)
        return false;

    return true;
}

void KiProtocol::disconnect()
{
    uint16_t cmd=CMD_DISCONNECT;
    uint8_t  qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    txBuffer.len=qnty;
    ReqComMeter(KI_DEVICE_ADDRESS, &txBuffer, &rxBuffer);
    QThread::msleep(1000);
}

bool KiProtocol::hver(uint16_t &ver, uint16_t &type)
{
    uint16_t cmd=CMD_READTYPE;
    uint8_t  qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    txBuffer.len=qnty;
    if(PRIM_NO_ERR!=ReqComMeter(KI_DEVICE_ADDRESS, &txBuffer, &rxBuffer))
        return false;
    uint16_t status;
    if((sizeof(cmd)+sizeof(type)+sizeof(ver)+sizeof(status))>rxBuffer.len)
        return false;
    qnty=0;
    qnty+=CPacking::Unpack(cmd, rxBuffer.data+qnty); if(CMD_READTYPE!=cmd) return false;
    qnty+=CPacking::Unpack(type, rxBuffer.data+qnty);
    qnty+=CPacking::Unpack(ver, rxBuffer.data+qnty);
    qnty+=CPacking::Unpack(status, rxBuffer.data+qnty); if(STATUS_OK!=status) return false;

    return true;
}

bool KiProtocol::erase(uint32_t address, void (*callback)(void))
{
    uint16_t cmd=CMD_ERASE;
    uint8_t  qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    qnty+=CPacking::Pack(address, txBuffer.data+qnty);
    txBuffer.len=qnty;
    ReqOnly(KI_DEVICE_ADDRESS, &txBuffer);

    cmd=CMD_ERASEPOLL;
    qnty=0;
    qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
    txBuffer.len=qnty;
    int cnt;//seconds
    for(cnt=0; cnt<ERASE_TIMEOUT; ++cnt)
    {
        if(PRIM_NO_ERR==ReqComMeterOne(KI_DEVICE_ADDRESS, &txBuffer, &rxBuffer))
            break;
        if(nullptr!=callback)
            callback();
        QThread::msleep(1000);
    }
    if(ERASE_TIMEOUT<=cnt)
        return false;

    uint16_t status;
    if((sizeof(cmd)+sizeof(status))>rxBuffer.len)
        return false;
    qnty=0;
    qnty+=CPacking::Unpack(cmd, rxBuffer.data+qnty); if(CMD_ERASEPOLL!=cmd) return false;
    qnty+=CPacking::Unpack(status, rxBuffer.data+qnty); if(STATUS_OK!=status) return false;

    return true;
}

bool KiProtocol::program(uint32_t address, const QByteArray &chunk)
{
    uint16_t cmd=CMD_PROGRAM;
    uint16_t status;
    uint8_t  qnty;
    uint16_t length;

    for(uint8_t offset=0; offset<chunk.count(); offset+=length)
    {
        length=chunk.count()-offset;
        if(PROGRAM_LENGTH<length)
            length=PROGRAM_LENGTH;

        qnty=0;
        qnty+=CPacking::Pack(cmd, txBuffer.data+qnty);
        qnty+=CPacking::Pack(length, txBuffer.data+qnty);
        qnty+=CPacking::Pack(address, txBuffer.data+qnty);
        for(uint8_t i=0; i<length; ++i)
            txBuffer.data[qnty+i]=chunk.at(offset+i);
        qnty+=length;
        address+=length/addressWidth;

        txBuffer.len=qnty;
        if(PRIM_NO_ERR!=ReqComMeter(KI_DEVICE_ADDRESS, &txBuffer, &rxBuffer))
            return false;
        if((sizeof(cmd)+sizeof(status))>rxBuffer.len)
            return false;
        qnty=0;
        qnty+=CPacking::Unpack(cmd, rxBuffer.data+qnty);
        if(CMD_PROGRAM!=cmd)
            return false;
        qnty+=CPacking::Unpack(status, rxBuffer.data+qnty);
        if(STATUS_OK!=status)
            return false;
    }

    return true;
}
