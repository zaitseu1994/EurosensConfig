#include "differenseanglvolt.h"
#include "ui_differenseanglvolt.h"
#include <QTextCodec>

DifferenseAnglVolt::DifferenseAnglVolt(dev_base *parent) :
    dev_base(parent),
    ui(new Ui::DifferenseAnglVolt)
{
    ui->setupUi(this);

}

DifferenseAnglVolt::~DifferenseAnglVolt()
{
//    if(ui->check_xsls->isChecked())
//    {
//        endLogTime = QTime::currentTime();
//        QString saveName = "Serial-"+QString::number(device.device.Regs.SerialNum)+"("+startLogTime.toString("hh-mm-ss")
//                                                                        +endLogTime.toString("hh-mm-ss")+")";
//        LogFileXlsx->saveAs(saveName);
//    }

    Tim10PerSecond->stop();
    Tim1PerSecond->stop();
    delete Tim10PerSecond;
    delete Tim1PerSecond;
    delete ui;
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

      graph_amplitudeANGLX << 0;
      graph_amplitudeANGLY << 0;
      graph_amplitudeANGLZ << 0;
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
    pen1.setWidth(3);
    pen1.setColor(QColor(0x55,0x05,0x05));
    ui->graph_volts->graph(0)->setPen(pen1); // цвет и ширина графика 1 канала

    pen2.setWidth(3);
    pen2.setColor(QColor(0x35,0x25,0x65));
    ui->graph_volts->graph(1)->setPen(pen2);// цвет и ширина графика 2 канала

    pen3.setWidth(3);
    pen3.setColor(QColor(0x25,0x25,0x25));
    ui->graph_volts->graph(2)->setPen(pen3);// цвет и ширина графика 3 канала

    pen4.setWidth(3);
    pen4.setColor(QColor(200,240,30));
    ui->graph_volts->graph(3)->setPen(pen4);// цвет и ширина графика 4 канала

    ui->graph_volts->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeCH1);
    ui->graph_volts->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeCH2);
    ui->graph_volts->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeCH3);
    ui->graph_volts->graph(3)->setData(graph_amplitudeTIME, graph_amplitudeCH4);

    ui->graph_volts->rescaleAxes();
    ui->graph_volts->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_volts->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);


//    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//    dateTicker->setDateTimeFormat("hh:mm:ss,zzz");
//    ui->graph_volts->xAxis->setTicker(dateTicker);
//    ui->graph_angls->xAxis->setTicker(dateTicker);

//    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
//    timeTicker->setTimeFormat("%h:%m:%s");
//    ui->graph_volts->xAxis->setTicker(timeTicker);
//    ui->graph_angls->xAxis->setTicker(timeTicker);

    ui->graph_volts->xAxis->setVisible(true);
    ui->graph_volts->yAxis->setVisible(true);
    ui->graph_volts->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend);


    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(200, 250, 200));
    plotGradient.setColorAt(1, QColor(160, 50, 50));
    ui->graph_volts->setBackground(plotGradient);

    ui->graph_volts->xAxis->grid()->setVisible(false);
    ui->graph_volts->yAxis->grid()->setVisible(false);
    ui->graph_volts->replot();

    ui->graph_angls->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
    ui->graph_angls->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
    ui->graph_angls->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
    ui->graph_angls->axisRect()->setRangeZoom(Qt::Horizontal);   // Включаем удаление/приближение только по горизонтальной оси

    ui->graph_angls->addGraph(); // 0 - график угла X
    ui->graph_angls->addGraph(); // 1 - график угла Y
    ui->graph_angls->addGraph(); // 2 - график угла Z

    QPen pen1X;
    QPen pen2Y;
    QPen pen3Z;
    pen1X.setWidth(1);
    pen1X.setColor(QColor(0x55,0x05,0x05));
    ui->graph_angls->graph(0)->setPen(pen1); // цвет и ширина графика угла X

    pen2Y.setWidth(1);
    pen2Y.setColor(QColor(0x35,0x25,0x65));
    ui->graph_angls->graph(1)->setPen(pen2);// цвет и ширина графика угла Y

    pen3Z.setWidth(1);
    pen3Z.setColor(QColor(0x25,0x25,0x25));
    ui->graph_angls->graph(2)->setPen(pen3);// цвет и ширина графика угла Z

    ui->graph_angls->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeANGLX);
    ui->graph_angls->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeANGLY);
    ui->graph_angls->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeANGLZ);

    ui->graph_angls->rescaleAxes();
    ui->graph_angls->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_angls->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    QLinearGradient plotGradientANGLS;
    plotGradientANGLS.setStart(0, 0);
    plotGradientANGLS.setFinalStop(0, 350);
    plotGradientANGLS.setColorAt(0, QColor(200, 250, 200));
    plotGradientANGLS.setColorAt(1, QColor(160, 50, 50));
    ui->graph_angls->setBackground(plotGradientANGLS);

    ui->graph_angls->xAxis->grid()->setVisible(false);
    ui->graph_angls->yAxis->grid()->setVisible(false);
    ui->graph_angls->replot();
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

    ui->graph_angls->graph(0)->setVisible(ui->check_X->isChecked());
    ui->graph_angls->graph(1)->setVisible(ui->check_Y->isChecked());
    ui->graph_angls->graph(2)->setVisible(ui->check_Z->isChecked());

    std::rotate ( graph_amplitudeTIME.begin() , graph_amplitudeTIME.begin()+1, graph_amplitudeTIME.end() ) ;
    std::rotate ( graph_amplitudeCH1.begin() , graph_amplitudeCH1.begin()+1, graph_amplitudeCH1.end() ) ;
    std::rotate ( graph_amplitudeCH2.begin() , graph_amplitudeCH2.begin()+1, graph_amplitudeCH2.end() ) ;
    std::rotate ( graph_amplitudeCH3.begin() , graph_amplitudeCH3.begin()+1, graph_amplitudeCH3.end() ) ;
    std::rotate ( graph_amplitudeCH4.begin() , graph_amplitudeCH4.begin()+1, graph_amplitudeCH4.end() ) ;

    std::rotate ( graph_amplitudeANGLX.begin() , graph_amplitudeANGLX.begin()+1, graph_amplitudeANGLX.end() ) ;
    std::rotate ( graph_amplitudeANGLY.begin() , graph_amplitudeANGLY.begin()+1, graph_amplitudeANGLY.end() ) ;
    std::rotate ( graph_amplitudeANGLZ.begin() , graph_amplitudeANGLZ.begin()+1, graph_amplitudeANGLZ.end() ) ;

    static double now = 0;
    static double ch1 = v_volts.chanel1;
    static double ch2 = v_volts.chanel2;
    static double ch3 = v_volts.chanel3;
    static double ch4 = v_volts.chanel4;

    static double anglX = a_angls1.angl_x;
    static double anglY = a_angls1.angl_y;
    static double anglZ = a_angls1.angl_z;

    now++;
    ch1 = v_volts.chanel1;
    ch2 = v_volts.chanel2;
    ch3 = v_volts.chanel3;
    ch4 = v_volts.chanel4;

    anglX = a_angls1.angl_x;
    anglY = a_angls1.angl_y;
    anglZ = a_angls1.angl_z;

    graph_amplitudeTIME[DEEP_GRAPHS_VOLTS_POINTS-1] = now;
    graph_amplitudeCH1[DEEP_GRAPHS_VOLTS_POINTS-1] = static_cast<double>(ch1)/1000;
    graph_amplitudeCH2[DEEP_GRAPHS_VOLTS_POINTS-1] = static_cast<double>(ch2)/1000;
    graph_amplitudeCH3[DEEP_GRAPHS_VOLTS_POINTS-1] = static_cast<double>(ch3)/1000;
    graph_amplitudeCH4[DEEP_GRAPHS_VOLTS_POINTS-1] = static_cast<double>(ch4)/1000;

    graph_amplitudeANGLX[DEEP_GRAPHS_VOLTS_POINTS-1] = anglX;
    graph_amplitudeANGLY[DEEP_GRAPHS_VOLTS_POINTS-1] = anglY;
    graph_amplitudeANGLZ[DEEP_GRAPHS_VOLTS_POINTS-1] = anglZ;


    QString strCH1 = QString::number((static_cast<double>(ch1)/1000),'f',3);
    QString strCH2 = QString::number((static_cast<double>(ch2)/1000),'f',3);
    QString strCH3 = QString::number((static_cast<double>(ch3)/1000),'f',3);
    QString strCH4 = QString::number((static_cast<double>(ch4)/1000),'f',3);

//    ui->lcd_ch1->display(static_cast<double>(ch1)/1000);
//    ui->lcd_ch2->display(static_cast<double>(ch2)/1000);
//    ui->lcd_ch3->display(static_cast<double>(ch3)/1000);
//    ui->lcd_ch4->display(static_cast<double>(ch4)/1000);

    ui->lcd_ch1->display(strCH1);
    ui->lcd_ch2->display(strCH2);
    ui->lcd_ch3->display(strCH3);
    ui->lcd_ch4->display(strCH4);

    ui->lcd_X->display(anglX);
    ui->lcd_Y->display(anglY);
    ui->lcd_Z->display(anglZ);

    ui->graph_volts->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeCH1);
    ui->graph_volts->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeCH2);
    ui->graph_volts->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeCH3);
    ui->graph_volts->graph(3)->setData(graph_amplitudeTIME, graph_amplitudeCH4);

    ui->graph_volts->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_volts->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->graph_volts->rescaleAxes();
    ui->graph_volts->replot();

    ui->graph_angls->graph(0)->setData(graph_amplitudeTIME, graph_amplitudeANGLX);
    ui->graph_angls->graph(1)->setData(graph_amplitudeTIME, graph_amplitudeANGLY);
    ui->graph_angls->graph(2)->setData(graph_amplitudeTIME, graph_amplitudeANGLZ);

    ui->graph_angls->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->graph_angls->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    ui->graph_angls->rescaleAxes();
    ui->graph_angls->replot();
}


void DifferenseAnglVolt::sendRegs(int startAdr, int countregs)
{
    if ( modbusDevice->state() == QModbusDevice::ConnectedState)
    {
            QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAdr, countregs);
            QModbusReply *reply = nullptr;
            for (int i = 0, total = int(writeUnit.valueCount()); i < total; i++) {
                 writeUnit.setValue(i,Wtable.Adr[i+writeUnit.startAddress()]);
            }
            reply =  modbusDevice->sendWriteRequest(writeUnit,device.modbusadr.toInt());
        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &DifferenseAnglVolt::replyReceivedWrite);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {

        }
    } else
    {
       emit DEVErrorString(tr("Утеряно соединение с портом!"));
       emit DevDisconnect(device);
    }
}

void DifferenseAnglVolt::replyReceivedWrite()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            if( flgSave )
            {
               flgSave =false;
               emit DEVErrorString(tr("Калибровка сохранена"));
            }else
            {
               emit DEVErrorString(tr("Калибровка каналов произведена"));
            }
        } else {
            emit DEVErrorString(tr("Ошибка канала связи ")+ "replyReceivedWrite "+ replyModbus->errorString());
        }
        replyModbus->deleteLater();
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
            for(int i = 0,total  = int(unit.valueCount()); i < total ;i++)
            {
                Wtable.Adr[i+unit.startAddress()] = unit.value(i);
            }
            if(unit.startAddress()==10 )
            DataToQueue();
            else
            DataToView();
        } else {
            emit DEVErrorString(tr("Ошибка канала связи ")+"replyReceivedRead "+replyModbus->errorString());
            emit DevDisconnect(device);
            Tim1PerSecond->stop();
        }
        replyModbus->deleteLater();
}

void DifferenseAnglVolt::DataToQueue()
{
      VoltChanels volts;
      AnglChanels angl1;
      AnglChanels angl2;
      for(int i=COUNT_POINT-1;i!=0;i--)
      {
         volts.chanel1 = Wtable.Regs.adc_chanal1[i];
         volts.chanel2 = Wtable.Regs.adc_chanal2[i];
         volts.chanel3 = Wtable.Regs.adc_chanal3[i];
         volts.chanel4 = Wtable.Regs.adc_chanal4[i];

         angl1.angl_x = Wtable.Regs.angls1[i].anglX;
         angl1.angl_y = Wtable.Regs.angls1[i].anglY;
         angl1.angl_z = Wtable.Regs.angls1[i].anglZ;

         angl2.angl_x = Wtable.Regs.angls2[i].anglX;
         angl2.angl_y = Wtable.Regs.angls2[i].anglY;
         angl2.angl_z = Wtable.Regs.angls2[i].anglZ;


         if(Volts.length()<=COUNT_POINT*2)
         Volts << volts;
         if(AnglsFirst.length()<=COUNT_POINT*2)
         AnglsFirst << angl1;
         if(AnglsSecond.length()<=COUNT_POINT*2)
         AnglsSecond << angl2;
      }
      if(ui->check_xsls->isChecked())
      {
         logSave();
      }
}

void DifferenseAnglVolt::DataToView()
{
   ui->lab_ch1_vref->setText(QString::number(Wtable.Regs.settings.set[0]));
   ui->lab_ch2_vref->setText(QString::number(Wtable.Regs.settings.set[1]));
   ui->lab_ch3_vref->setText(QString::number(Wtable.Regs.settings.set[2]));
   ui->lab_ch4_vref->setText(QString::number(Wtable.Regs.settings.set[3]));

   ui->lab_ch1_v->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[4])/1000,'f',3));
   ui->lab_ch2_v->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[5])/1000,'f',3));
   ui->lab_ch3_v->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[6])/1000,'f',3));
   ui->lab_ch4_v->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[7])/1000,'f',3));

   ui->lin_vref_ch1->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[4])/1000,'f',3));
   ui->lin_vref_ch2->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[5])/1000,'f',3));
   ui->lin_vref_ch3->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[6])/1000,'f',3));
   ui->lin_vref_ch4->setText(QString::number(static_cast<float>(Wtable.Regs.settings.set[7])/1000,'f',3));

   ui->lab_adr_angl1->setText(QString::number(Wtable.Regs.settings.set[8]>>8));
   ui->lab_adr_angl2->setText(QString::number(Wtable.Regs.settings.set[8] & 0x00FF));

   ui->lin_adr_angl1->setText(QString::number(Wtable.Regs.settings.set[8]>>8));
   ui->lin_adr_angl2->setText(QString::number(Wtable.Regs.settings.set[8] & 0x00FF));

   ui->but_sendCalibration->setEnabled(true);
   ui->but_save->setEnabled(true);
}

void DifferenseAnglVolt::sendCalibration()
{

   uint16_t vref_ch1 = ui->lab_ch1_vref->text().toShort()*(ui->lin_vref_ch1->text().toFloat())/(ui->lab_ch1_v->text().toFloat());
   uint16_t vref_ch2 = ui->lab_ch2_vref->text().toShort()*(ui->lin_vref_ch2->text().toFloat())/(ui->lab_ch2_v->text().toFloat());
   uint16_t vref_ch3 = ui->lab_ch3_vref->text().toShort()*(ui->lin_vref_ch3->text().toFloat())/(ui->lab_ch3_v->text().toFloat());
   uint16_t vref_ch4 = ui->lab_ch4_vref->text().toShort()*(ui->lin_vref_ch4->text().toFloat())/(ui->lab_ch4_v->text().toFloat());

   if((vref_ch1!=0) && (vref_ch1>3000))
   {
          ui->lab_ch1_vref->setText(QString::number(Wtable.Regs.settings.set[0]));
          Wtable.Regs.settings.set[0] =  vref_ch1;
   }

   if((vref_ch2!=0) && (vref_ch1>3000))
   {
          ui->lab_ch2_vref->setText(QString::number(Wtable.Regs.settings.set[1]));
          Wtable.Regs.settings.set[1] =  vref_ch2;
   }

   if((vref_ch3!=0) && (vref_ch1>3000))
   {
          ui->lab_ch3_vref->setText(QString::number(Wtable.Regs.settings.set[2]));
          Wtable.Regs.settings.set[2] =  vref_ch3;
   }

   if((vref_ch4!=0) && (vref_ch1>3000))
   {
          ui->lab_ch4_vref->setText(QString::number(Wtable.Regs.settings.set[3]));
          Wtable.Regs.settings.set[3] =  vref_ch4;
   }

   uint16_t sum_adr_angls = ui->lin_adr_angl1->text().toShort()<<8;
   sum_adr_angls+=ui->lin_adr_angl2->text().toShort();
   Wtable.Regs.settings.set[8] = sum_adr_angls;

   ui->lab_ch1_vref->setText(QString::number(Wtable.Regs.settings.set[0]));
   ui->lab_ch2_vref->setText(QString::number(Wtable.Regs.settings.set[1]));
   ui->lab_ch3_vref->setText(QString::number(Wtable.Regs.settings.set[2]));
   ui->lab_ch4_vref->setText(QString::number(Wtable.Regs.settings.set[3]));

   ui->lab_adr_angl1->setText(QString::number(Wtable.Regs.settings.set[8]>>8));
   ui->lab_adr_angl2->setText(QString::number(Wtable.Regs.settings.set[8] & 0x00FF));

   sendRegs(0, 10);
}


void DifferenseAnglVolt::logSave()
{
    if(XSLXcountLine==1)
    {
        LogFileXlsx->write("A1", "Время");
        LogFileXlsx->write("B1", "Канал 1,мВ");
        LogFileXlsx->write("C1", "Канал 2,мВ");
        LogFileXlsx->write("D1", "Канал 3,мВ");
        LogFileXlsx->write("E1", "Канал 4,мВ");

        LogFileXlsx->write("F1", "Угол первого X");
        LogFileXlsx->write("G1", "Угол первого Y");
        LogFileXlsx->write("H1", "Угол первого Z");

        LogFileXlsx->write("I1", "Угол второго X");
        LogFileXlsx->write("J1", "Угол второго Y");
        LogFileXlsx->write("K1", "Угол второго Z");

        XSLXcountLine = 2;
    }else
    {
       for(int i=0;i<COUNT_POINT;i++)
       {
           LogFileXlsx->write("A"+QString::number(XSLXcountLine), QTime::currentTime().addMSecs(100*i).toString("hh:mm:ss,zzz"));
           LogFileXlsx->write("B"+QString::number(XSLXcountLine), Wtable.Regs.adc_chanal1[i]);
           LogFileXlsx->write("C"+QString::number(XSLXcountLine), Wtable.Regs.adc_chanal2[i]);
           LogFileXlsx->write("D"+QString::number(XSLXcountLine), Wtable.Regs.adc_chanal3[i]);
           LogFileXlsx->write("E"+QString::number(XSLXcountLine), Wtable.Regs.adc_chanal4[i]);

           LogFileXlsx->write("F"+QString::number(XSLXcountLine), Wtable.Regs.angls1[i].anglX);
           LogFileXlsx->write("G"+QString::number(XSLXcountLine), Wtable.Regs.angls1[i].anglY);
           LogFileXlsx->write("H"+QString::number(XSLXcountLine), Wtable.Regs.angls1[i].anglZ);

           LogFileXlsx->write("I"+QString::number(XSLXcountLine), Wtable.Regs.angls2[i].anglX);
           LogFileXlsx->write("J"+QString::number(XSLXcountLine), Wtable.Regs.angls2[i].anglY);
           LogFileXlsx->write("K"+QString::number(XSLXcountLine), Wtable.Regs.angls2[i].anglZ);
           XSLXcountLine++;
       }
    }
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

     Tim1PerSecond->setInterval(TIMER_1000ms);
     Tim1PerSecond->setSingleShot(false);
     connect(Tim1PerSecond,&QTimer::timeout,this,[=]
     {
         updateRegs(10, 70);
     });

     connect(ui->button_start,&QPushButton::clicked,this,[=]
     {

         if(ui->check_xsls->isChecked())
         {
             startLogTime = QTime::currentTime();
             LogFileXlsx = new QXlsx::Document(this);
             XSLXcountLine = 1;
             logSave();
         }
         ui->button_stop->setEnabled(true);
         ui->button_start->setEnabled(false);
         ui->group_setting_calibration->setEnabled(false);
         ui->check_xsls->setEnabled(false);
         Tim10PerSecond->start();
         Tim1PerSecond->start();
     });

     connect(ui->button_stop,&QPushButton::clicked,this,[=]
     {

         ui->button_stop->setEnabled(false);
         ui->button_start->setEnabled(true);
         ui->group_setting_calibration->setEnabled(true);
         ui->check_xsls->setEnabled(true);
         Tim10PerSecond->stop();
         Tim1PerSecond->stop();

         if(ui->check_xsls->isChecked())
         {
            endLogTime = QTime::currentTime();
            QString saveName = "Serial-"+QString::number(device.device.Regs.SerialNum)+"("+startLogTime.toString("hh-mm-ss")
                                                                            +"..."+endLogTime.toString("hh-mm-ss")+")";
            QString saveFileName = QFileDialog::getSaveFileName(this,tr("Сохранить"),saveName,tr("log(*.xlsx)"));
            LogFileXlsx->saveAs(saveFileName);
            delete LogFileXlsx;
         }
     });
     connect(ui->but_readCalibration,&QPushButton::clicked,this,[=]
     {
         updateRegs(0, 10);
     });
     connect(ui->but_sendCalibration,&QPushButton::clicked,this,[=]
     {
         ui->but_sendCalibration->setEnabled(false);
         sendCalibration();
     });
     connect(ui->but_save,&QPushButton::clicked,this,[=]
     {
        ui->but_save->setEnabled(false);
        flgSave = true;
        Wtable.Regs.settings.set[9] = CMD_SAVE ;
        sendCalibration();
     });

     InitGraphsVoltsChanel();
     ui->button_start->click();
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
    json["VrefCH1"]= Wtable.Regs.settings.set[0];
    json["VrefCH2"]= Wtable.Regs.settings.set[1];
    json["VrefCH3"]= Wtable.Regs.settings.set[2];
    json["VrefCH4"]= Wtable.Regs.settings.set[3];

    json["ADRANGL1"]= Wtable.Regs.settings.set[8]>>8;
    json["ADRANGL2"]= Wtable.Regs.settings.set[8] & 0x00FF;

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
