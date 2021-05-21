#ifndef STRUCT_LIB_DIF_H
#define STRUCT_LIB_DIF_H

#include <stdint.h>
#include <QString>

#define COUNT_POINT 10
;
#pragma pack(push, 1)
typedef struct struct_angls {
    uint8_t anglX;
    uint8_t anglY;
    uint8_t anglZ;
}struct_angls;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct struct_settings {
    uint16_t set[10];
}struct_settings;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct struct_tableRegsWrite
{
    struct_settings settings; //20 байтов (10 регистров модбас)
    uint16_t  adc_chanal1[COUNT_POINT];// 2байта*COUNT_POINT
    uint16_t  adc_chanal2[COUNT_POINT];// 2байта*COUNT_POINT
    uint16_t  adc_chanal3[COUNT_POINT];// 2байта*COUNT_POINT
    uint16_t  adc_chanal4[COUNT_POINT];// 2байта*COUNT_POINT
    struct_angls  angls1[COUNT_POINT];// 6байта*COUNT_POINT
    struct_angls  angls2[COUNT_POINT];// 6байта*COUNT_POINT
}struct_tableRegsWrite;
#pragma pack(pop)

typedef union uinon_tableRegsWrite {
              struct_tableRegsWrite Regs;
              uint16_t              Adr[sizeof(struct_tableRegsWrite)/2];
}uinon_tableRegsWrite;


#endif // STRUCT_LIB_DIF_H
