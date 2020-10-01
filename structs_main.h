#ifndef STRUCTS_MAIN_H
#define STRUCTS_MAIN_H

#include <QString>

#pragma pack(push, 1)
typedef struct struct_tableRegsRead
{
   uint32_t  LogError;         // лог ошибок
   uint64_t  timeconnect;      // Дата последней связи            // 8b
   uint64_t  timechange;       // Дата изменения настроек         // 8b
   uint32_t  idchange;         // id пользователя внес изменения  // 4b
   uint64_t  timedefault;      // Дата изменения заводских        // 8b
   uint32_t  iddefault;        // id пользователя внес заводские  // 4b
   uint16_t  TypeDevice;       // тип устройства 		          // 2b
   uint32_t  SerialNum;        // серийный номер устройства       // 4b
   uint32_t  VerApp;           // аппаратная версия устройства    // 4b
   uint16_t  mas[10];          // доп резерв                      // 20b
}struct_tableRegsRead;
#pragma pack(pop)

typedef union union_tableRegsRead {
              struct_tableRegsRead Regs;
              uint16_t             Adr[sizeof(struct_tableRegsRead)/2];
}union_tableRegsRead;

typedef struct struct_listSavedDevices
{
    union_tableRegsRead  device;
    QString              devicename;
    QString              portname;
    QString              modbusadr;
}struct_listSavedDevices;


#endif // STRUCTS_MAIN_H
