#ifndef BOOTFORM_H
#define BOOTFORM_H

#include <QWidget>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>

#include "structs_main.h"

#include <string.h>

#define MAX_LEN_STR_HEX      50 //длина данных записи в байтах
#define MAX_LEN_START_HEX    1  //размер начала записи
#define MAX_LEN_SIZE_STR_HEX 1  //размер длины данных
#define MAX_LEN_ADR_HEX      2  //размер длины адреса
#define MAX_LEN_TYPE_HEX     1  //размер типа поля записи
#define MAX_LEN_CC_HEX       1  //размер чек суммы hex

#define MAX_PACKET_SIZE     (MAX_LEN_START_HEX+MAX_LEN_SIZE_STR_HEX+MAX_LEN_ADR_HEX+MAX_LEN_TYPE_HEX+MAX_LEN_STR_HEX+MAX_LEN_CC_HEX)

namespace Ui {
class bootForm;
}

class bootForm : public QWidget
{
    Q_OBJECT

#pragma pack(push, 1)
    typedef struct struct_hex_str
    {
      uint8_t  countByte;
      uint16_t adrFlash;
      uint8_t  typeStr;
      uint8_t  str[MAX_LEN_STR_HEX];
      uint8_t  crcStr;
    }struct_hex_str;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct struct_tableHEX
{
       uint16_t  RegComand;             // регистр команд   0x00
       uint16_t  RegStatus;             // регистр статуса  0x01
       uint16_t  RegErors;              // регистр ошибок   0x02
       uint32_t  RegTransfer1;          // регистр трансферный 0x03
       uint32_t  RegTransfer2;          // регистр трансферный 0x05
       uint8_t   BUF[MAX_PACKET_SIZE];  // массив под зашифрованную строку hex файла 0x07
}struct_tableHEX; //70 байт
#pragma pack(pop)

#pragma pack(push, 1)
typedef union union_tableHEX {
              struct_tableHEX Regs;
              uint16_t             Adr[sizeof(struct_tableHEX)/2];
}union_tableHEX;
#pragma pack(pop)

typedef union_tableHEX* TableHEX;

public:
    explicit bootForm(QWidget *parent = nullptr);
    ~bootForm();
public:
    void setTable(struct_listSavedDevices table);
    void setModbus(QModbusClient *modbusDev);
    void ButtonOpenFile();
    bool Ascii_To_Hex(struct_hex_str* str_hex, QString str);
private:
    Ui::bootForm *ui;
    QStringList hexFileString;
    QVector<struct_hex_str> hexFileStruct;
    struct_listSavedDevices Bootdevice;
    QModbusClient *BootmodbusDevice = nullptr;
};

#endif // BOOTFORM_H
