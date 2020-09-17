#include "mws.h"
#include "ui_mws.h"
#include <string.h>

#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSpinBox>

#include "structs_lib.h"

#define MODBUS_TIMEOUT_PACKET 200
#define MODBUS_COUNT_REPAET   1

#define MODBUS_INTERVAL_ALL 500

QStringList STypeApproximation = {"кусочно линейный","квадратичный"};
QStringList STypeAverage = {"экспоненциальный","бегущее среднее"};
QStringList SNoize = {"да","нет"};
QStringList SDownSampFactor = {"1","2","4"};
QStringList SMaximizeSignal = {"да","нет"};
QStringList SPowerSaveMode = {"выключен","сон","готовность","активный","гибернация"};
QStringList SAsynchMeasure = {"да","нет"};
QStringList SRepetitionMode = {"внешнее управление","управление сенсором"};
QStringList STXDisable = {"да","нет"};
QStringList SProfile = {"1","2","3","4","5"};

MWS::MWS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MWS)
{
    ui->setupUi(this);

    ModbusRegsTimer = new QTimer();
    ModbusRegsTimer->stop();
    ModbusRegsTimer->setSingleShot(false);
    ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
    connect(ModbusRegsTimer,&QTimer::timeout,this,[=]
    {
         sendRegs();
         updateRegs();
    });

    connect(ui->slid_AdrModbus,&QSlider::valueChanged,ui->spin_AdrModbus,&QSpinBox::setValue);
    connect(ui->spin_AdrModbus,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_AdrModbus,&QSlider::setValue);

    connect(ui->slid_IntAverage,&QSlider::valueChanged,ui->spin_IntAverage,&QSpinBox::setValue);
    connect(ui->spin_IntAverage,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_IntAverage,&QSlider::setValue);

    connect(ui->slid_IdSensor,&QSlider::valueChanged,ui->spin_IdSensor,&QSpinBox::setValue);
    connect(ui->spin_IdSensor,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_IdSensor,&QSlider::setValue);

    connect(ui->slid_HWAAS,&QSlider::valueChanged,ui->spin_HWAAS,&QSpinBox::setValue);
    connect(ui->spin_HWAAS,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_HWAAS,&QSlider::setValue);

    connect(ui->slid_StartMeasure,&QSlider::valueChanged,ui->spin_StartMeasure,&QSpinBox::setValue);
    connect(ui->spin_StartMeasure,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_StartMeasure,&QSlider::setValue);

    connect(ui->slid_EndMeasure,&QSlider::valueChanged,ui->spin_EndMeasure,&QSpinBox::setValue);
    connect(ui->spin_EndMeasure,QOverload<int>::of(&QSpinBox::valueChanged),ui->slid_EndMeasure,&QSlider::setValue);

    connect(ui->slid_ReceiverGain,&QSlider::valueChanged,this,[=]
    {
       ui->spin_ReceiverGain->setValue((static_cast<double>(ui->slid_ReceiverGain->value()))/100);
    });
    connect(ui->spin_ReceiverGain,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,[=]
    {
       ui->slid_ReceiverGain->setValue(static_cast<int>(ui->spin_ReceiverGain->value()*100));
    });

    connect(ui->slid_RunningAverage,&QSlider::valueChanged,this,[=]
    {
        ui->spin_RunningAverage->setValue((static_cast<double>(ui->slid_RunningAverage->value()))/100);
    });
    connect(ui->spin_RunningAverage,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,[=]
    {
       ui->slid_RunningAverage->setValue(static_cast<int>(ui->spin_RunningAverage->value()*100));
    });
    firstRequest = true;

//    connect(ui->button_Update,&QPushButton::clicked,this,[=]
//    {
//        sendRegs();
//        updateRegs();
//    });
}

MWS::~MWS()
{
    if(ModbusRegsTimer->isActive())
    ModbusRegsTimer->stop();
    delete ModbusRegsTimer;
    delete ui;
}

void MWS::startView()
{
    ui->comb_TypeApproximation->addItems(STypeApproximation);
    ui->comb_TypeAverage->addItems(STypeAverage);
    ui->comb_Noize->addItems(SNoize);
    ui->comb_DownSampFactor->addItems(SDownSampFactor);
    ui->comb_MaximizeSignal->addItems(SMaximizeSignal);
    ui->comb_PowerSaveMode->addItems(SPowerSaveMode);
    ui->comb_AsynchMeasure->addItems(SAsynchMeasure);
    ui->comb_RepetitionMode->addItems(SRepetitionMode);
    ui->comb_TXDisable->addItems(STXDisable);
    ui->comb_Profile->addItems(SProfile);
}

void MWS::getStr(QString str)
{
    ui->edit_Password->setText(str);
    struct_listSavedDevices table = stringToTable(str);
    memcpy(&device.device,&table.device,sizeof(device.device));
    device.devicename = table.devicename;
    device.modbusadr = table.modbusadr;
    device.portname = table.portname;
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
    startView();
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
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,0,41);
        QModbusReply *reply = nullptr;
        reply =  modbusDevice->sendReadRequest(readUnit, device.modbusadr.toInt());
        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &MWS::replyReceivedRead);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
                ui->lcd_Distance->display("хрень");
        }
    } else
    {
       ModbusRegsTimer->stop();
    }
}

void MWS::replyReceivedRead()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();
            if ( int(unit.valueCount()) == sizeof(struct_tableRegsWrite)/2 )
            {
                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTableRecieve.Adr[i] = unit.value(i);
                }
                updateAllSettingsView(LoclTableRecieve);
                firstRequest=false;
            }else
            {

            }
        } else {
            ui->edit_Password->setText("sdf");
        }
        replyModbus->deleteLater();
}

void MWS::sendRegs()
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 41);
        QModbusReply *reply = nullptr;
        if(!firstRequest)
        {
            updateAllSettingsTable(&LoclTableRecieve);
            for (int i = 0, total = int(writeUnit.valueCount()); i < total; i++) {
                 writeUnit.setValue(i,LoclTableRecieve.Adr[i]);
            }
            reply =  modbusDevice->sendWriteRequest(writeUnit,device.modbusadr.toInt());
            if (reply) {
                if (!reply->isFinished())
                {
                     connect(reply, &QModbusReply::finished, this, &MWS::replyReceivedWrite);
                }
                else
                    delete reply; // broadcast replies return immediately
            } else {
                    ui->lcd_Distance->display("хрень");
            }
        }
        else
        {
          // сразу запрос на чтение
        }
    } else
    {
       ModbusRegsTimer->stop();
    }
}

void MWS::replyReceivedWrite()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

        } else {
              // ui->textBrowser->append(replyModbus->errorString());
            ui->edit_Password->setText("sdf");
        }
        replyModbus->deleteLater();
}


void MWS::updateAllSettingsView(union_tableRegsWrite Table)
{
     ui->lcd_Distance->display(Table.Regs.CurrentDistanse);
     ui->lcd_Volume1->display(Table.Regs.CurrentVolume);

     ui->spin_AdrModbus->setValue(Table.Regs.AdrModbus);

     ui->spin_EndMeasure->setValue(Table.Regs.LenghtOfMeasure);
     ui->spin_HWAAS->setValue(Table.Regs.HWAAS);
     ui->spin_IdSensor->setValue(Table.Regs.SensorId);
     ui->spin_IntAverage->setValue(Table.Regs.IntervalAverag);
     ui->spin_ReceiverGain->setValue(Table.Regs.ReceiverGain);
     ui->spin_RunningAverage->setValue(Table.Regs.RunningAverage);
     ui->spin_StartMeasure->setValue(Table.Regs.StartOfMeasure);

     ui->comb_AsynchMeasure->setCurrentIndex(Table.Regs.AsynchMeasure);
     ui->comb_DownSampFactor->setCurrentIndex(Table.Regs.DownSampFactor);
     ui->comb_MaximizeSignal->setCurrentIndex(Table.Regs.MaximizeSignal);
     ui->comb_Noize->setCurrentIndex(Table.Regs.NoiseLevel);
     ui->comb_PowerSaveMode->setCurrentIndex(Table.Regs.PowerSaveMode);
     ui->comb_Profile->setCurrentIndex(Table.Regs.Profile);
     ui->comb_RepetitionMode->setCurrentIndex(Table.Regs.RepetitionMode);
     ui->comb_TXDisable->setCurrentIndex(Table.Regs.TXDisable);
     ui->comb_TypeApproximation->setCurrentIndex(Table.Regs.TypeApproxim);
     ui->comb_TypeAverage->setCurrentIndex(Table.Regs.TypeAverag);
}

void MWS::updateMainSettingsView(union_tableRegsWrite Table)
{
     ui->lcd_Distance->display(Table.Regs.CurrentDistanse);
     ui->lcd_Volume1->display(Table.Regs.CurrentVolume);
}

void MWS::updateAllSettingsTable(union_tableRegsWrite *Table)
{
     Table->Regs.AdrModbus = ui->spin_AdrModbus->value();
     Table->Regs.LenghtOfMeasure = ui->spin_EndMeasure->value();
     Table->Regs.HWAAS = ui->spin_HWAAS->value();

     Table->Regs.SensorId = ui->spin_IdSensor->value();
     Table->Regs.IntervalAverag = ui->spin_IntAverage->value();
     Table->Regs.ReceiverGain = ui->spin_ReceiverGain->value();
     Table->Regs.RunningAverage = ui->spin_RunningAverage->value();
     Table->Regs.StartOfMeasure = ui->spin_StartMeasure->value();
}
