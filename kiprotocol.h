#ifndef KIPROTOCOL_H
#define KIPROTOCOL_H

#include "vtypes.h"
#include "can.h"
#include <QByteArray>

class KiProtocol
{
    BUF_CAN txBuffer, rxBuffer;
    void copy(const QByteArray &array, uint8_t start, uint8_t qnty, uint8_t *buf) const;

public:
    ~KiProtocol();
    bool create(int &err);

    void reset();
    bool connect();
    void disconnect();
    bool hver(uint16_t &ver, uint16_t &type);
    bool erase(void (*callback)(void));
    bool program(uint32_t address, const QByteArray &chunk);
};

#endif // KIPROTOCOL_H
