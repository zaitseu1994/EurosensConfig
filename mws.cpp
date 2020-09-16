#include "mws.h"
#include "ui_mws.h"
#include <string.h>

#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QSerialPortInfo>
#include <QSerialPort>

#include "structs_lib.h"

#define MODBUS_TIMEOUT_PACKET 200
#define MODBUS_COUNT_REPAET   1

#define MODBUS_INTERVAL_ALL 800

MWS::MWS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MWS)
{
    ui->setupUi(this);

    ModbusRegsTimer = new QTimer();
    ModbusRegsTimer->stop();
    ModbusRegsTimer->setSingleShot(false);
    ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
    connect(ModbusRegsTimer,&QTimer::timeout,this,&MWS::updateRegs);
}

MWS::~MWS()
{
    if(ModbusRegsTimer->isActive())
    ModbusRegsTimer->stop();
    delete ModbusRegsTimer;
    delete ui;
}

void MWS::getStr(QString str)
{
    ui->edit_Password->setText(str);
    struct_listSavedDevices table = stringToTable(str);
    memcpy(&device.device,&table.device,sizeof(device.device));
    device.devicename = table.devicename;
    device.modbusadr = table.modbusadr;
    device.portname = table.portname;
    ui->textBrowser->append("getStr");
}

struct_listSavedDevices MWS::stringToTable(QString str)
{
     QStringList name = str.split(";");
     struct_listSavedDevices table;
     if( name.count()==4)
     {
         QStringList tablstr  = name[3].split(",");
         table.devicename = name[0];
         table.portname = name[1];
         table.modbusadr = name[2];
         if( tablstr.count()==5)
         {
             table.device.Regs.LastDate = tablstr[0].toUInt();
             table.device.Regs.LogError = tablstr[1].toUShort();
             table.device.Regs.SerialNum = tablstr[2].toUInt();
             table.device.Regs.TypeDevice = tablstr[3].toUShort();
             table.device.Regs.VerApp = tablstr[4].toUInt();
         }
     }
     return table;
}

void MWS::start(QModbusClient *modbusDev)
{
      modbusDevice = modbusDev;
    if(modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        modbusDevice->setTimeout(MODBUS_TIMEOUT_PACKET);
        modbusDevice->setNumberOfRetries(MODBUS_COUNT_REPAET);
        ModbusRegsTimer->start();
    }
}

void MWS::updateRegs()
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit request;
        request.setRegisterType(QModbusDataUnit::HoldingRegisters);
        request.setStartAddress(0);
        request.setValueCount(41);

        if (auto *reply =  modbusDevice->sendReadRequest(request, device.modbusadr.toInt())) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &MWS::replyReceived);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
               // ui->textBrowser->append("Read error: " + vectorModbusDevice.first().modbusDev->errorString());
                ui->lcd_Distance->display("хрень");
        }
    } else
    {
       ModbusRegsTimer->stop();
    }
}

void MWS::replyReceived()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

            if ( int(unit.valueCount()) == sizeof(struct_tableRegsWrite)/2 )
            {
                union_tableRegsWrite LoclTable;
                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTable.Adr[i] = unit.value(i);
                }
                updateAllSettingsView(LoclTable);
            }else
            {
                ui->textBrowser->append(QString::number(int(unit.valueCount())));
            }
        } else {
                ui->textBrowser->append(replyModbus->errorString());
        }
        replyModbus->deleteLater();
}

void MWS::updateAllSettingsView(union_tableRegsWrite Table)
{
     ui->lcd_Distance->display(Table.Regs.CurrentDistanse);
}
