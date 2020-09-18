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

    connect(ui->spin_CountPoint,QOverload<int>::of(&QSpinBox::valueChanged),this,&MWS::updateTableWidget);

    connect(ui->button_SendTable,&QPushButton::clicked,this,[=]
    {
        readTableWidget();
        setupAction(WRITE_TABLE);
        currentPointTableCalibration = 0;
        ModbusRegsTimer->stop();
        ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL/10);
        ModbusRegsTimer->start();
        ui->button_SendTable->setEnabled(false);
        ui->button_ReseiveTable->setEnabled(false);
    });

   connect(ui->button_ReseiveTable,&QPushButton::clicked,this,[=]
   {
       TableCalibration.clear();
       setupAction(READ_TABLE);
       ModbusRegsTimer->stop();
       ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL/10);
       ModbusRegsTimer->start();
       ui->button_SendTable->setEnabled(false);
       ui->button_ReseiveTable->setEnabled(false);
   });

 //  connect(ui->tableWidget,&QTableWidget::cellChanged,this,&MWS::updatePlotWidget);
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
                upperModbusCheck();
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
            upperModbusCheck();
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

void MWS::upperModbusCheck()
{
       switch ( CurrentAction )
       {
       case NO_ACTION:
       {

       }break;
       case SEND_TO_SAVE_CONFIG:
       {

       }break;
       case SEND_TO_UPDATE_CONFIG:
       {

       }break;
       case SEND_TO_CHECK_PASSWORD:
       {

       }break;
       case WRITE_TABLE:
       {
            ActionWriteTable();
       }break;
       case READ_TABLE:
       {
            ActionReadTable();
       }break;
       default:
       break;
       };
}

void MWS::setupAction(Action Action)
{
    CurrentAction = Action;
    CounterStepAction = 0;
}


void MWS::ActionReadTable()
{
    switch( CounterStepAction )
    {
    case 0:
    {
      LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_POZITION;
      CounterStepAction = 1;
    }break;
    case 1:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_POZITION )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          CounterStepAction = 2;
      }
    }break;
    case 2:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_TRANSMIT;
        CounterStepAction = 3;
    }break;
    case 3:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_TRANSMIT )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            struct_pointTableCalibration point;
            point.pointDistanse = LoclTableRecieve.Regs.RegTransfer1;
            point.pointVolume = LoclTableRecieve.Regs.RegTransfer2;
            TableCalibration << point;
            CounterStepAction = 2;
        }else
        {
            if ( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_END )
            {
                LoclTableRecieve.Regs.RegStatus = 0;
                currentPointTableCalibration = 0;
                CurrentAction = NO_ACTION;
                CounterStepAction = 0;
                ModbusRegsTimer->stop();
                ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
                ModbusRegsTimer->start();
                writeTableWidget();
                ui->button_SendTable->setEnabled(true);
                ui->button_ReseiveTable->setEnabled(true);
            }
        }
    }break;
    }
}

void MWS::ActionWriteTable()
{
    switch( CounterStepAction )
    {
    case 0:
    {
      LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_CLEAR;
      CounterStepAction = 1;
    }break;
    case 1:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_CLEAR )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          CounterStepAction = 2;
      }
    }break;
    case 2:
    {
          if( currentPointTableCalibration < TableCalibration.count())
          {
              LoclTableRecieve.Regs.RegTransfer1 = TableCalibration[currentPointTableCalibration].pointDistanse;
              LoclTableRecieve.Regs.RegTransfer2 = TableCalibration[currentPointTableCalibration].pointVolume;
              LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_RECEIVE;
              currentPointTableCalibration++;
              CounterStepAction = 3;
          }else
          {
              LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_WRITE_END;
              CounterStepAction = 4;
          }
    }break;
    case 3:
    {
          if ( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_RECEIVE )
          {
               LoclTableRecieve.Regs.RegStatus = 0;
               CounterStepAction = 2;
          }else
          {
              if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_END )
              {
                  LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_WRITE_END;
                  CounterStepAction = 4;
              }
          }
    }break;
    case 4:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_END )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          currentPointTableCalibration = 0;
          CurrentAction = NO_ACTION;
          CounterStepAction = 0;
          ModbusRegsTimer->stop();
          ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
          ModbusRegsTimer->start();
          ui->button_SendTable->setEnabled(true);
          ui->button_ReseiveTable->setEnabled(true);
      }
    }break;
    }
}

void MWS::updateTableWidget(int countPoints)
{
    QTableWidget *tableWidget = ui->tableWidget;
    tableWidget->setColumnCount(2);
    QTableWidgetItem *itemHead1 = new QTableWidgetItem("Расстояние, мм");
    QTableWidgetItem *itemHead2 = new QTableWidgetItem("Обьем, л");
    tableWidget->setHorizontalHeaderItem(0,itemHead1);
    tableWidget->setHorizontalHeaderItem(1,itemHead2);
    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(countPoints);

    if ( rowCount < countPoints )
    {
        for(int row=rowCount; row!=tableWidget->rowCount(); ++row){
            for(int column = 0; column!=tableWidget->columnCount(); ++column) {
                QTableWidgetItem *newItem = new QTableWidgetItem("Введите число..");
                tableWidget->setItem(row, column, newItem);
            }
        }
    }
}

void MWS::readTableWidget()
{
    QTableWidget *tableWidget = ui->tableWidget;
    int rowCount = tableWidget->rowCount();
    TableCalibration.clear();
    for( int row = 0; row!=rowCount; ++row  )
    {
        struct_pointTableCalibration point;
        point.pointDistanse = tableWidget->item(row,0)->text().toShort();
        point.pointVolume = tableWidget->item(row,1)->text().toShort();
        TableCalibration << point;
    }
}

void MWS::writeTableWidget()
{
    QTableWidget *tableWidget = ui->tableWidget;
    tableWidget->setColumnCount(2);
    QTableWidgetItem *itemHead1 = new QTableWidgetItem("Расстояние, мм");
    QTableWidgetItem *itemHead2 = new QTableWidgetItem("Обьем, л");
    tableWidget->setHorizontalHeaderItem(0,itemHead1);
    tableWidget->setHorizontalHeaderItem(1,itemHead2);
    tableWidget->setRowCount(TableCalibration.count());

    for(int row = 0; row!=tableWidget->rowCount(); ++row){
        QTableWidgetItem *ItemDistanse = new QTableWidgetItem(QString::number(TableCalibration[row].pointDistanse));
        QTableWidgetItem *ItemVolume = new QTableWidgetItem(QString::number(TableCalibration[row].pointVolume));
        tableWidget->setItem(row, 0, ItemDistanse);
        tableWidget->setItem(row, 1, ItemVolume);
    }
    ui->spin_CountPoint->setValue(TableCalibration.count());
}


void MWS::updatePlotWidget(int s,int k)
{
    QVector<double> distanceX,volumeY;
    QTableWidget *tableWidget = ui->tableWidget;
    double testX,testY;
    for(int row = 0; row!=tableWidget->rowCount(); ++row){
            testX = tableWidget->item(row,0)->text().toDouble();
            if(testX)
            distanceX << testX;
            testY = tableWidget->item(row,1)->text().toDouble();
            if(testY)
            volumeY <<  testY;
    }

    //Вычисляем наши данные

    ui->widget_2->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    ui->widget_2->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    ui->widget_2->graph(0)->setData(distanceX, volumeY);

    //Подписываем оси Ox и Oy
    ui->widget_2->xAxis->setLabel("Расстояние");
    ui->widget_2->yAxis->setLabel("Обьем");

   //Установим область, которая будет показываться на графике

   // ui->widget_2->xAxis->setRange(a, b);//Для оси Ox

    //Для показа границ по оси Oy сложнее, так как надо по правильному
    //вычислить минимальное и максимальное значение в векторах
    double minY = volumeY[0], maxY = volumeY[0];
    for (int i=1; i<volumeY.count(); i++)
    {
        if (volumeY[i]<minY) minY = volumeY[i];
        if (volumeY[i]>maxY) maxY = volumeY[i];
    }
    ui->widget_2->yAxis->setRange(minY, maxY);

    ui->widget_2->replot();
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
