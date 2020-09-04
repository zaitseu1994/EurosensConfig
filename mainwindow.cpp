#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->add,&QAction::triggered,this,&MainWindow::libsAdd);
    connect(ui->view,&QAction::triggered,this,&MainWindow::LibsView);
    connect(ui->searh,&QAction::triggered,this,&MainWindow::DevicesSearch);
    connect(ui->saved,&QAction::triggered,this,&MainWindow::DevicesSaved);

//    modbusDevice = new QModbusRtuSerialMaster(this);
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
 ui->listMain->clear();
 ui->textBrowser->clear();

 const auto infos = QSerialPortInfo::availablePorts();
 QStringList list;
 for (const QSerialPortInfo &info : infos) {
     list << info.portName();
 }
 searchModbusDevice(list);
 pollModbus();
}

void MainWindow::searchModbusDevice(QStringList list)
{
    for( int i=0;i<vectorModbusDevice.count();i++)
    {
        vectorModbusDevice[i].modbusDev->disconnectDevice();
        vectorModbusDevice[i].modbusDev->deleteLater();
    }
    vectorModbusDevice.clear();

    for ( int i=0;i<list.count();i++ )
    {
           struct_ComModbus newCh;
           newCh.modbusDev = new QModbusRtuSerialMaster();
           newCh.nameCom =   list[i];
           newCh.currentAdr = 1;
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

        if (auto *reply =  vectorModbusDevice.first().modbusDev->sendReadRequest(request,  vectorModbusDevice.first().currentAdr)) {
            if (!reply->isFinished())
            {
                vectorModbusDevice.first().currentAdr++;
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
                ui->textBrowser->append("Найдено устройство; Протокол: MODBUS-rtu; Адрес: "+QString::number(replyModbus->serverAddress()));
                union_tableRegsRead LoclTable;
                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTable.Adr[i] = unit.value(i);
                }
                getNewDevice(LoclTable,replyModbus->serverAddress());
            }
        } else if (replyModbus->error() == QModbusDevice::ProtocolError) {
            statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                        arg(replyModbus->errorString()).
                                        arg(replyModbus->rawResult().exceptionCode(), -1, 16), 5000);
        } else {
            if ( replyModbus->serverAddress() == LAST_MODBUS_ADRESS-1 )
            {
                 ui->textBrowser->append("Закончен опрос порта: "+vectorModbusDevice.first().nameCom);
            }
    //        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
    //                                    arg(reply->errorString()).
    //                                    arg(reply->error(), -1, 16), 5000);
        }
        replyModbus->deleteLater();
        pollAdrModbus();
}


void MainWindow::getNewDevice(union_tableRegsRead table,int adress)
{
   QString str ="Type: " + QString::number(table.Regs.TypeDevice) + " SerialNumber: "+ QString::number(table.Regs.SerialNum);
   ui->listMain->addItem(str);
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
