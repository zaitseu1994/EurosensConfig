#ifndef BOOTFORM_H
#define BOOTFORM_H

#include <QWidget>
#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QFile>
#include <QFileDialog>

#include <QTimer>

#include "structs_main.h"
#include "structs_ui.h"

#include <string.h>

//#define BOOT_FILE_DEBUG

#define PreBootTimer  500

#define CMD_READY_TO_BOOT 0x34
#define ANSW_READY_TO_BOOT 0xFFCB

#define CMD_START_BOOT 0x37
#define ANSW_START_BOOT 0xFFC8

#define CMD_FLASH_STR 0x39
#define ANSW_FLASH_STR 0xFFC6

#define CMD_END_BOOT 0x41
#define ANSW_END_BOOT 0xFFBE


#define MAX_LEN_STR_HEX      50 //длина данных записи в байтах
#define MAX_LEN_START_HEX    1  //размер начала записи
#define MAX_LEN_SIZE_STR_HEX 1  //размер длины данных
#define MAX_LEN_ADR_HEX      2  //размер длины адреса
#define MAX_LEN_TYPE_HEX     1  //размер типа поля записи
#define MAX_LEN_CC_HEX       1  //размер чек суммы hex

#define MAX_PACKET_SIZE     (MAX_LEN_START_HEX+MAX_LEN_SIZE_STR_HEX+MAX_LEN_ADR_HEX+MAX_LEN_TYPE_HEX+MAX_LEN_STR_HEX+MAX_LEN_CC_HEX)
#define COUNT_STR_BLOCK     7

#define START_MAIN_ADR_W    0
#define COUNT_MAIN_REGS_W   1

#define START_MAIN_ADR_R    1
#define COUNT_MAIN_REGS_R   2

#define START_ALL_ADR    0
#define COUNT_ALL_REGS   35

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
      uint8_t adrFlash[2];
      uint8_t  typeStr;
      uint8_t  str[MAX_LEN_STR_HEX];
      uint8_t  crcStr;
    }struct_hex_str;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union union_hex_str
{
       struct_hex_str Regs;
       uint8_t adr[sizeof(struct_hex_str)];
}union_hex_str;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct struct_tableHEX
{
       uint16_t  RegComand;             // регистр команд   0x00
       uint16_t  RegStatus;             // регистр статуса  0x01
       uint16_t  RegErors;              // регистр ошибок   0x02
       uint32_t  RegTransfer1;          // регистр трансферный 0x03
       uint32_t  RegTransfer2;          // регистр трансферный 0x05
       uint8_t   BUF[MAX_PACKET_SIZE*COUNT_STR_BLOCK];  // массив под зашифрованную строку hex файла 0x07
}struct_tableHEX; //70 +112 байт
#pragma pack(pop)

#pragma pack(push, 1)
typedef union union_tableHEX {
              struct_tableHEX Regs;
              uint8_t         adr8[100];
              uint16_t        Adr[sizeof(struct_tableHEX)/2];
}union_tableHEX;
#pragma pack(pop)

typedef union_tableHEX* TableHEX;

typedef enum
{
    ERR_NOT,
    ERR_SIZE,
    ERR_COD,
    ERR_CRCHEX,
    ERR_FLASH
} FLASHERROR;

public:
    explicit bootForm(QWidget *parent = nullptr);
    ~bootForm();
public:
    void setTable(struct_listSavedDevices table);
    void setFindDev(bool flag);
    void setVector(QVector<struct_ComModbus>& vec);

    void enterBootMode();

    void setModbus(QModbusClient *modbusDev);
    void ButtonOpenFile();
    QByteArray readFileFromCode(QString *str,bool *open);

    void ButtonBoot();
    bool Ascii_To_HexStruct(union_hex_str* str_hex, QByteArray buf);

    void bootSteps(int numRepeat);
    void sendRegs(int startAdr, int countregs);
    void replyReceivedWrite();
    void updateRegs(int startAdr, int countregs);
    void replyReceivedRead();

    void updateRegsBoot(int startAdr, int countregs);
    void replyReadBoot();

private:
    Ui::bootForm *ui;
    bool deviceIsfind = false;
    bool bootFileOpen = false;

    QVector<struct_ComModbus> vectorModbusDevice;
    QStringList hexFileString;
    QVector<union_hex_str> hexFileStruct;
    struct_listSavedDevices Bootdevice;
    QModbusClient *BootmodbusDevice = nullptr;
    QTimer *ModbusBootTimer = nullptr;
    QFileInfo *fileInfo;
    union_tableHEX tablH;
    TableHEX LoclTableHex = &tablH;
    uint8_t currentStepBoot = 0;
    uint16_t countNoAnswerProtcl =0;
    uint16_t currentNumStruct =0;
};

#endif // BOOTFORM_H
