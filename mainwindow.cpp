#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QPushButton>

#include <QLineEdit>
#include <QLabel>

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
                getNewDeviceModbus(LoclTable,vectorModbusDevice.first());
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
    //        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
    //                                    arg(reply->errorString()).
    //                                    arg(reply->error(), -1, 16), 5000);
        }
        replyModbus->deleteLater();
        pollAdrModbus();
}


QString MainWindow::findNameDevice(union_tableRegsRead table)
{
 return NULL;
}
void MainWindow::setNameDevice(union_tableRegsRead table,QString name)
{

}

void MainWindow::getNewDeviceModbus(union_tableRegsRead table, struct_ComModbus com)
{
   QString str =" Серийный номер: "+ QString::number(table.Regs.SerialNum) + " Имя: \" \" ";
   QTreeWidgetItem *toplevel = new QTreeWidgetItem(ui->treeWidget);
   toplevel->setText(0,str);
   QTreeWidgetItem *item=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *item1=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *item2=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *item3=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *item4=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *item5=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *Protocol=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *FisicalPort=new QTreeWidgetItem(toplevel);

   QLineEdit *name = new QLineEdit();
   name->setFrame(false);
   name->setText("Введите имя уст...");
   name->setStyleSheet("color: blue;"
                       "background-color: yellow;"
                       "selection-color: yellow;"
                       "selection-background-color: blue;"
                       "font: italic;");

   ui->treeWidget->setItemWidget(item,0,name);
   QLabel *typedevice = new QLabel();
   typedevice->setText("Тип: "+QString::number(table.Regs.TypeDevice));
   QLabel *SerialNum = new QLabel();
   SerialNum->setText("Серийный №: "+QString::number(table.Regs.SerialNum));
   QLabel *VerApp = new QLabel();
   VerApp->setText("Аппаратная версия: "+QString::number(table.Regs.VerApp));
   QLabel *LastDate = new QLabel();
   LastDate->setText("Дата связи: "+QString::number(table.Regs.LastDate));
   QLabel *LogError = new QLabel();
   LogError->setText("Лог ошибок: "+QString::number(table.Regs.LogError));

   ui->treeWidget->setItemWidget(item1,0,typedevice);
   ui->treeWidget->setItemWidget(item2,0,SerialNum);
   ui->treeWidget->setItemWidget(item3,0,VerApp);
   ui->treeWidget->setItemWidget(item4,0,LastDate);
   ui->treeWidget->setItemWidget(item5,0,LogError);

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
}



void MainWindow::DevicesSaved()
{
    for( int i=0;i<vectorModbusDevice.count();i++)
    {
        vectorModbusDevice[i].modbusDev->disconnectDevice();
        vectorModbusDevice[i].modbusDev->deleteLater();
    }
    vectorModbusDevice.clear();
}
