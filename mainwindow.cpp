#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QPushButton>

#include "structs_ui.h"
#include "structs_main.h"

#include <QLineEdit>
#include <QLabel>

#include <QFile>

#include <QTimer>
#include <QTime>

#include <QMessageBox>
#include <QGridLayout>

#include <QLibrary>

#include "libtype4.h"
#include "mws.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ModbusTimer = new QTimer;
    libs = new DeviceLibs;
    connect(ui->add,&QAction::triggered,this,&MainWindow::libsAdd);
    connect(ui->view,&QAction::triggered,this,&MainWindow::LibsView);
    connect(ui->searh,&QAction::triggered,this,&MainWindow::DevicesSearch);
    connect(ui->saved,&QAction::triggered,this,&MainWindow::DevicesSaved);

//    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error)
//    {
//        ui->textBrowser->append(modbusDevice->errorString());
//    });

//    connect(modbusDevice, &QModbusClient::stateChanged,this,[this](QModbusDevice::State)
//    {
//        if(modbusDevice->state() == QModbusDevice::UnconnectedState)
//        {
//         ui->textBrowser->append("Modbus Disconnect");
//        }
//        if(modbusDevice->state() == QModbusDevice::ConnectedState)
//         ui->textBrowser->append("Modbus Connect");
//    });
    ui->treeWidget->setHeaderLabel(" ");
   // ui->treeWidget->headerItem()->setHidden(true);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);
    ui->saved->setDisabled(true);

//    setStyleSheet("QTreeWidget{"
//    //"background-color: red;"
//    "border-style: solid;"
//    "border-width: 1px;"
//    "border-radius: 20px;"
//    //"border-color: beige;"
//    //"font: bold 14px;"
//    "min-width: 10em;"
//    "padding: 5px;"
//    "}"
//    );
//    ui->treeWidget->setWindowOpacity(1);

}

MainWindow::~MainWindow()
{
   // delete libs;
    delete ModbusTimer;
    delete ui;
}

void MainWindow::libsAdd()
{

}

void MainWindow::LibsView()
{

}

void MainWindow::DevicesSearch()
{
 ui->textBrowser->clear();
 ui->treeWidget->clear();

 ui->searh->setText("Идет поиск...");
 ui->searh->setDisabled(true);

 libs->CloseAll();

 tablListSavedDevices.clear();
 strListSavedDevices.clear();
 DevicesRead();

 const auto infos = QSerialPortInfo::availablePorts();
 QString avlPorts;
 for (const QSerialPortInfo &info : infos) {
      avlPorts +=info.portName();
      avlPorts +=";";
 };
 if ( infos.count()>0  )
 {
     ui->treeWidget->setHeaderLabel("Найденные устройства...");
     ui->textBrowser->append("Найденые порты: "+avlPorts);
 } else
 {
     ui->treeWidget->setHeaderLabel(" ");
     ui->textBrowser->append("Портов не найдено..."+avlPorts);
 }
 endcomModBusDevice = infos.count();
 intcomModBusDevice = 0;

 ModbusTimer->stop();
 ModbusTimer->setSingleShot(true);
 ModbusTimer->setInterval(infos.count()*(LAST_MODBUS_ADRESS*MODBUS_TIMEOUT_REPLY*(MODBUS_COUNT_REPEAT+1))*2.5);
 connect(ModbusTimer,&QTimer::timeout,this,[this]()
         {
              QString comname = vectorModbusDevice[intcomModBusDevice].nameCom;
//              vectorModbusDevice.first().modbusDev->disconnectDevice();
//              vectorModbusDevice.first().modbusDev->deleteLater();
              QMessageBox::warning(this, comname,"Проверьте соединение с портом");
//              vectorModbusDevice.clear();
              ui->searh->setEnabled(true);
         });
 ModbusTimer->start();

 searchModbusDevice(infos);
 pollModbus();
}

void MainWindow::searchModbusDevice(QList<QSerialPortInfo> listport)
{
    for( int i=0;i<vectorModbusDevice.count();i++)
    {
        vectorModbusDevice[i].modbusDev->disconnectDevice();
        vectorModbusDevice[i].modbusDev->deleteLater();
    }
    vectorModbusDevice.clear();

    for (const QSerialPortInfo &info : listport) {
           struct_ComModbus newCh;
           newCh.modbusDev = new QModbusRtuSerialMaster();
           newCh.nameCom =   info.portName();
           newCh.currentAdr = 0;
           newCh.description = info.description();
           newCh.manufacturer = info.manufacturer();
           newCh.productIdentifier = info.productIdentifier();
           newCh.vendorIdentifier = info.vendorIdentifier();
           newCh.isFind =false;
           vectorModbusDevice << newCh;
    }
}

void MainWindow::pollModbus()
{

    if(intcomModBusDevice<vectorModbusDevice.count())
    {
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialPortNameParameter,vectorModbusDevice[intcomModBusDevice].nameCom);
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud115200);
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);

         vectorModbusDevice[intcomModBusDevice].modbusDev->setTimeout(MODBUS_TIMEOUT_REPLY);
         vectorModbusDevice[intcomModBusDevice].modbusDev->setNumberOfRetries(MODBUS_COUNT_REPEAT);


         if ( !vectorModbusDevice[intcomModBusDevice].modbusDev->connectDevice() )
         {
              ui->textBrowser->append(tr("Соединение с портом ") + vectorModbusDevice[intcomModBusDevice].nameCom +" не установленно:" + vectorModbusDevice[intcomModBusDevice].modbusDev->errorString());


              vectorModbusDevice[intcomModBusDevice].modbusDev->disconnectDevice();
              vectorModbusDevice[intcomModBusDevice].modbusDev->deleteLater();

              vectorModbusDevice.remove(intcomModBusDevice);

              pollModbus();
         } else
         {
              ui->textBrowser->append(tr("Соединение с портом ") + vectorModbusDevice[intcomModBusDevice].nameCom +" установленно...");
              pollAdrModbus();
         }
    }else
    {
      ui->searh->setText("Поиск устройств");
      ui->searh->setEnabled(true);
      ModbusTimer->stop();
    }
}

void MainWindow::pollAdrModbus()
{

    if ( vectorModbusDevice[intcomModBusDevice].currentAdr !=LAST_MODBUS_ADRESS+1)
    {
        QModbusDataUnit request;
        request.setRegisterType(QModbusDataUnit::InputRegisters);
        request.setStartAddress(0);
        request.setValueCount(8);
        //vectorModbusDevice.first().currentAdr++;
        vectorModbusDevice[intcomModBusDevice].currentAdr++;

        if (auto *reply =  vectorModbusDevice[intcomModBusDevice].modbusDev->sendReadRequest(request,  vectorModbusDevice[intcomModBusDevice].currentAdr)) {
            if (!reply->isFinished())
            {                
                connect(reply, &QModbusReply::finished, this, &MainWindow::pollReplyModbus);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
            ui->textBrowser->append("Read error: " + vectorModbusDevice[intcomModBusDevice].modbusDev->errorString());
        }
    }
    else
    {
          if ( !vectorModbusDevice[intcomModBusDevice].isFind)
          {
                vectorModbusDevice[intcomModBusDevice].modbusDev->disconnectDevice();
                vectorModbusDevice[intcomModBusDevice].modbusDev->deleteLater();
                vectorModbusDevice.remove(intcomModBusDevice);
          } else
          {
              intcomModBusDevice++;
          }
        pollModbus();
    }
}

void MainWindow::pollReplyModbus()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;

        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();
            if ( int(unit.valueCount()) == sizeof(struct_tableRegsRead)/2 )
            {
                ui->textBrowser->append("Найдено устройство; Протокол: MODBUS-rtu; Адрес: "+ QString::number(replyModbus->serverAddress()));
                union_tableRegsRead LoclTable;
                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTable.Adr[i] = unit.value(i);
                }
                QString findname = findNameDevice(LoclTable);
                getDeviceModbus(LoclTable,vectorModbusDevice[intcomModBusDevice],findname);
                vectorModbusDevice[intcomModBusDevice].isFind = true;
            }
        } else {
            if ( replyModbus->serverAddress() == LAST_MODBUS_ADRESS )
            {
                 ui->textBrowser->append("Закончен опрос порта: "+ vectorModbusDevice[intcomModBusDevice].nameCom);
            }
        }
        replyModbus->deleteLater();
        pollAdrModbus();
}


QString MainWindow::findNameDevice(union_tableRegsRead table)
{
    QString name = NULL;
    for ( int i=0;i<tablListSavedDevices.count();i++ )
    {
          if ( (tablListSavedDevices[i].device.Regs.SerialNum == table.Regs.SerialNum) &&

               (tablListSavedDevices[i].device.Regs.TypeDevice == table.Regs.TypeDevice) &&

               (tablListSavedDevices[i].device.Regs.VerApp == table.Regs.VerApp)  )
          {
                name = tablListSavedDevices[i].devicename;
                break;
          }
    }
    return name;
}

void MainWindow::setNameDevice(struct_tableRegsRead table,QString name)
{
    bool nameBool = false;
    for ( int i=0;i<tablListSavedDevices.count();i++ )
    {
          if ( (tablListSavedDevices[i].device.Regs.SerialNum == table.SerialNum) &&

               (tablListSavedDevices[i].device.Regs.TypeDevice == table.TypeDevice) &&

               (tablListSavedDevices[i].device.Regs.VerApp == table.VerApp)  )
          {
                tablListSavedDevices[i].devicename = name;
                nameBool  = true;
                break;
          }
    }
    if ( !nameBool )
    {
        struct_listSavedDevices newDev;
        memcpy(&newDev.device,&table,sizeof(newDev.device));
        newDev.devicename = name;
        tablListSavedDevices << newDev;
    }
}

void MainWindow::getDeviceModbus(union_tableRegsRead table, struct_ComModbus com,QString nameconnect)
{
   disconnect(ui->treeWidget,&QTreeWidget::itemChanged,this,&MainWindow::treeItemChange);

   struct_listSavedDevices locl;
   memcpy(&locl.device,&table.Regs,sizeof(locl.device));
   locl.devicename = nameconnect;
   locl.portname = com.nameCom;
   locl.modbusadr = QString::number(com.currentAdr);
   QString strRole = tableToString(locl);

   QString str ="№ номер: "+ QString::number(table.Regs.SerialNum);
   QTreeWidgetItem *toplevel = new QTreeWidgetItem(ui->treeWidget);

   toplevel->setText(0,str);
   toplevel->setData(0,Qt::UserRole,strRole);

   QTreeWidgetItem *itemName=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemType=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemSerial=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemApp=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemDate=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemLog=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *Protocol=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *FisicalPort=new QTreeWidgetItem(toplevel);

   itemName->setText(0,"Имя: "+nameconnect);
   itemName->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
   //itemName->setData(0, Qt::UserRole, strRole);

   itemType->setText(0,"Тип: "+QString::number(table.Regs.TypeDevice));
   itemSerial->setText(0,"Серийный №: "+QString::number(table.Regs.SerialNum));
   itemApp->setText(0,"Аппаратная версия: "+QString::number(table.Regs.VerApp));
   itemDate->setText(0,"Дата связи: "+QString::number(table.Regs.LastDate));
   itemLog->setText(0,"Лог ошибок: "+QString::number(table.Regs.LogError));

   Protocol->setText(0,"Протокол");
   QTreeWidgetItem *protc1=new QTreeWidgetItem(Protocol);
   QTreeWidgetItem *protc2=new QTreeWidgetItem(Protocol);
   protc1->setText(0,"Тип: MODBUS");
   protc2->setText(0,"Адрес: "+QString::number(com.currentAdr));

   FisicalPort->setText(0,"Порт/канал связи: "+com.nameCom);
   QTreeWidgetItem *comp1=new QTreeWidgetItem(FisicalPort);
   QTreeWidgetItem *comp2=new QTreeWidgetItem(FisicalPort);
   QTreeWidgetItem *comp3=new QTreeWidgetItem(FisicalPort);
   QTreeWidgetItem *comp4=new QTreeWidgetItem(FisicalPort);
   comp1->setText(0,"Описание: " + com.description);
   comp2->setText(0,"Производитель: " + com.manufacturer);
   comp3->setText(0,"ProdID: " + QString::number(com.productIdentifier));
   comp4->setText(0,"VendID: " + QString::number(com.vendorIdentifier));

   connect(ui->treeWidget,&QTreeWidget::itemChanged,this,&MainWindow::treeItemChange);
}

void MainWindow::treeItemChange(QTreeWidgetItem * item, int column)
{
 if( item->parent()!=NULL)
 {
    auto strloc = item->parent()->data(column,Qt::UserRole);
    if (strloc.isValid())
    {
     if ( strloc.type() == QVariant::String)
     {
          QString str = QString("%1").arg(strloc.toString());
          struct_listSavedDevices table = stringToTable(str);
         // QString itemStr  = item->text(column).replace(" ","");
          QStringList newName = item->text(column).replace(" ","").split(":",Qt::SkipEmptyParts);
          if ( newName.count()==1 )
          {
               table.devicename = newName[0];
          }
          else
          {
               table.devicename = newName[1];
          }
          setNameDevice(table.device.Regs,table.devicename);
          ui->treeWidget->setHeaderLabel("Найденные устройства ( не сохранены )");
          ui->saved->setEnabled(true);
     }
    }
  }
}


QString MainWindow::tableToString(struct_listSavedDevices table_point)
{
  QString str = table_point.devicename +";"+table_point.portname +";"+table_point.modbusadr +";"
                         +QString::number(table_point.device.Regs.LastDate) +","
                         +QString::number(table_point.device.Regs.LogError) +","
                         +QString::number(table_point.device.Regs.SerialNum) +","
                         +QString::number(table_point.device.Regs.TypeDevice) +","
                         +QString::number(table_point.device.Regs.VerApp);
  return str;
}

struct_listSavedDevices MainWindow::stringToTable(QString str)
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

void MainWindow::strListTotableList()
{
     tablListSavedDevices.clear();
     for ( int i=0;i<strListSavedDevices.count();i++)
     {
           tablListSavedDevices <<  stringToTable(strListSavedDevices[i]);
     }
     strListSavedDevices.clear();
}

void MainWindow::tableListTostrList()
{
     for (int i=0;i<tablListSavedDevices.count();i++)
     {
          strListSavedDevices << tableToString(tablListSavedDevices[i]);
     }
}

void MainWindow::DevicesSaved()
{
    static const char* const FILE_NAME = "devices.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );

    tableListTostrList();

    file.open( QIODevice::WriteOnly );
    if (file.isOpen())
    {
        stream << QList<QString> (strListSavedDevices);
        file.close();
        ui->treeWidget->setHeaderLabel("Найденные устройства...");
        ui->saved->setDisabled(true);
    }
}

void MainWindow::DevicesRead()
{
    static const char* const FILE_NAME = "devices.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );

    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
        stream >> strListSavedDevices;
        file.close();
        strListTotableList();
    }
}

void MainWindow::prepareMenu( const QPoint & pos )
{
QTreeWidget *tree = ui->treeWidget;
QTreeWidgetItem *item = tree->itemAt( pos );
    if ( item!=NULL )
    {
       auto strloc = item->data(0,Qt::UserRole);
       if (strloc.isValid())
       {
        if ( strloc.type() == QVariant::String )
        {
            QAction *newconnect = new QAction("Открыть");
            QAction *newsave = new QAction("Сохранить");
            QAction *newSetting = new QAction("Свойства");
            QMenu menu(this);
            newconnect->setData(strloc);
            connect(newconnect,&QAction::triggered,this,&MainWindow::LoadLibDevice);

            menu.addAction(newconnect);
            menu.addAction(newsave);
            menu.addSection("new");
            menu.addAction(newSetting);
            menu.exec( tree->mapToGlobal(pos) );        
        }
       }
    }
}

void MainWindow::LoadLibDevice()
{
     QAction* open = qobject_cast< QAction* >( sender() );
     auto data = open->data();
     if ( data.isValid() )
     {
         if ( data.type()==QVariant::String )
         {
              QString str = QString("%1").arg(data.toString());
              struct_listSavedDevices table = stringToTable(str);
              QModbusClient *modbus =nullptr;
              for(int i =0; i < vectorModbusDevice.count();i++)
              {
                  if( vectorModbusDevice[i].nameCom == table.portname)
                  {
                      modbus  = vectorModbusDevice[i].modbusDev;
                  }
              }
              if(modbus!=nullptr)
              {

                if ( libs->LibOpen(str,ui->mdiArea,modbus) )
                {

                }
              }
         }
     }
}

