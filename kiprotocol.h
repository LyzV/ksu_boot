#ifndef KIPROTOCOL_H
#define KIPROTOCOL_H

#include "vtypes.h"
#include "can.h"
#include <QByteArray>

class KiProtocol: public QObject
{
    Q_OBJECT

    uint8_t addressWidth;//Шаг адреса. Для КИ - 2 байта.
    BUF_CAN txBuffer, rxBuffer;
    void copy(const QByteArray &array, uint8_t start, uint8_t qnty, uint8_t *buf) const;

public:
    KiProtocol(QObject *parent=nullptr): QObject(parent){}
    virtual ~KiProtocol();
    bool create(uint8_t addressWidth, int &err);

    bool connect();
    void disconnect();
    bool hver(uint16_t &ver, uint16_t &type);
    bool erase(uint32_t address);
    bool program(uint32_t address, const QByteArray &chunk);

signals:
    void eraseSignal(int seconds);
};

#endif // KIPROTOCOL_H
