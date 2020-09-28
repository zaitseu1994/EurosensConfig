#include "mws.h"
#include "ui_mws.h"
#include <string.h>

#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSpinBox>

#include <QPointF>

#include "structs_lib.h"
#include "math.h"

#define MODBUS_TIMEOUT_PACKET 200
#define MODBUS_COUNT_REPAET   1

#define MODBUS_COUNT_READ_ADR sizeof(struct_tableRegsWrite)/2

#define MODBUS_INTERVAL_ALL 400
#define MODBUS_INTERVAL_FAST 40

QStringList STypeApproximation = {"кусочно линейный","полином Лангранжа"};
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
    QWidget(parent),
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
        ModbusRegsTimer->setInterval(MODBUS_INTERVAL_FAST);
        ModbusRegsTimer->start();
        ui->button_SendTable->setEnabled(false);
        ui->button_ReseiveTable->setEnabled(false);
    });

   connect(ui->button_ReseiveTable,&QPushButton::clicked,this,[=]
   {
       TableCalibration.clear();
       setupAction(READ_TABLE);
       ModbusRegsTimer->stop();
       ModbusRegsTimer->setInterval(MODBUS_INTERVAL_FAST);
       ModbusRegsTimer->start();
       ui->button_SendTable->setEnabled(false);
       ui->button_ReseiveTable->setEnabled(false);
   });
   connect(ui->button_Accept,&QPushButton::clicked,this,[=]
   {
      setupAction(SEND_TO_SAVE_CONFIG);
      ModbusRegsTimer->stop();
      ModbusRegsTimer->setInterval(MODBUS_INTERVAL_FAST);
      ModbusRegsTimer->start();
      ui->button_Accept->setEnabled(false);
      ui->button_Update->setEnabled(false);
   });
   connect(ui->button_Update,&QPushButton::clicked,this,[=]
   {
       setupAction(SEND_TO_UPDATE_CONFIG);
       ModbusRegsTimer->stop();
       ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
       ModbusRegsTimer->start();
       ui->button_Accept->setEnabled(false);
       ui->button_Update->setEnabled(false);
   });

   connect(ui->tableWidget,&QTableWidget::cellChanged,this,&MWS::updatePlotWidget);
   connect(ui->check_Lagranj,&QCheckBox::clicked,this,[=]
   {
       updatePlotWidget(0,0);
   });
   connect(ui->check_Linear,&QCheckBox::clicked,this,[=]
   {
       updatePlotWidget(0,0);
   });

   ui->graph_table->clearGraphs();
   ui->graph_table->addGraph(); // наши точки
   ui->graph_table->addGraph(); // аппроксимация лагранжа
   ui->graph_table->addGraph(); // аппроксимация кусочно линейная
   ui->graph_table->addGraph(); // текущий обьем по Y
   ui->graph_table->addGraph(); // текущее расстояние по X
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
    strWindow = str;
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

        TableCalibration.clear();
        setupAction(READ_TABLE);
        ModbusRegsTimer->stop();
        ModbusRegsTimer->setInterval(MODBUS_INTERVAL_FAST);
        ModbusRegsTimer->start();
        ui->button_SendTable->setEnabled(false);
        ui->button_ReseiveTable->setEnabled(false);
    }
}

void MWS::updateRegs()
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,0,MODBUS_COUNT_READ_ADR);
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
                   addPointMeasure(LoclTableRecieve.Regs.CurrentDistanse,LoclTableRecieve.Regs.CurrentVolume);
                   upperModbusCheck();
                   firstRequest=false;
            }else
            {

            }
        } else {
            ui->edit_Password->setText("sdf");
            ModbusRegsTimer->stop();
        }
        replyModbus->deleteLater();
}

void MWS::sendRegs()
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, MODBUS_COUNT_READ_ADR);
        QModbusReply *reply = nullptr;
        if(!firstRequest)
        {
            updateAllSettingsTable(&LoclTableRecieve);
            upperModbusCheck();
            for (int i = 0, total = int(writeUnit.valueCount()); i < total; i++) {
                 writeUnit.setValue(i,LoclTableRecieve.Adr[i]);
            }
            reply =  modbusDevice->sendWriteRequest(writeUnit,device.modbusadr.toInt());
            LoclTableRecieve.Regs.RegCommand = 0;
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
            ui->edit_Password->setText("sdf");
            ModbusRegsTimer->stop();
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
             ActionSaveConfig();
       }break;
       case SEND_TO_UPDATE_CONFIG:
       {
             ActionReadConfig();
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

void MWS::ActionSaveConfig()
{
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_SAVE_CONFIG;
        CounterStepAction = 1;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CONFIG_SAVE )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CurrentAction = NO_ACTION;
            CounterStepAction = 0;
            ModbusRegsTimer->stop();
            ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
            ModbusRegsTimer->start();
            ui->button_Accept->setEnabled(true);
            ui->button_Update->setEnabled(true);
        }
    }break;
    }
}
void MWS::ActionReadConfig()
{
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_UPDATE_CONFIG;
        CounterStepAction = 1;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CONFIG_UPDATE )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CurrentAction = NO_ACTION;
            CounterStepAction = 0;
            ModbusRegsTimer->stop();
            ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
            ModbusRegsTimer->start();
            ui->button_Accept->setEnabled(true);
            ui->button_Update->setEnabled(true);
        }
    }break;
    }
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
        point.pointVolume = tableWidget->item(row,1)->text().toFloat();
        TableCalibration << point;
    }

    const int length = TableCalibration.count();
    for (int startIndex = 0; startIndex < length - 1; ++startIndex)
    {
        int smallestIndex = startIndex;
        for (int currentIndex = startIndex + 1; currentIndex < length; ++currentIndex)
        {
            if (TableCalibration[currentIndex].pointDistanse < TableCalibration[smallestIndex].pointDistanse)

                smallestIndex = currentIndex;
        }
        std::swap(TableCalibration[startIndex], TableCalibration[smallestIndex]);
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


void MWS::readPlotGraph()
{
    QTableWidget *tableWidget = ui->tableWidget;
    double testX,testY;
    graphTable.clear();
    for(int row = 0; row!=tableWidget->rowCount(); ++row){
        QTableWidgetItem *ItemDistanse = tableWidget->item(row,0);
        QTableWidgetItem *ItemVolume   = tableWidget->item(row,1);
        if(ItemDistanse!=NULL && ItemVolume!=NULL)
        {
            testX = tableWidget->item(row,0)->text().toDouble();
            testY = tableWidget->item(row,1)->text().toDouble();
            if ( testX && testY)
            {
               QPointF p(testX,testY);
               graphTable << p;
            }
        }
    }

    const int length = graphTable.count();
    for (int startIndex = 0; startIndex < length - 1; ++startIndex)
    {
        int smallestIndex = startIndex;
        for (int currentIndex = startIndex + 1; currentIndex < length; ++currentIndex)
        {
            if (graphTable[currentIndex].rx() < graphTable[smallestIndex].rx())

                smallestIndex = currentIndex;
        }
        std::swap(graphTable[startIndex], graphTable[smallestIndex]);
    }

}

void MWS::addLagranj(double maxX,double minX)
{
        QVector<double> AproximationX,AproximationY;
        int rezolution  = (graphTable.count())*100;
        double h = ( (maxX - minX ) / rezolution);
        for(int i=0;i<rezolution;++i)
        {
            AproximationX << minX + h*i;
            AproximationY << Lagranj( minX+h*i);
        }
        ui->graph_table->graph(1)->setData(AproximationX, AproximationY);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(QColor(0x2D,0x05,0x71));
        ui->graph_table->graph(1)->setPen(pen);
}

void MWS::addLinear(double maxX,double minX)
{
    QVector<double> AproximationX,AproximationY;
    int rezolution  = (graphTable.count())*10;
    double h = ( (maxX - minX ) / rezolution);
    for(int i=0;i<rezolution+1;++i)
    {
        AproximationX << minX + h*i;
        AproximationY << Linear( minX+h*i);
    }
    ui->graph_table->graph(2)->setData(AproximationX, AproximationY);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0x2D,0x71,0x05));
    ui->graph_table->graph(2)->setPen(pen);
}

void MWS::addMeasure(double distatanse,double volume)
{
    QVector<double> distanseX,distaseY;
    QVector<double> volumeX,volumeY;

    double maxX = ui->graph_table->xAxis->range().upper;
    double maxY = ui->graph_table->yAxis->range().upper;

    distanseX << distatanse;
    distaseY << 0;
    distanseX << distatanse;
    distaseY << maxY;

    volumeX << 0;
    volumeY << volume;
    volumeX << maxX;
    volumeY << volume;

    ui->graph_table->graph(3)->setData(volumeX, volumeY);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0x55,0x05,0x05));
    ui->graph_table->graph(3)->setPen(pen);

    ui->graph_table->graph(4)->setData(distanseX, distaseY);
    QPen pens;
    pens.setWidth(1);
    pens.setColor(QColor(0x55,0x05,0x05));
    ui->graph_table->graph(4)->setPen(pens);

    ui->graph_table->replot();
}

void MWS::addPointMeasure(double distatanse,double volume)
{
//     double volum = 0;
//    if(LoclTableRecieve.Regs.TypeApproxim == 0)
//       volum = Linear(distatanse);
//    else
//       volum = Lagranj(distatanse);
    addMeasure(distatanse,volume);

}

void MWS::updatePlotWidget(int s,int k)
{

    readPlotGraph();

    ui->graph_table->clearGraphs();//Если нужно, но очищаем все графики
    ui->graph_table->addGraph(); // наши точки
    ui->graph_table->addGraph(); // аппроксимация лагранжа
    ui->graph_table->addGraph(); // аппроксимация кусочно линейная
    ui->graph_table->addGraph(); // текущий обьем по Y
    ui->graph_table->addGraph(); // текущее расстояние по X

    QVector<double> distanceX,volumeY;
    for(int i=0;i<graphTable.count();i++)
    {
       distanceX << graphTable[i].rx();
       volumeY << graphTable[i].ry();
    }

    ui->graph_table->graph(0)->setData(distanceX, volumeY);
    QPen penM;
    penM.setWidth(1);
    penM.setColor(QColor(0xE8,0x10,0x40));
    ui->graph_table->graph(0)->setPen(penM);
    ui->graph_table->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->graph_table->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    //Подписываем оси Ox и Oy
    ui->graph_table->xAxis->setLabel("Расстояние");
    ui->graph_table->yAxis->setLabel("Обьем");

    if ( volumeY.count()> 0 && distanceX.count() > 0 )
    {
         double maxX = distanceX[0], maxY = volumeY[0];
         double minX = distanceX[0];
         for (int i=1; i<volumeY.count(); i++)
         {
            if (volumeY[i]>maxY) maxY = volumeY[i];
         }
         ui->graph_table->yAxis->setRange(0, maxY+maxY/10);

         for (int i=1; i<distanceX.count(); i++)
         {
            if (distanceX[i]>maxX) maxX = distanceX[i];
            if (distanceX[i]<minX) minX = distanceX[i];
         }
         ui->graph_table->xAxis->setRange(minX-maxX/10, maxX+maxX/10);

         if(ui->check_Lagranj->isChecked())
         addLagranj(maxX,minX);
         if(ui->check_Linear->isChecked())
         addLinear(maxX,minX);
    }
    ui->graph_table->replot();
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

     Table->Regs.TypeApproxim = ui->comb_TypeApproximation->currentIndex();
     Table->Regs.TypeAverag = ui->comb_TypeAverage->currentIndex();
}

qreal MWS::Linear (double X)
{
   double x0 = 0,x1 = 0,y0  =0,y1 = 0;
   qreal Fx = 0;
   if(graphTable.count() >=2)
   {
      for(int i=graphTable.count()-1;i>=1;i--)
      {
          if( abs(X - graphTable[i].rx()) + abs(X - graphTable[i-1].rx())
                      == abs(graphTable[i-1].rx() - graphTable[i].rx()))
          {
                 x0 =  graphTable[i-1].rx();
                 y0 =  graphTable[i-1].ry();
                 x1 =  graphTable[i].rx();
                 y1 =  graphTable[i].ry();
              break;
          }
      }
      if(x0 && y0 && x1 && y1)
      {
         Fx = y0+(y1-y0)/(x1-x0)*(X-x0);
      }
   }
   return Fx;
}

qreal MWS::Lagranj (double X)
{
    int n = graphTable.count();
    qreal L, l;
    int i, j;

    L = 0;

    for (i = 0; i < n; ++i)
    {
        l = 1;

        for (j = 0; j < n; ++j)
            if (i != j)
                l *= (X - graphTable[j].rx()) / (graphTable[i].rx() - graphTable[j].rx());
        L += graphTable[i].ry() * l;
    }
    return L;
}
