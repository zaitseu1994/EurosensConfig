#ifndef STRUCTS_UI_H
#define STRUCTS_UI_H

#include <stdint.h>
#include <QString>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QSerialPortInfo>
#include <QSerialPort>


typedef struct struct_ComModbus
{
    QModbusClient *modbusDev;
    bool           isFind;
    QString        nameCom;
    int            currentAdr;
    QString        description;
    QString        manufacturer;
    quint16        productIdentifier;
    quint16        vendorIdentifier;
} struct_ComModbus;



#endif // STRUCTS_UI_H
