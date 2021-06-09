#include "mws.h"
#include "ui_mws.h"
#include <string.h>

#include <QModbusDataUnit>
#include <QModbusClient>
#include <QModbusRtuSerialMaster>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSpinBox>

#include <QTranslator>

#include <QPointF>


#include "structs_lib.h"
#include "math.h"

#define MODBUS_TIMEOUT_PACKET 300
#define MODBUS_COUNT_REPAET   1

#define MODBUS_COUNT_READ_ADR COUNT_REGSWRITE

#define MODBUS_INTERVAL_ALL 500
#define MODBUS_INTERVAL_FAST 150

#define LOGFILE_DATA_TIMER 1000 // 1000 мс
#define LOGFILE_SAVE_TIMER (1800) // в секундах

#define BOOT_TIMEOUT   5000

MWS::MWS(dev_base *parent) :
    dev_base(parent),
    ui(new Ui::MWS)
{
    ui->setupUi(this);

    ui->dat_lastconnect->setEnabled(false);
    ui->lin_idchange->setEnabled(false);
    ui->tabWidget->removeTab(4);

//    MouseEvent = new Mouseenter(this);
    setToolTips();

    LogXSLS = new QAction();
    this->addAction(LogXSLS);
    LogXSLS->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    connect(LogXSLS,&QAction::triggered,this,&MWS::logInit);

    poup = new PopUp();

//    connect(MouseEvent,&Mouseenter::signalMouseHoverEnter,this,[=](QLabel* lab)
//    {
//        QString str =  lab->toolTip();
//        emit MWSMouseEvent(str);
//    });

//    connect(MouseEvent,&Mouseenter::signalMouseHoverLeave,this,[=]()
//    {
//        QString str = tr("Наведите курсор чтоб получить информацию...");
//        emit MWSMouseEvent(str);
//    });


    connect(ui->button_defaultKoef,&QPushButton::clicked,this,[=]
    {
        ui->lin_koefA->setText("0.7294");
        ui->lin_koefB->setText("0.0503");
    });

    connect(ui->button_AddRealdistatnse,&QPushButton::clicked,this,[=]
    {
        QTableWidget *tableWidget = ui->tableWidget_2;
        tableWidget->setColumnCount(2);
        QTableWidgetItem *itemHead1 = new QTableWidgetItem(tr("Измеренное расстояние, мм"));
        QTableWidgetItem *itemHead2 = new QTableWidgetItem(tr("Реальное расстояние, мм"));
        itemHead1->setTextAlignment(Qt::AlignLeft);
        itemHead2->setTextAlignment(Qt::AlignLeft);

        tableWidget->setHorizontalHeaderItem(0,itemHead1);
        tableWidget->setHorizontalHeaderItem(1,itemHead2);
        int rowCount = tableWidget->rowCount();

        if( rowCount < 50)
        {
            tableWidget->insertRow(rowCount);

            QTableWidgetItem *newItemMeasure = new QTableWidgetItem();
            newItemMeasure->setText(ui->lcd_uncalibrDistanse->text());
            newItemMeasure->setTextAlignment(Qt::AlignLeft);
            tableWidget->setItem(tableWidget->rowCount()-1, 0, newItemMeasure);

            QTableWidgetItem *newItemReal = new QTableWidgetItem("sdf");
            newItemReal->setText(ui->lin_realDistatnse->text());
            newItemReal->setTextAlignment(Qt::AlignLeft);
            tableWidget->setItem(tableWidget->rowCount()-1, 1, newItemReal);

            tableWidget->resizeColumnsToContents();
            ui->lin_realDistatnse->clear();
        }
    });

    connect(ui->button_removeRealDistatnse,&QPushButton::clicked,this,[=]
    {
        QTableWidget *tableWidget = ui->tableWidget_2;
        tableWidget->removeRow(tableWidget->rowCount()-1);
    });

    connect(ui->button_calcKoef,&QPushButton::clicked,this,[=]
    {
       float arrayY[50];
       float arrayX[50];

       float koefA =0;
       float koefB =0;

       QTableWidget *tableWidget = ui->tableWidget_2;
       int countNum = tableWidget->rowCount();

       for(int i=0;i<tableWidget->rowCount();i++)
       {
          arrayX[i] = tableWidget->item(i,0)->text().toFloat();
          arrayY[i] = tableWidget->item(i,1)->text().toFloat()/arrayX[i];
          if(  arrayY[i]<0.8 || arrayY[i] > 1.2 )
          {
               QMessageBox::warning(this, tr("Калибровка"),tr("Проверьте правильность данных"));
               break;
          }
       }
       float sum_YlnX = 0;
       float sum_lnX =0;
       float sum_Y=0;
       float sum_ln2X=0;

       for(int i =0;i<countNum;i++)
       {
           sum_YlnX+= arrayY[i]*log(arrayX[i]);
           sum_lnX+=log(arrayX[i]);
           sum_Y+=arrayY[i];
           sum_ln2X+=log(arrayX[i])*log(arrayX[i]);
       }

       koefB =  (countNum*sum_YlnX-sum_lnX*sum_Y)/(countNum*sum_ln2X-sum_lnX*sum_lnX);
       koefA = (sum_Y/countNum);
       koefA = koefA - (koefB/countNum)*sum_lnX;
       ui->lin_koefA->setText(QString::number(koefA));
       ui->lin_koefB->setText(QString::number(koefB));
    });

    memset(&LoclTableRecieve,0,sizeof(LoclTableRecieve));

    static const char* const FILE_NAME = "lib.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window3;
    QByteArray window2;
    QByteArray window;
    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
            stream >> window3;
            stream >> window2;
            stream >> window;

        file.close();
        ui->splitter_3->restoreState(window3);
        ui->splitter_2->restoreState(window2);
        ui->splitter->restoreState(window);
    }

    ModbusRegsTimer = new QTimer();
    ModbusRegsTimer->stop();
    ModbusRegsTimer->setSingleShot(false);
    ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);

    LogFileTimer = new QTimer();
    LogFileTimer->stop();
    LogFileTimer->setSingleShot(false);
    LogFileTimer->setInterval(LOGFILE_DATA_TIMER);

    BootTimeout = new QTimer();
    BootTimeout->stop();
    BootTimeout->setSingleShot(true);
    BootTimeout->setInterval(BOOT_TIMEOUT);

    connect(LogFileTimer,&QTimer::timeout,this,&MWS::logSave);
    connect(BootTimeout,&QTimer::timeout,this,&MWS::bootloadTimeout);

    if( !ui->check_AddParam->isChecked() )
    {
        ui->widget_AddParam->hide();
    }

    connect(ui->check_AddParam,&QCheckBox::stateChanged,this,[=]
    {
        if( !ui->check_AddParam->isChecked() )
        {
            ui->widget_AddParam->hide();
        } else
        {
            ui->widget_AddParam->show();
        }
    });

    connect(ModbusRegsTimer,&QTimer::timeout,this,[=]
    {
        volatile static bool fastread = false;
        if(!firstRequest)
        if ( !isCurrentAction() )
        {
             if(!queueAction.isEmpty())
             {
                 Action act = queueAction.dequeue();
                 setupAction(act);
                 if((act == WRITE_TABLE) || (act == READ_TABLE))
                 {
                     ModbusRegsTimer->stop();
                     ModbusRegsTimer->setInterval(MODBUS_INTERVAL_FAST);
                     ModbusRegsTimer->start();
                     fastread = true;
                 }
             }else
             {
                 if ( fastread )
                 {
                      fastread =false;
                      ModbusRegsTimer->stop();
                      ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
                      ModbusRegsTimer->start();

                 }
                 if(!(this->isEnabled()))
                 {
                     this->setEnabled(true);
                     emit DevReady(device);
                 }
             }
        }

        type_send type = ONLY_MEASURE;
        if( ( CurrentAction == SEND_TO_UPDATE_CONFIG ) || ( CurrentAction == SEND_TO_SAVE_CONFIG ) || ( CurrentAction == SEND_TO_CHECK_PASSWORD ) )
        {
              type = ANOTHER_ACTIONS;
        }
        if ( CurrentAction == UPDATE_ADDREGS )
        {
             type = ADD_TABLE;
        }
        if ( CurrentAction == WRITE_TABLE || ( CurrentAction == READ_TABLE )
             || ( CurrentAction == SEND_TO_SAVE_FACTORY ) || (CurrentAction == SEND_DATA_CONNECT) )
        {
             type = RW_TABLE;
        }

        if(!firstRequest)
        switch(type)
        {
           case ONLY_MEASURE:
            {
                updateRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R);
            }
            break;
           case RW_TABLE:
            {
                sendRegs(START_MINIMAL_ADR_W,COUNT_MINIMAL_REGS_W);
                updateRegs(START_MINIMAL_ADR_W,COUNT_MINIMAL_REGS_W);
            }
            break;
           case UPDATE_SETTINGS:
            {
                sendRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R+COUNT_SENSOR_REGS+COUNT_DEV_REGS);
                updateRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R+COUNT_SENSOR_REGS+COUNT_DEV_REGS);
            }
            break;
           case ANOTHER_ACTIONS:
            {
                sendRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R+COUNT_SENSOR_REGS+COUNT_DEV_REGS);
                updateRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R+COUNT_SENSOR_REGS+COUNT_DEV_REGS);
            }
            break;
           case ADD_TABLE:
            {
                sendRegs(START_ADDDEV_ADR,COUNT_ADDDEV_REGS);
                updateRegs(START_ADDDEV_ADR,COUNT_ADDDEV_REGS);
                CurrentAction = NO_ACTION;
            }
            break;
         }
         else
         updateRegs(START_MINIMAL_ADR_R,COUNT_MINIMAL_REGS_R+COUNT_SENSOR_REGS+COUNT_DEV_REGS);
    });

    connect(ui->button_password,&QPushButton::clicked,this,[=]
    {
        QString str = ui->edit_Password->text();
        ui->edit_Password->clear();
        if ( str == "1111" )
        {
            QWidget* tab = ui->tabWidget->findChild<QWidget*>("addit",Qt::FindChildrenRecursively);
            ui->tabWidget->insertTab(4,tab,tr("Заводские"));
        }else
        {


        }
    });

    connect(ui->button_savefactory,&QPushButton::clicked,this,[=]
    {
            ui->tabWidget->removeTab(4);

            QDateTime datefactory = QDateTime::currentDateTime();

            LoclTableRecieve.Regs.timedefault = datefactory.toUTC().toTime_t();
            LoclTableRecieve.Regs.iddefault = idUser.toUInt();

            LoclTableRecieve.Regs.TypeDevice = ui->lin_typedev->text().toUShort();
            LoclTableRecieve.Regs.SerialNum = ui->lin_serial->text().toUInt();
            LoclTableRecieve.Regs.VerApp = ui->lin_verapp->text().toUInt();

            device.device.Regs.TypeDevice = LoclTableRecieve.Regs.TypeDevice;
            device.device.Regs.SerialNum = LoclTableRecieve.Regs.SerialNum;
            device.device.Regs.VerApp = LoclTableRecieve.Regs.VerApp;

            QByteArray aray = ui->lin_addit->text().toLocal8Bit();
            memset(LoclTableRecieve.Regs.mas,0,sizeof(LoclTableRecieve.Regs.mas));
            if(aray.length()<=static_cast<int>(sizeof(LoclTableRecieve.Regs.mas)))
            {
               memcpy(LoclTableRecieve.Regs.mas,aray,aray.length());
               memcpy(device.device.Regs.mas,aray,aray.length());
            }
            else
            {
               memcpy(LoclTableRecieve.Regs.mas,aray,sizeof(LoclTableRecieve.Regs.mas));
               memcpy(device.device.Regs.mas,aray,sizeof(LoclTableRecieve.Regs.mas));
            }

            queueAction.enqueue(UPDATE_ADDREGS);
            queueAction.enqueue(SEND_TO_SAVE_FACTORY);
    });


//    connect(ui->spin_CountPoint,QOverload<int>::of(&QSpinBox::valueChanged),this,&MWS::updateTableWidget);
    connect(ui->button_ChangePoints,&QPushButton::clicked,this,[=]
    {
           int countPoint =  ui->spin_CountPoint->value();
           updateTableWidget(countPoint);
           updatePlotWidget(0,0);
    });

    connect(ui->button_SendTable,&QPushButton::clicked,this,[=]
    {
        ModbusRegsTimer->stop();
        readTableWidget();
        currentPointTableCalibration = 0;
        queueAction.enqueue(WRITE_TABLE);
        ui->button_SendTable->setEnabled(false);
        ui->button_ReseiveTable->setEnabled(false);
        ModbusRegsTimer->start();
    });

   connect(ui->button_ReseiveTable,&QPushButton::clicked,this,[=]
   {
       ModbusRegsTimer->stop();
       TableCalibration.clear();
       queueAction.enqueue(READ_TABLE);
       ui->button_ReseiveTable->setEnabled(false);
       ui->button_SendTable->setEnabled(false);
       ModbusRegsTimer->start();
   });

   connect(ui->button_Accept,&QPushButton::clicked,this,[=]
   {
      ModbusRegsTimer->stop();
      updateAllSettingsTable(&LoclTableRecieve);

      device.device.Regs.idset = idUser.toUInt();
      LoclTableRecieve.Regs.idset = idUser.toUInt();

      QDateTime datechange = QDateTime::currentDateTime().toUTC();

      device.device.Regs.timechange = datechange.toTime_t();
      device.device.Regs.idchange  = idUser.toUInt();

      LoclTableRecieve.Regs.timechange = device.device.Regs.timechange;
      LoclTableRecieve.Regs.idchange = device.device.Regs.idchange;

      queueAction.enqueue(UPDATE_ADDREGS);
      queueAction.enqueue(SEND_TO_SAVE_CONFIG);
      ui->button_Accept->setEnabled(false);
      ui->button_Update->setEnabled(false);
      ModbusRegsTimer->start();
   });

   connect(ui->button_Update,&QPushButton::clicked,this,[=]
   {
       queueAction.enqueue(SEND_TO_UPDATE_CONFIG);
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

   barGraph = new QCPBars(ui->graph_distanse->xAxis, ui->graph_distanse->yAxis);
   ui->graph_table->clearGraphs();
   ui->graph_table->addGraph(); // наши точки
   ui->graph_table->addGraph(); // аппроксимация лагранжа
   ui->graph_table->addGraph(); // аппроксимация кусочно линейная
   ui->graph_table->addGraph(); // текущий обьем по Y
   ui->graph_table->addGraph(); // текущее расстояние по X
}




void MWS::logInit()
{
 if( !LogEnable )
 {
   LogEnable = true;

   poup->setPopupText(tr("Запись лог файла для устройства\r\n")+tr("Cерийный номер: ")+QString::number(device.device.Regs.SerialNum)+"\r\n"+tr("Имя: ")+device.devicename+"\r\n"+tr("Включена"));
   poup->show();

   LogFileTimer->start();
 }
 else
 {
   LogEnable =false;
   poup->setPopupText(tr("Запись лог файла для устройства\r\n")+tr("Cерийный номер: ")+QString::number(device.device.Regs.SerialNum)+"\r\n"+tr("Имя: ")+device.devicename+"\r\n"+tr("Выключена!"));
   poup->show();

   LogFileTimer->stop();
   LogFileXlsx.saveAs("Log"+QString::number(device.device.Regs.SerialNum)+".xlsx");
 }
}

void MWS::bootloadTimeout()
{
    BootTimeout->stop();
    ModbusRegsTimer->start();
}

void MWS::logSave()
{
    if(LogEnable )
    {
        if(countLine==1)
        {
            LogFileXlsx.write("A1", "Время");
            LogFileXlsx.write("B1", "Серийный номер");
            LogFileXlsx.write("C1", "Расстояние,мм");
            LogFileXlsx.write("D1", "Обьем");

            LogFileXlsx.write("E1", "Ошибка данных");
            LogFileXlsx.write("F1", "Ошибка обмена");
            LogFileXlsx.write("G1", "Ошибка насыщения");
            LogFileXlsx.write("H1", "Ошибка температуре");
            LogFileXlsx.write("I1", "Неизвестная ошибка");

        countLine = 2;
        } 
    LogFileXlsx.write("A"+QString::number(countLine), QDateTime::currentDateTimeUtc());
    LogFileXlsx.write("B"+QString::number(countLine), QString::number(device.device.Regs.SerialNum));
    LogFileXlsx.write("C"+QString::number(countLine), LoclTableRecieve.Regs.CurrentDistanse);
    LogFileXlsx.write("D"+QString::number(countLine), static_cast<float>(LoclTableRecieve.Regs.CurrentVolume)/1000);

    int errMissData = LoclTableRecieve.Regs.RegError & 0b000000001 ? 1 :0;
    int errSensCom = LoclTableRecieve.Regs.RegError & 0b000000010 ? 1 :0;
    int errDataSatur = LoclTableRecieve.Regs.RegError & 0b000000100 ? 1 :0;
    int errDataQuality = LoclTableRecieve.Regs.RegError & 0b000001000 ? 1 :0;
    int errUnknown = LoclTableRecieve.Regs.RegError & 0b100000000 ? 1 :0;

    LogFileXlsx.write("E"+QString::number(countLine), errMissData);
    LogFileXlsx.write("F"+QString::number(countLine), errSensCom);
    LogFileXlsx.write("G"+QString::number(countLine), errDataSatur);
    LogFileXlsx.write("H"+QString::number(countLine), errDataQuality);
    LogFileXlsx.write("I"+QString::number(countLine), errUnknown);

    countTimesave++;
    countLine++;
        if(countTimesave>=LOGFILE_SAVE_TIMER)
        {
            LogFileXlsx.saveAs("Log"+QString::number(device.device.Regs.SerialNum)+".xlsx");
        }
    }
}

void MWS::retranslate()
{
    ui->retranslateUi(this);
}

void MWS::setId(QString str)
{
    idUser = str;
}

MWS::~MWS()
{
    if(ModbusRegsTimer->isActive())
    ModbusRegsTimer->stop();

    static const char* const FILE_NAME = "lib.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window3 = ui->splitter_3->saveState();
    QByteArray window2 = ui->splitter_2->saveState();
    QByteArray window = ui->splitter->saveState();
    file.open( QIODevice::WriteOnly );
    if (file.isOpen())
    {
            stream << window3;
            stream << window2;
            stream << window;
        file.close();
    }
    barGraph->deleteLater();

    LogFileTimer->stop();
    LogFileXlsx.saveAs("Log"+QString::number(device.device.Regs.SerialNum)+".xlsx");

    delete LogXSLS;
    delete LogFileTimer;
    delete ModbusRegsTimer;
    delete ui;
}

void MWS::startView()
{
    ui->comb_TypeApproximation->addItems(QStringList() << tr("Кусочно линейный") << tr("Полином Лангранжа"));
    ui->comb_TypeAverage->addItems(QStringList() << tr("Экспоненциальный") << tr("Арифметический") << tr("МНК первого порядка") << tr("МНК второго порядка"));
    ui->comb_Noize->addItems(QStringList() << tr("Да") << tr("Нет"));
    ui->comb_DownSampFactor->addItems(QStringList() << "1" << "2"<< "4");
    ui->comb_MaximizeSignal->addItems(QStringList() << tr("Да") << tr("Нет"));
    ui->comb_PowerSaveMode->addItems(QStringList() << tr("Выключен") << tr("Сон")<< tr("Готовность")<< tr("Активный")<< tr("Гибернация"));
    ui->comb_AsynchMeasure->addItems(QStringList() << tr("Да") << tr("Нет"));
    ui->comb_Profile->addItems(QStringList() <<"1"<<"2"<<"3"<<"4"<<"5");
}


bool MWS::setSetting(QJsonObject json,QString idset,QString timeset)
{
  ModbusRegsTimer->stop();
  this->setEnabled(false);
  emit DevBusy(device);
  bool stat =false;

  if( !json.isEmpty() )
  {
        LoclTableRecieve.Regs.SensorId = json.value("SensorId").toInt();
        LoclTableRecieve.Regs.StartOfMeasure = json.value("StartOfMeasure").toVariant().toFloat();
        LoclTableRecieve.Regs.LenghtOfMeasure = json.value("LenghtOfMeasure").toVariant().toFloat();
        LoclTableRecieve.Regs.RepetitionMode = json.value("RepetitionMode").toVariant().toUInt();
        LoclTableRecieve.Regs.PowerSaveMode = json.value("PowerSaveMode").toVariant().toUInt();
        LoclTableRecieve.Regs.ReceiverGain = json.value("ReceiverGain").toVariant().toFloat();
        LoclTableRecieve.Regs.TXDisable = json.value("TXDisable").toVariant().toUInt();
        LoclTableRecieve.Regs.HWAAS = json.value("HWAAS").toInt();
        LoclTableRecieve.Regs.Profile = json.value("Profile").toVariant().toUInt();
        LoclTableRecieve.Regs.MaximizeSignal = json.value("MaximizeSignal").toVariant().toUInt();
        LoclTableRecieve.Regs.AsynchMeasure = json.value("AsynchMeasure").toVariant().toUInt();
        LoclTableRecieve.Regs.DownSampFactor = json.value("DownSampFactor").toInt();
        LoclTableRecieve.Regs.RunningAverage = json.value("RunningAverage").toVariant().toFloat();
        LoclTableRecieve.Regs.NoiseLevel = json.value("NoiseLevel").toVariant().toUInt();
        LoclTableRecieve.Regs.TypeApproxim = json.value("TypeApproxim").toVariant().toUInt();
        LoclTableRecieve.Regs.TypeAverag = json.value("TypeAverag").toVariant().toUInt();
        LoclTableRecieve.Regs.IntervalAverag = json.value("IntervalAverag").toVariant().toUInt();

        updateFirstSettingsView(&LoclTableRecieve);

        LoclTableRecieve.Regs.timechange = timeset.toULongLong();
        LoclTableRecieve.Regs.idset = idset.toUInt();

        device.device.Regs.timechange = timeset.toULongLong();
        device.device.Regs.idset = idset.toUInt();

        TableCalibration.clear();
        QJsonArray tableArray = json.value("Table").toArray();
        for ( int i = 0; i < tableArray.count(); i++) {
              QJsonObject subtree = tableArray.at(i).toObject();
              struct_pointTableCalibration point;
              point.pointDistanse =  subtree.value("Distance").toVariant().toUInt();
              point.pointVolume = subtree.value("Volume").toVariant().toFloat();
              TableCalibration << point;
        }
        writeTableWidget();
        updatePlotWidget(0,0);

        readTableWidget();
        currentPointTableCalibration = 0;

        queueAction.enqueue(WRITE_TABLE);
        queueAction.enqueue(UPDATE_ADDREGS);
        queueAction.enqueue(SEND_TO_SAVE_CONFIG);

        stat = true;
  }
  ModbusRegsTimer->start();
  return stat;
}

QJsonObject MWS::getSetting()
{
    QJsonObject json;
    json["SensorId"]= LoclTableRecieve.Regs.SensorId;
    json["StartOfMeasure"]= LoclTableRecieve.Regs.StartOfMeasure;
    json["LenghtOfMeasure"]= LoclTableRecieve.Regs.LenghtOfMeasure;
    json["RepetitionMode"]= LoclTableRecieve.Regs.RepetitionMode;
    json["PowerSaveMode"]= LoclTableRecieve.Regs.PowerSaveMode;
    json["ReceiverGain"]= LoclTableRecieve.Regs.ReceiverGain;
    json["TXDisable"]= LoclTableRecieve.Regs.TXDisable;
    json["HWAAS"]= LoclTableRecieve.Regs.HWAAS;
    json["Profile"]= LoclTableRecieve.Regs.Profile;
    json["MaximizeSignal"]= LoclTableRecieve.Regs.MaximizeSignal;
    json["AsynchMeasure"]= LoclTableRecieve.Regs.AsynchMeasure;
    json["DownSampFactor"]= LoclTableRecieve.Regs.DownSampFactor;
    json["RunningAverage"]= LoclTableRecieve.Regs.RunningAverage;
    json["NoiseLevel"]= LoclTableRecieve.Regs.NoiseLevel;

    json["TypeApproxim"]= LoclTableRecieve.Regs.TypeApproxim;
    json["TypeAverag"]= LoclTableRecieve.Regs.TypeAverag;
    json["IntervalAverag"]= LoclTableRecieve.Regs.IntervalAverag;

    QJsonArray tableArray;
    for ( int i=0;i<TableCalibration.count();i++)
    {
         QJsonObject pointTable;
         pointTable["Distance"] = TableCalibration[i].pointDistanse;
         pointTable["Volume"] = TableCalibration[i].pointVolume;
         tableArray.append(pointTable);
    }
    json["Table"] = tableArray;
    return json;
}

void MWS::getTable(struct_listSavedDevices table)
{
    device = table;

    startTime = QDateTime::currentDateTime().toUTC();

    ui->lin_idchange->setText(idUser);
    ui->dat_lastconnect->setDateTime(startTime);
    ui->lin_serial->setText(QString::number(device.device.Regs.SerialNum));
    ui->lin_typedev->setText(QString::number(device.device.Regs.TypeDevice));
    ui->lin_verapp->setText(QString::number(device.device.Regs.VerApp));
}

struct_listSavedDevices MWS::stringToTable(QString str)
{
     QStringList name = str.split(";");
     struct_listSavedDevices table;
     if( name.count()==4 )
     {
         QStringList tablstr  = name[3].split(",");
         table.devicename = name[0];
         table.portname = name[1];
         table.modbusadr = name[2];
         if( tablstr.count()==4)
         {
             table.device.Regs.LogError = tablstr[0].toUInt();
             table.device.Regs.SerialNum = tablstr[1].toUInt();
             table.device.Regs.TypeDevice = tablstr[2].toUShort();
             table.device.Regs.VerApp = tablstr[3].toUInt();
         }
     }
     return table;
}

void MWS::setToolTips()
{
   ui->lab_StartMeasure->setToolTip( tr("Расстояние от начала измерения. Рекомендуется ставить от 5 см для профиля 1, от 12 см для профилей 2-5 "));
   ui->lab_EndMeasure->setToolTip(tr("Диапазон измерений.Датчик измеряет амплитуды отраженных сигналов сенсора и преобразуя их в расстояние в диапазоне от начала измерения до начала измерения + расстояние установленное этим параметром"));
   ui->lab_ReceiverGain->setToolTip(tr("Коэффициент усиления приемника, используемый в датчике. Если усиление слишком низкое, объекты могут быть не видны или это может привести к плохому качеству сигнала из-за ошибок квантования. Если усиление слишком велико, сильные отражения могут привести к искажению данных. Рекомендуется не устанавливать усиление выше, чем необходимо, из соображений качества сигнала"));
   ui->lab_HWAAS->setToolTip(tr("Количество измерений на одну точку измерения амплитуды сигнала, по которому ведется усредненение уонечного результата. Время измерения пропорционально значению HWAAS. Не влияет на количество данных в развертке сигнала"));
   ui->lab_RunningAverage->setToolTip(tr("Чтобы стабилизировать сигнал и увеличить SNR, развертки в Envelope Service можно фильтровать по времени. Это можно настроить, установив коэффициент бегущего среднего от 0 до 1, где большое число указывает на усиление фильтрации. Фильтрация представляет собой стандартный фильтр экспоненциального сглаживания со значением по умолчанию 0,7."));
   ui->lab_Profile->setToolTip(tr("Основная конфигурация всех сервисов - это профили, пронумерованные от 1 до 5. Разница между профилями заключается в длине радиолокационного импульса и в способе дискретизации входящего импульса. Профили с низкими номерами используют короткие импульсы, в то время как более высокие профили используют более длинные импульсы \r\n  Профиль 1 рекомендуется для: \r\n - измерение сильных отражений \r\n  - работа на близком расстоянии (<20 см) \r\n Профиль 2 и 3 рекомендуется для: \r\n - работа на средних расстояниях (от 20 см до 1 м) \r\n Профиль 4 и 5 рекомендован для: \r\n - работа на больших расстояниях (> 1 м) "));
   ui->lab_Noize->setToolTip(tr("Настройка влияет на какие амплитуды сигналов в развертке будут использоваться, абсолютные или нормализованные относительно усиления датчика и аппаратного усреднения"));
   ui->lab_DownSampFactor->setToolTip(tr("Этот показатель влияет на частоту дискретизации сигнала, большее значение меньшее потребление энергии, памяти большая скорость работы, меньшее значение большая точность"));
   ui->lab_MaximizeSignal->setToolTip(tr("Настройка позволяет минимизировать насыщение приемника. Не рекомендуется использовать эту настройку при нормальной работе"));
   ui->lab_PowerSaveMode->setToolTip(tr("Конфигурация режима энергосбережения устанавливает, в каком состоянии датчик находится между измерениями. Режим потребления «АКТИВНЫЙ» дает самый быстрый отклик, а режим наименьшего потребления энергии «ВЫКЛ» дает самый медленный ответ. Режим «HIBERNATE» означает, что сенсор датчика все еще находится под напряжением, но внутренний генератор выключен"));
   ui->lab_AsynchMeasure->setToolTip(tr("Различие между режимами заключается в том, что в асинхронном режиме микроконтроллер датчика может работать, пока сенсор завершает работу. Поскольку датчик и хост могут работать параллельно, скорость обновления системы будет выше в асинхронном режиме. В асинхронном режиме данные полученые от сенсора являются данными предыдущей итерации"));
   ui->lab_IntAverage->setToolTip(tr("Значение относится к типу усреднения, определяет за какой промежуток времени будут усреднятся данные"));
   ui->lab_TypeAverage->setToolTip(tr("Определяет тип усреднения. Арифметическое усреднение работает на выбранном промежутке и следует из 2 измерений в секунду. Экспоненциальный работает с фильтром с адаптивным коэфициентом к скачам показаний сенсора. МНК - значения выбираются на апроксимирующей прямой линии в случае МНК первого порядка, в случае МНК второго порядка значения выбираются с кривой линии полинома второго пярядка A*x*x+B*x+C"));
   ui->lab_TypeApproximation->setToolTip(tr("Определяет каким образом будут расчитаны промежуточные значения между точками тарировочной таблицы"));


   MouseEvent->attach(ui->lab_StartMeasure);
   MouseEvent->attach(ui->lab_EndMeasure);
   MouseEvent->attach(ui->lab_ReceiverGain);
   MouseEvent->attach(ui->lab_HWAAS);
   MouseEvent->attach(ui->lab_RunningAverage);
   MouseEvent->attach(ui->lab_Profile);
   MouseEvent->attach(ui->lab_Noize);
   MouseEvent->attach(ui->lab_DownSampFactor);
   MouseEvent->attach(ui->lab_MaximizeSignal);
   MouseEvent->attach(ui->lab_PowerSaveMode);
   MouseEvent->attach(ui->lab_AsynchMeasure);

   MouseEvent->attach(ui->lab_IntAverage);
   MouseEvent->attach(ui->lab_TypeAverage);
   MouseEvent->attach(ui->lab_TypeApproximation);

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

        LoclTableRecieve.Regs.timeconnect = startTime.toTime_t();
        device.device.Regs.timeconnect = startTime.toTime_t();

        device.device.Regs.idchange = idUser.toUInt();
        LoclTableRecieve.Regs.idchange = idUser.toUInt();

        queueAction.enqueue(UPDATE_ADDREGS);
        queueAction.enqueue(SEND_DATA_CONNECT);
        queueAction.enqueue(READ_TABLE);
    }

    this->setEnabled(false);
    emit DevBusy(device);
}


void MWS::updateRegs(int startAdr, int countregs)
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,startAdr,countregs);
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

        }

    } else
    {
       ModbusRegsTimer->stop();
       emit DEVErrorString(tr("Утеряно соединение с портом!"));
       emit DevDisconnect(device);
    }
}

void MWS::replyReceivedRead()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTableRecieve.Adr[i+unit.startAddress()] = unit.value(i);
                }
                updateAllSettingsView(&LoclTableRecieve);
                addPointMeasure(LoclTableRecieve.Regs.CurrentDistanse,static_cast<double>(LoclTableRecieve.Regs.CurrentVolume)/1000);
                upperModbusCheck();

                if (firstRequest)
                {
                   updateFirstSettingsView(&LoclTableRecieve);
                   firstRequest=false;
                }

        } else {
            emit DEVErrorString(tr("Ошибка канала связи ")+"replyReceivedRead "+replyModbus->errorString());
            emit DevDisconnect(device);
            ModbusRegsTimer->stop();
        }
        replyModbus->deleteLater();
}

void MWS::sendRegs(int startAdr, int countregs)
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
            QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAdr, countregs);
            QModbusReply *reply = nullptr;
            upperModbusCheck();
            for (int i = 0, total = int(writeUnit.valueCount()); i < total; i++) {
                 writeUnit.setValue(i,LoclTableRecieve.Adr[i+writeUnit.startAddress()]);
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

        }
    } else
    {
       ModbusRegsTimer->stop();
       emit DEVErrorString(tr("Утеряно соединение с портом!"));
       emit DevDisconnect(device);
    }
}

void MWS::replyReceivedWrite()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
        } else {
            emit DEVErrorString(tr("Ошибка канала связи ")+ "replyReceivedWrite "+ replyModbus->errorString());
        }
        replyModbus->deleteLater();
}

MWS::stat_readwrite MWS::upperModbusCheck()
{
   stat_readwrite stat = NO;
       switch ( CurrentAction )
       {
       case NO_ACTION:
       {

       }break;
       case SEND_TO_SAVE_CONFIG:
       {
         stat = ActionSaveConfig();
       }break;
       case SEND_TO_UPDATE_CONFIG:
       {
         stat = ActionReadConfig();
       }break;
       case SEND_TO_CHECK_PASSWORD:
       {

       }break;
       case WRITE_TABLE:
       {
         stat = ActionWriteTable();
       }break;
       case READ_TABLE:
       {
         stat = ActionReadTable();
       }break;
       case SEND_TO_SAVE_FACTORY:
       {
         stat = ActionSaveFactory();
       }break;
       case SEND_DATA_CONNECT:
       {
         stat = ActionSaveDataConnect();
       }break;
       default:
       break;
       };
   return stat;
}

bool MWS::isCurrentAction()
{
  if(CurrentAction==NO_ACTION)
  return false;
  return true;
}

void MWS::setupAction(Action Action)
{
    CurrentAction = Action;
    CounterStepAction = 0;
}

MWS::stat_readwrite MWS::ActionSaveDataConnect()
{
    stat_readwrite stat = NO;
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_DATA_CONNECT;
        CounterStepAction = 1;
        stat = WRITE;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_DATA_CONNECT )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CounterStepAction = 0;
            CurrentAction = NO_ACTION;
            stat = NO;
            break;
        }
        stat = READ;
    }break;
    }
    return stat;
}


MWS::stat_readwrite MWS::ActionSaveFactory()
{
    stat_readwrite stat = NO;
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_SAVE_FACTORY;
        CounterStepAction = 1;
        stat = WRITE;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_SAVE_FACTORY )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CounterStepAction = 0;
            CurrentAction = NO_ACTION;
            stat = NO;
            break;
        }
        stat = READ;
    }break;
    }
    return stat;
}


MWS::stat_readwrite MWS::ActionSaveConfig()
{
    stat_readwrite stat = NO;
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_SAVE_CONFIG;
        CounterStepAction = 1;
        stat = WRITE;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CONFIG_SAVE )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CounterStepAction = 0;
            ui->button_Accept->setEnabled(true);
            ui->button_Update->setEnabled(true);
            CurrentAction = NO_ACTION;

            ui->dat_lastconnect->setDateTime(QDateTime::fromTime_t(device.device.Regs.timechange));

            emit DevSettingAccept(device,getSetting());

            stat = NO;
            break;
        }
        stat = READ;
    }break;
    }
    return stat;
}

MWS::stat_readwrite MWS::ActionReadConfig()
{
    stat_readwrite stat = NO;
    switch( CounterStepAction )
    {
    case 0:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_UPDATE_CONFIG;
        CounterStepAction = 1;
        stat = WRITE;
    }break;
    case 1:
    {
        if( LoclTableRecieve.Regs.RegStatus == STAT_CONFIG_UPDATE )
        {
            LoclTableRecieve.Regs.RegStatus = 0;
            CounterStepAction = 0;
            ModbusRegsTimer->stop();
            ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
            ModbusRegsTimer->start();
            ui->button_Accept->setEnabled(true);
            ui->button_Update->setEnabled(true);
            CurrentAction = NO_ACTION;
            stat =NO;
            break;
        }
        stat = READ;
    }break;
    }
    return stat;
}

MWS::stat_readwrite MWS::ActionReadTable()
{
    stat_readwrite stat =NO;
    switch( CounterStepAction )
    {
    case 0:
    {
      LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_POZITION;
      CounterStepAction = 1;
      stat = WRITE;
    }break;
    case 1:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_POZITION )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          CounterStepAction = 2;
      }
      stat = READ;
    }break;
    case 2:
    {
        LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_TRANSMIT;
        CounterStepAction = 3;
        stat = WRITE;
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
                CounterStepAction = 0;
                ModbusRegsTimer->stop();
                ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
                ModbusRegsTimer->start();
                writeTableWidget();
                ui->button_SendTable->setEnabled(true);
                ui->button_ReseiveTable->setEnabled(true);
                CurrentAction = NO_ACTION;
                stat =NO;
                break;
            }
        }
        stat = READ;
    }break;
    }
    return stat;
}

MWS::stat_readwrite MWS::ActionWriteTable()
{
    stat_readwrite stat = NO;
    switch( CounterStepAction )
    {
    case 0:
    {
      LoclTableRecieve.Regs.RegCommand = MODBUS_CMD_TABLE_CLEAR;
      CounterStepAction = 1;
      stat = WRITE;
    }break;
    case 1:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_CLEAR )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          CounterStepAction = 2;
      }
      stat = READ;
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
          stat = WRITE;
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
                  stat = WRITE;
                  break;
              }
              stat = READ;
          }
    }break;
    case 4:
    {
      if( LoclTableRecieve.Regs.RegStatus == STAT_CMD_TABLE_END )
      {
          LoclTableRecieve.Regs.RegStatus =0;
          currentPointTableCalibration = 0;
          CounterStepAction = 0;
          ModbusRegsTimer->stop();
          ModbusRegsTimer->setInterval(MODBUS_INTERVAL_ALL);
          ModbusRegsTimer->start();
          ui->button_SendTable->setEnabled(true);
          ui->button_ReseiveTable->setEnabled(true);
          CurrentAction = NO_ACTION;
          stat = NO;
          break;
      }
      stat = READ;
    }break;
    }
    return stat;
}

void MWS::updateTableWidget(int countPoints)
{
    QTableWidget *tableWidget = ui->tableWidget;
    tableWidget->setColumnCount(2);
    QTableWidgetItem *itemHead1 = new QTableWidgetItem(tr("Расстояние, мм"));
    QTableWidgetItem *itemHead2 = new QTableWidgetItem(tr("Обьем, л"));
    tableWidget->setHorizontalHeaderItem(0,itemHead1);
    tableWidget->setHorizontalHeaderItem(1,itemHead2);
    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(countPoints);

    if ( rowCount < countPoints )
    {
        for(int row=rowCount; row!=tableWidget->rowCount(); ++row){
            for(int column = 0; column!=tableWidget->columnCount(); ++column) {
                QTableWidgetItem *newItem = new QTableWidgetItem(tr("Введите число.."));
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
    QTableWidgetItem *itemHead1 = new QTableWidgetItem(tr("Расстояние, мм"));
    QTableWidgetItem *itemHead2 = new QTableWidgetItem(tr("Обьем, л"));
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

void MWS::addAmplitude(double distanse)
{

    std::rotate ( graph_amplitudeY.begin() , graph_amplitudeY.begin()+1, graph_amplitudeY.end() ) ;
    graph_amplitudeY[19] = distanse;

    //static QCPBars *barGraph = new QCPBars(ui->graph_distanse->xAxis, ui->graph_distanse->yAxis);
    barGraph->setWidth(15/(double)graph_amplitudeX.size());
    barGraph->setData(graph_amplitudeX, graph_amplitudeY);
    barGraph->setPen(Qt::NoPen);
    barGraph->setBrush(QColor(10, 140, 70, 160));



    ui->graph_distanse->xAxis->grid()->setSubGridVisible(false);
    ui->graph_distanse->yAxis->grid()->setSubGridVisible(false);

    ui->graph_distanse->xAxis->grid()->setVisible(false);
    ui->graph_distanse->yAxis->grid()->setVisible(false);

    ui->graph_distanse->xAxis->setVisible(false);

    ui->graph_distanse->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_distanse->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);


    ui->graph_distanse->rescaleAxes();
    ui->graph_distanse->replot();
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

    ui->graph_table->rescaleAxes();
    ui->graph_table->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_table->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(200, 200, 200));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->graph_table->setBackground(plotGradient);

    ui->graph_table->xAxis->grid()->setVisible(false);
    ui->graph_table->yAxis->grid()->setVisible(false);

    ui->graph_table->replot();
}

void MWS::addPointMeasure(double distatanse,double volume)
{
    addMeasure(distatanse,volume);
    addAmplitude(distatanse);
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
    ui->graph_table->graph(0)->setPen(QPen(QColor(120, 120, 120), 2));
    ui->graph_table->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->graph_table->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));

    //Подписываем оси Ox и Oy
    ui->graph_table->xAxis->setLabel(tr("Расстояние"));
    ui->graph_table->yAxis->setLabel(tr("Обьем"));

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

    ui->graph_table->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_table->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->graph_table->replot();
}


void MWS::updateFirstSettingsView(union_tableRegsWrite* Table)
{

ui->spin_AdrModbus->setValue(Table->Regs.AdrModbus);
ui->spin_IntAverage->setValue(Table->Regs.IntervalAverag);

ui->comb_TypeApproximation->setCurrentIndex(Table->Regs.TypeApproxim);
ui->comb_TypeAverage->setCurrentIndex(Table->Regs.TypeAverag);

 ui->spin_EndMeasure->setValue(static_cast<int>(Table->Regs.LenghtOfMeasure*1000));
 ui->spin_HWAAS->setValue(Table->Regs.HWAAS);
 ui->spin_IntAverage->setValue(Table->Regs.IntervalAverag);
 ui->spin_ReceiverGain->setValue(Table->Regs.ReceiverGain);
 ui->spin_RunningAverage->setValue(Table->Regs.RunningAverage);
 ui->spin_StartMeasure->setValue(static_cast<int>(Table->Regs.StartOfMeasure*1000));

 ui->comb_AsynchMeasure->setCurrentIndex(Table->Regs.AsynchMeasure);
 ui->comb_DownSampFactor->setCurrentIndex(Table->Regs.DownSampFactor);
 ui->comb_MaximizeSignal->setCurrentIndex(Table->Regs.MaximizeSignal);
 ui->comb_Noize->setCurrentIndex(Table->Regs.NoiseLevel);
 ui->comb_PowerSaveMode->setCurrentIndex(Table->Regs.PowerSaveMode);
 ui->comb_Profile->setCurrentIndex(Table->Regs.Profile);
 ui->comb_TypeApproximation->setCurrentIndex(Table->Regs.TypeApproxim);
 ui->comb_TypeAverage->setCurrentIndex(Table->Regs.TypeAverag);

 ui->lin_koefA->setText(QString::number(Table->Regs.logA));
 ui->lin_koefB->setText(QString::number(Table->Regs.logB));

 ui->check_res_CAN->setChecked(Table->Regs.Reslift & 0x0f00 ? true :false);
 ui->check_res_RS485->setChecked(Table->Regs.Reslift & 0x000f ? true :false);
 ui->check_pow_CAN->setChecked(Table->Regs.Reslift & 0xf000 ? true :false);
 ui->check_pow_RS485->setChecked(Table->Regs.Reslift & 0x00f0 ? true :false);
}


void MWS::updateAllSettingsView(union_tableRegsWrite* Table)
{
     ui->lcd_Distance->setText(QString::number(Table->Regs.CurrentDistanse));
     ui->lcd_Volume->setText(QString::number(static_cast<double>(Table->Regs.CurrentVolume)/1000));

     float koef = ui->lab_curKoefA->text().toFloat() + (ui->lab_curKoefB->text().toFloat())*log(Table->Regs.CurrentDistanse);
     ui->lcd_uncalibrDistanse->setText(QString::number(static_cast<uint16_t>(Table->Regs.CurrentDistanse/koef)));

     QString errstr;
     QString errFlag;
     QString errTrue  = tr("присутствует");
     QString errFalse = tr("отсутствует");

     QString errMissData = Table->Regs.RegError & 0b000000001 ? errTrue : errFalse;
     QString errSensCom = Table->Regs.RegError & 0b000000010 ? errTrue : errFalse;
     QString errDataSatur = Table->Regs.RegError & 0b000000100 ? errTrue : errFalse;
     QString errDataQuality = Table->Regs.RegError & 0b000001000 ? errTrue : errFalse;
     QString errUnknown = Table->Regs.RegError & 0b100000000 ? errTrue : errFalse;

     errstr.append(tr("Ошибка данных: ")+ "\r\n");
     errstr.append(tr("Ошибка обмена: ")+"\r\n");
     errstr.append(tr("Ошибка насыщения: ")+"\r\n");
     errstr.append(tr("Ошибка по температуре: ")+"\r\n");
     errstr.append(tr("Неизвестная ошибка: ")+"\r\n");

     errFlag.append(errMissData+"\r\n");
     errFlag.append(errSensCom+"\r\n");
     errFlag.append(errDataSatur+"\r\n");
     errFlag.append(errDataQuality+"\r\n");
     errFlag.append(errUnknown+"\r\n");

     ui->lab_error->setText(errstr);
     ui->lab_errFlag->setText(errFlag);

     ui->lab_AdrModbusCur->setText(QString::number(Table->Regs.AdrModbus));
     ui->lab_IntAverageCur->setText(QString::number(Table->Regs.IntervalAverag));

     ui->lab_TypeApproximationCur->setText(ui->comb_TypeApproximation->itemText(Table->Regs.TypeApproxim));
     ui->lab_TypeAverageCur->setText(ui->comb_TypeAverage->itemText(Table->Regs.TypeAverag));

     ui->lab_EndMeasureCur->setText(QString::number(static_cast<int>(Table->Regs.LenghtOfMeasure*1000)));
     ui->lab_HWAASCur->setText(QString::number(Table->Regs.HWAAS));
     ui->lab_ReceiverGainCur->setText(QString::number(Table->Regs.ReceiverGain));
     ui->lab_RunningAverageCur->setText(QString::number(Table->Regs.RunningAverage));
     ui->lab_StartMeasureCur->setText(QString::number(static_cast<int>(Table->Regs.StartOfMeasure*1000)));

     ui->lab_AsynchMeasureCur->setText(ui->comb_AsynchMeasure->itemText(Table->Regs.AsynchMeasure));
     ui->lab_DownSampFactorCur->setText(ui->comb_DownSampFactor->itemText(Table->Regs.DownSampFactor));
     ui->lab_MaximizeSignalCur->setText(ui->comb_MaximizeSignal->itemText(Table->Regs.MaximizeSignal));
     ui->lab_NoizeCur->setText(ui->comb_Noize->itemText(Table->Regs.NoiseLevel));
     ui->lab_PowerSaveModeCur->setText(ui->comb_PowerSaveMode->itemText(Table->Regs.PowerSaveMode));
     ui->lab_ProfileCur->setText(ui->comb_Profile->itemText(Table->Regs.Profile));

     ui->lab_curKoefA->setText(QString::number(Table->Regs.logA));
     ui->lab_curKoefB->setText(QString::number(Table->Regs.logB));

     ui->check_res_CANCur->setText(Table->Regs.Reslift & 0x0f00 ? "1":"0");
     ui->check_res_RS485Cur->setText(Table->Regs.Reslift & 0x000f ? "1":"0");
     ui->check_pow_CANCur->setText(Table->Regs.Reslift & 0xf000 ? "1":"0");
     ui->check_pow_RS485Cur->setText(Table->Regs.Reslift & 0x00f0 ? "1":"0");
}

bool MWS::updateAllSettingsTable(union_tableRegsWrite *Table)
{
     bool stat = false;
     static union_tableRegsWrite TableCheck;
     Table->Regs.AdrModbus = ui->spin_AdrModbus->value();
     Table->Regs.LenghtOfMeasure = static_cast<float>(ui->spin_EndMeasure->value())/1000;
     Table->Regs.HWAAS = ui->spin_HWAAS->value();

     Table->Regs.SensorId = 1;
     Table->Regs.IntervalAverag = ui->spin_IntAverage->value();
     Table->Regs.ReceiverGain = ui->spin_ReceiverGain->value();
     Table->Regs.RunningAverage = ui->spin_RunningAverage->value();
     Table->Regs.StartOfMeasure = static_cast<float>(ui->spin_StartMeasure->value())/1000;

     Table->Regs.TypeApproxim = ui->comb_TypeApproximation->currentIndex();
     Table->Regs.TypeAverag = ui->comb_TypeAverage->currentIndex();

     Table->Regs.AsynchMeasure  = ui->comb_AsynchMeasure->currentIndex();
     Table->Regs.DownSampFactor =  ui->comb_DownSampFactor->currentIndex();
     Table->Regs.MaximizeSignal = ui->comb_MaximizeSignal->currentIndex();
     Table->Regs.NoiseLevel = ui->comb_Noize->currentIndex();
     Table->Regs.PowerSaveMode = ui->comb_PowerSaveMode->currentIndex();
     Table->Regs.Profile = ui->comb_Profile->currentIndex();
     Table->Regs.TXDisable = 0;

     Table->Regs.logA = ui->lin_koefA->text().toFloat();
     Table->Regs.logB = ui->lin_koefB->text().toFloat();

     ui->check_res_CAN->isChecked() ? Table->Regs.Reslift|= 0x0100 : Table->Regs.Reslift&= 0x1011;
     ui->check_res_RS485->isChecked() ? Table->Regs.Reslift|= 0x0001 : Table->Regs.Reslift&= 0x1110;
     ui->check_pow_CAN->isChecked() ? Table->Regs.Reslift|= 0x1000 : Table->Regs.Reslift&= 0x0111;
     ui->check_pow_RS485->isChecked() ? Table->Regs.Reslift|= 0x0010 : Table->Regs.Reslift&= 0x1101;

     if( memcmp(&TableCheck.Adr[10],&(Table->Adr[10]),(COUNT_SENSOR_REGS+COUNT_DEV_REGS)*2))
     {
         memcpy(&TableCheck.Adr[10],&(Table->Adr[10]),(COUNT_SENSOR_REGS+COUNT_DEV_REGS)*2);
         stat = true;
     }
     return stat;
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

