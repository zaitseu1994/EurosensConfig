#ifndef STRUCTS_LIB_H
#define STRUCTS_LIB_H

#include <stdint.h>
#include <QString>

#define COUNT_REGSWRITE       41 //72

#define START_MINIMAL_ADR_R    0
#define COUNT_MINIMAL_REGS_R   10

#define START_MINIMAL_ADR_W    5
#define COUNT_MINIMAL_REGS_W   5

#define START_SENSOR_ADR       10
#define COUNT_SENSOR_REGS      25

#define START_DEV_ADR          35
#define COUNT_DEV_REGS         6

#define START_ADDDEV_ADR       41
#define COUNT_ADDDEV_REGS      31
;
#pragma pack(push, 1)
typedef struct struct_tableRegsWrite
{
    uint16_t  CurrentDistanse;   // Текущее измеряемое расстояние  // 2b <- 0
    uint32_t  CurrentVolume;     // Текущий измеряемый обьем       // 4b
    uint32_t  RegError;          //                                // 4b
    uint16_t  RegTransfer1;      // Регистр приема передачи 1      // 2b <-5
    float     RegTransfer2;      // Регистр приема передачи 2      // 4b
    uint16_t  RegCommand;        // Регистр команд                 // 2b
    uint16_t  RegStatus;         // Статусный регистр              // 2b <- 9

    int       SensorId;          // Id сенсора                     // 4b <- 10
    float     StartOfMeasure;    // -0.7 - 7.0 метр                // 4b
    float     LenghtOfMeasure;   // 0.0 - 7.7 метр                 // 4b
    uint16_t  RepetitionMode;    // On demand / Streaming          // 2b
    uint16_t  PowerSaveMode;     // Режимы заданы перечислением    // 2b
    float     ReceiverGain;      // 0.0 - 1.0                      // 4b
    uint16_t  TXDisable;         // bool                           // 2b
    int       HWAAS;             // 1-63                           // 4b
    uint16_t  Profile;           // заданны перечислением (1-5)    // 2b
    uint16_t  MaximizeSignal;    // bool                           // 2b
    uint16_t  AsynchMeasure;     // bool                           // 2b
    int       DownSampFactor;    // 1,2,4 (“делитель волны”)       // 4b
    float     RunningAverage;    // 0.0 - 1.0                      // 4b
    uint16_t  NoiseLevel;        // bool                           // 2b
    float     logA;                                                // 4b
    float     logB;                                                // 4b <-33

    uint16_t  AdrModbus;        // Адрес модбас                    // 2b <-35
    uint16_t  TypeApproxim;     // Тип аппроксимации               // 2b
    uint16_t  TypeAverag;       // Тип усреднения                  // 2b
    uint16_t  IntervalAverag;   // Интервал усреднения             // 2b
    uint16_t  Password;         // Пароль                          // 2b <-39
    uint16_t  Reslift;                                             // 2b 82b  <- 40 adr

    uint64_t  timeconnect;      // Дата последней связи            // 8b 90b  <- 41 adr
    uint64_t  timechange;       // Дата изменения настроек         // 8b
    uint32_t  idchange;         // id пользователя внес изменения  // 4b
    uint64_t  timedefault;      // Дата изменения заводских        // 8b
    uint32_t  iddefault;        // id пользователя внес заводские  // 4b
    uint16_t  TypeDevice;       // тип устройства 		           // 2b
    uint32_t  SerialNum;        // серийный номер устройства       // 4b
    uint32_t  VerApp;           // аппаратная версия устройства    // 4b
    uint32_t  idset;            // id пользователя создавший настройки //4b
    uint16_t  mas[8];           // доп резерв                      // 16b 144b <- 62 adr
}struct_tableRegsWrite;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union union_tableRegsWrite {
              struct_tableRegsWrite Regs;
              uint16_t              Adr[sizeof(struct_tableRegsWrite)/2];
}union_tableRegsWrite;
#pragma pack(pop)

#endif // STRUCTS_LIB_H
