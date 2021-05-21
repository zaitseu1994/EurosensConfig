#include "differenseanglvolt.h"
#include "ui_differenseanglvolt.h"

DifferenseAnglVolt::DifferenseAnglVolt(dev_base *parent) :
    dev_base(parent),
    ui(new Ui::DifferenseAnglVolt)
{
    ui->setupUi(this);

}

DifferenseAnglVolt::~DifferenseAnglVolt()
{
    delete ui;

    Tim10PerSecond->stop();
    Tim1PerSecond->stop();
    delete Tim10PerSecond;
    delete Tim1PerSecond;
}

void DifferenseAnglVolt::InitGraphsVoltsChanel()
{
    for(int i=0;i<DEEP_GRAPHS_VOLTS_POINTS;i++)
    {
      graph_amplitudeTIME << 0;
      graph_amplitudeCH1 << 0;
      graph_amplitudeCH2 << 0;
      graph_amplitudeCH3 << 0;
      graph_amplitudeCH4 << 0;
    }


    ui->graph_volts->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
    ui->graph_volts->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
    ui->graph_volts->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
    ui->graph_volts->axisRect()->setRangeZoom(Qt::Horizontal);   // Включаем удаление/приближение только по горизонтальной оси

    ui->graph_volts->addGraph(); // 0 - график канала 1
    ui->graph_volts->addGraph(); // 1 - график канала 2
    ui->graph_volts->addGraph(); // 2 - график канала 3
    ui->graph_volts->addGraph(); // 3 - график канала 4

    QPen pen1;
    QPen pen2;
    QPen pen3;
    QPen pen4;
    pen1.setWidth(1);
    pen1.setColor(QColor(0x55,0x05,0x05));
    ui->graph_volts->graph(0)->setPen(pen1); // цвет и ширина графика 1 канала

    pen2.setWidth(1);
    pen2.setColor(QColor(0x35,0x25,0x65));
    ui->graph_volts->graph(1)->setPen(pen2);// цвет и ширина графика 2 канала

    pen3.setWidth(1);
    pen3.setColor(QColor(0x25,0x25,0x25));
    ui->graph_volts->graph(2)->setPen(pen3);// цвет и ширина графика 3 канала

    pen4.setWidth(1);
    pen4.setColor(QColor(200,240,30));
    ui->graph_volts->graph(3)->setPen(pen4);// цвет и ширина графика 4 канала

    ui->graph_volts->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeCH1);
    ui->graph_volts->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeCH2);
    ui->graph_volts->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeCH3);
    ui->graph_volts->graph(3)->setData(graph_amplitudeTIME, graph_amplitudeCH4);

    ui->graph_volts->rescaleAxes();
    ui->graph_volts->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_volts->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(200, 250, 200));
    plotGradient.setColorAt(1, QColor(160, 50, 50));
    ui->graph_volts->setBackground(plotGradient);

    ui->graph_volts->xAxis->grid()->setVisible(false);
    ui->graph_volts->yAxis->grid()->setVisible(false);

    ui->graph_volts->replot();
}

void DifferenseAnglVolt::WritePointsGraphs()
{
    static VoltChanels v_volts;
    static AnglChanels a_angls1;
    static AnglChanels a_angls2;

    if(!Volts.isEmpty() )
    {
      v_volts = Volts.dequeue();
    }
    if(!AnglsFirst.isEmpty() )
    {
      a_angls1 = AnglsFirst.dequeue();
    }
    if(!AnglsSecond.isEmpty() )
    {
      a_angls2 = AnglsSecond.dequeue();
    }

    ui->graph_volts->graph(0)->setVisible(ui->check_ch1->isChecked());
    ui->graph_volts->graph(1)->setVisible(ui->check_ch2->isChecked());
    ui->graph_volts->graph(2)->setVisible(ui->check_ch3->isChecked());
    ui->graph_volts->graph(3)->setVisible(ui->check_ch4->isChecked());

    std::rotate ( graph_amplitudeTIME.begin() , graph_amplitudeTIME.begin()+1, graph_amplitudeTIME.end() ) ;
    std::rotate ( graph_amplitudeCH1.begin() , graph_amplitudeCH1.begin()+1, graph_amplitudeCH1.end() ) ;
    std::rotate ( graph_amplitudeCH2.begin() , graph_amplitudeCH2.begin()+1, graph_amplitudeCH2.end() ) ;
    std::rotate ( graph_amplitudeCH3.begin() , graph_amplitudeCH3.begin()+1, graph_amplitudeCH3.end() ) ;

    static double now = 0;
    static double chtime  = now;
    static double ch1 = static_cast<int>(now)%2;
    static double ch2 = static_cast<int>(now)%4;
    static double ch3  =static_cast<int>(now)%3;

    now++;
    chtime  = now;
    ch1 = static_cast<int>(now)%2;
    ch2 = static_cast<int>(now)%4;
    ch3  =static_cast<int>(now)%3;

    graph_amplitudeTIME[DEEP_GRAPHS_VOLTS_POINTS-1] = chtime;
    graph_amplitudeCH1[DEEP_GRAPHS_VOLTS_POINTS-1] = ch1;
    graph_amplitudeCH2[DEEP_GRAPHS_VOLTS_POINTS-1] = ch2;
    graph_amplitudeCH3[DEEP_GRAPHS_VOLTS_POINTS-1] = ch3;

    ui->graph_volts->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeCH1);
    ui->graph_volts->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeCH2);
    ui->graph_volts->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeCH3);
    ui->graph_volts->graph(3)->setData(graph_amplitudeTIME, graph_amplitudeCH4);

    ui->graph_volts->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_volts->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->graph_volts->rescaleAxes();
    ui->graph_volts->replot();
}


void DifferenseAnglVolt::updateRegs(int startAdr, int countregs)
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,startAdr,countregs);
        QModbusReply *reply = nullptr;
        reply =  modbusDevice->sendReadRequest(readUnit, device.modbusadr.toInt());
        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &DifferenseAnglVolt::replyReceivedRead);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {

        }

    } else
    {
       Tim1PerSecond->stop();
       emit DEVErrorString(tr("Утеряно соединение с портом!"));
       emit DevDisconnect(device);
    }
}

void DifferenseAnglVolt::replyReceivedRead()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

//                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
//                {
//                   LoclTableRecieve.Adr[i+unit.startAddress()] = unit.value(i);
//                }
//                updateAllSettingsView(&LoclTableRecieve);
//                addPointMeasure(LoclTableRecieve.Regs.CurrentDistanse,static_cast<double>(LoclTableRecieve.Regs.CurrentVolume)/1000);
//                upperModbusCheck();

//                if (firstRequest)
//                {
//                   updateFirstSettingsView(&LoclTableRecieve);
//                   firstRequest=false;
//                }
        } else {
            emit DEVErrorString(tr("Ошибка канала связи ")+"replyReceivedRead "+replyModbus->errorString());
            emit DevDisconnect(device);
            Tim1PerSecond->stop();
        }
        replyModbus->deleteLater();
}


void DifferenseAnglVolt::getTable(struct_listSavedDevices table)
{
    device = table;
    startTime = QDateTime::currentDateTime().toUTC();
}
void DifferenseAnglVolt::start(QModbusClient *modbusDev)
{
     modbusDevice = modbusDev;

     if(modbusDevice->state() == QModbusDevice::ConnectedState)
     {
         modbusDevice->setTimeout(MODBUS_TIMEOUT_PACKET);
         modbusDevice->setNumberOfRetries(MODBUS_COUNT_REPAET);
     }

     Tim10PerSecond = new QTimer();
     Tim1PerSecond = new QTimer();

     Tim10PerSecond->setInterval(TIMER_100ms);
     Tim10PerSecond->setSingleShot(false);
     connect(Tim10PerSecond,&QTimer::timeout,this,&DifferenseAnglVolt::WritePointsGraphs);

     connect(ui->button_start,&QPushButton::clicked,this,[=]
     {
         Tim10PerSecond->start();
     });

     connect(ui->button_stop,&QPushButton::clicked,this,[=]
     {
         Tim10PerSecond->stop();
     });

     InitGraphsVoltsChanel();
}
void DifferenseAnglVolt::setId(QString str)
{
     idUser = str;
}
void DifferenseAnglVolt::retranslate()
{

}
QJsonObject DifferenseAnglVolt::getSetting()
{
    QJsonObject json;
    json["Set1"]= 1;
    json["Set2"]= 1;
    json["Set3"]= 3;
    return json;
}
bool DifferenseAnglVolt::setSetting(QJsonObject json, QString idset,QString timeset)
{
    this->setEnabled(false);
    emit DevBusy(device);
    bool stat =false;

    stat = true;
    emit DevReady(device);
    emit DevSettingAccept(device,getSetting());
    return stat;
}
