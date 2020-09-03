#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QModbusRtuSerialMaster>
#include <QSerialPort>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->add,&QAction::triggered,this,&MainWindow::libsAdd);
    connect(ui->view,&QAction::triggered,this,&MainWindow::LibsView);
    connect(ui->searh,&QAction::triggered,this,&MainWindow::DevicesSearch);
    connect(ui->saved,&QAction::triggered,this,&MainWindow::DevicesSaved);

    modbusDevice = new QModbusRtuSerialMaster(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error)
    {
        ui->textBrowser->append(modbusDevice->errorString());
    });

    connect(modbusDevice, &QModbusClient::stateChanged,this,[this](QModbusDevice::State)
    {
        if(modbusDevice->state() == QModbusDevice::UnconnectedState)
         ui->textBrowser->append("Modbus Disconnect");
        if(modbusDevice->state() == QModbusDevice::ConnectedState)
         ui->textBrowser->append("Modbus Connect");
    });


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

 if (modbusDevice->state() == QModbusDevice::ConnectedState)
     modbusDevice->disconnectDevice();

 modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,"COM3");
 modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
 modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud115200);
 modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
 modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);

 modbusDevice->setTimeout(10);
 modbusDevice->setNumberOfRetries(3);


 if (!modbusDevice->connectDevice())
 {
     ui->textBrowser->append(tr("Connect failed: ") + modbusDevice->errorString());
 }

 if(modbusDevice->state() == QModbusDevice::ConnectedState)
 {
         QModbusDataUnit request;
         request.setRegisterType(QModbusDataUnit::InputRegisters);
         request.setStartAddress(0);
         request.setValueCount(8);

         for ( int i=1;i<50;i++)
         {
               sendModbusRequest(request,i);
         }
  }
}

void MainWindow::sendModbusRequest(QModbusDataUnit request,int adress)
{
 if (auto *reply = modbusDevice->sendReadRequest(request, adress)) {
     if (!reply->isFinished())
         connect(reply, &QModbusReply::finished, this, &MainWindow::readyModbusRequest);
     else
         delete reply; // broadcast replies return immediately
 } else {
     ui->textBrowser->append("Read error: " + modbusDevice->errorString());
 }
}

void MainWindow::readyModbusRequest()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;
    if (reply->error() == QModbusDevice::NoError) {
        ui->textBrowser->append("Найдено устройство по адресу: "+QString::number(reply->serverAddress()));
        const QModbusDataUnit unit = reply->result();
        if(int(unit.valueCount()) == sizeof(struct_tableRegsRead)/2)
        {
            union_tableRegsRead LoclTable;
            for(int i = 0, total  = int(unit.valueCount()); i < total ;i++)
            {
               LoclTable.Adr[i] = unit.value(i);
            }
            getNewDevice(LoclTable,reply->serverAddress());
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
//        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
//                                    arg(reply->errorString()).
//                                    arg(reply->error(), -1, 16), 5000);

        ui->textBrowser->append("Не обнаруженно по адресу: "+QString::number(reply->serverAddress()));
    }
    reply->deleteLater();
}

void MainWindow::getNewDevice(union_tableRegsRead table,int adress)
{
   QString str =  "Modbus: "+QString::number(adress)+ " Type: " + QString::number(table.Regs.TypeDevice) + " SerialNumber: "+ QString::number(table.Regs.SerialNum);
   ui->listMain->addItem(str);
}

void MainWindow::DevicesSaved()
{

}
