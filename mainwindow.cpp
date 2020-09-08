#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>

#include <QLineEdit>
#include <QLabel>

#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}

MainWindow::~MainWindow()
{
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
 ui->treeWidget->setHeaderLabel("Найденные устройства...");

 tablListSavedDevices.clear();
 strListSavedDevices.clear();
 DevicesRead();

 const auto infos = QSerialPortInfo::availablePorts();
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
           vectorModbusDevice << newCh;
    }
}

void MainWindow::pollModbus()
{
    if ( vectorModbusDevice.count()!=0 )
    {
         vectorModbusDevice.first().modbusDev->setConnectionParameter(QModbusDevice::SerialPortNameParameter,vectorModbusDevice.first().nameCom);
         vectorModbusDevice.first().modbusDev->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
         vectorModbusDevice.first().modbusDev->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud115200);
         vectorModbusDevice.first().modbusDev->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
         vectorModbusDevice.first().modbusDev->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);

         vectorModbusDevice.first().modbusDev->setTimeout(10);
         vectorModbusDevice.first().modbusDev->setNumberOfRetries(2);

         if ( !vectorModbusDevice.first().modbusDev->connectDevice() )
         {
              ui->textBrowser->append(tr("Соединение с портом ") + vectorModbusDevice.first().nameCom +" не установленно:" + vectorModbusDevice.first().modbusDev->errorString());

              vectorModbusDevice.first().modbusDev->disconnectDevice();
              vectorModbusDevice.first().modbusDev->deleteLater();
              QMutableVectorIterator<struct_ComModbus> i(vectorModbusDevice);
              if ( i.hasNext())
              {
                   i.next();
                   i.remove();
              }
              pollModbus();
         } else
         {
              ui->textBrowser->append(tr("Соединение с портом ") + vectorModbusDevice.first().nameCom +" установленно...");
              pollAdrModbus();
         }
    }
}

void MainWindow::pollAdrModbus()
{
    if ( vectorModbusDevice.first().currentAdr != LAST_MODBUS_ADRESS )
    {
        QModbusDataUnit request;
        request.setRegisterType(QModbusDataUnit::InputRegisters);
        request.setStartAddress(0);
        request.setValueCount(8);
        vectorModbusDevice.first().currentAdr++;

        if (auto *reply =  vectorModbusDevice.first().modbusDev->sendReadRequest(request,  vectorModbusDevice.first().currentAdr)) {
            if (!reply->isFinished())
            {                
                connect(reply, &QModbusReply::finished, this, &MainWindow::pollReplyModbus);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
            ui->textBrowser->append("Read error: " + vectorModbusDevice.first().modbusDev->errorString());
        }
    }
    else
    {
        vectorModbusDevice.first().modbusDev->disconnectDevice();
        vectorModbusDevice.first().modbusDev->deleteLater();
        QMutableVectorIterator<struct_ComModbus> i(vectorModbusDevice);
        if ( i.hasNext())
        {
             i.next();
             i.remove();
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
                getDeviceModbus(LoclTable,vectorModbusDevice.first(),findname);
                //setNameDevice(LoclTable,"new");
            }
        } else if (replyModbus->error() == QModbusDevice::ProtocolError) {
            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                        arg(replyModbus->errorString()).
                                        arg(replyModbus->rawResult().exceptionCode(), -1, 16), 5000);
        } else {
            if ( replyModbus->serverAddress() == LAST_MODBUS_ADRESS-1 )
            {
                 ui->textBrowser->append("Закончен опрос порта: "+ vectorModbusDevice.first().nameCom);
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
          if ( (tablListSavedDevices[i].device.SerialNum == table.Regs.SerialNum) &&

               (tablListSavedDevices[i].device.TypeDevice == table.Regs.TypeDevice) &&

               (tablListSavedDevices[i].device.VerApp == table.Regs.VerApp)  )
          {
                name = tablListSavedDevices[i].name;
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
          if ( (tablListSavedDevices[i].device.SerialNum == table.SerialNum) &&

               (tablListSavedDevices[i].device.TypeDevice == table.TypeDevice) &&

               (tablListSavedDevices[i].device.VerApp == table.VerApp)  )
          {
                tablListSavedDevices[i].name = name;
                nameBool  = true;
                break;
          }
    }
    if ( !nameBool )
    {
        struct_listSavedDevices newDev;
        memcpy(&newDev.device,&table,sizeof(newDev.device));
        newDev.name = name;
        tablListSavedDevices << newDev;
    }
}

void MainWindow::getDeviceModbus(union_tableRegsRead table, struct_ComModbus com,QString nameconnect)
{
   disconnect(ui->treeWidget,&QTreeWidget::itemChanged,this,&MainWindow::treeDoubleClick);

   QString str =" Серийный номер: "+ QString::number(table.Regs.SerialNum);
   QTreeWidgetItem *toplevel = new QTreeWidgetItem(ui->treeWidget);
   toplevel->setText(0,str);
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
   struct_listSavedDevices locl;
   memcpy(&locl.device,&table.Regs,sizeof(locl.device));
   locl.name = nameconnect;
   QString strRole = tableToString(locl);
   itemName->setData(0, Qt::UserRole, strRole);

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

   connect(ui->treeWidget,&QTreeWidget::itemChanged,this,&MainWindow::treeDoubleClick);
}

void MainWindow::treeDoubleClick(QTreeWidgetItem * item, int column)
{
 auto strloc = item->data(column,Qt::UserRole);
 if (strloc.isValid())
 {
     if ( strloc.type() == QVariant::String)
     {
          QString str = QString("%1").arg(strloc.toString());
          struct_listSavedDevices table = stringToTable(str);
          QStringList newName = item->text(column).split(":",Qt::SkipEmptyParts);
          if (newName.count()==1)
          {
             table.name = newName[0];
          }
          else
          {
             table.name = newName[1];
          }
          setNameDevice(table.device,table.name);
          ui->treeWidget->setHeaderLabel("Найденные устройства ( не сохранены )");
     }
 }
}


QString MainWindow::tableToString(struct_listSavedDevices table_point)
{
  QString str = table_point.name +";"+QString::number(table_point.device.LastDate) +","
                         +QString::number(table_point.device.LogError) +","
                         +QString::number(table_point.device.SerialNum) +","
                         +QString::number(table_point.device.TypeDevice) +","
                         +QString::number(table_point.device.VerApp);
  return str;
}

MainWindow::struct_listSavedDevices MainWindow::stringToTable(QString str)
{
     QStringList name = str.split(";");
     QStringList tablstr  = name[1].split(",");
     struct_listSavedDevices table;
     table.name = name[0];
     table.device.LastDate = tablstr[0].toUInt();
     table.device.LogError = tablstr[1].toUShort();
     table.device.SerialNum = tablstr[2].toUInt();
     table.device.TypeDevice = tablstr[3].toUShort();
     table.device.VerApp = tablstr[4].toUInt();
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
        ui->treeWidget->setHeaderLabel("Найденные устройства");
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
