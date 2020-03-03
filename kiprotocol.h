#ifndef KIPROTOCOL_H
#define KIPROTOCOL_H

#include "vtypes.h"
#include "can.h"
#include <QByteArray>

class KiProtocol
{
    uint8_t addressWidth;//Шаг адреса. Для КИ - 2 байта.
    BUF_CAN txBuffer, rxBuffer;
    void copy(const QByteArray &array, uint8_t start, uint8_t qnty, uint8_t *buf) const;

public:
    ~KiProtocol();
    bool create(uint8_t addressWidth, int &err);

    bool connect();
    void disconnect();
    bool hver(uint16_t &ver, uint16_t &type);
    bool erase(uint32_t address, void (*callback)(void));
    bool program(uint32_t address, const QByteArray &chunk);
};

#endif // KIPROTOCOL_H
