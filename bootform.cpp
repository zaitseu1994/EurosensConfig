#include "bootform.h"
#include "ui_bootform.h"

#include <QFile>
#include <QFileDialog>
#include <QDateTime>

#include <string.h>
#include <cstring>

bootForm::bootForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bootForm)
{
    ui->setupUi(this);
    ui->button_boot->setEnabled(false);

    memset(&tablH,0,sizeof(tablH));

    fileInfo = new QFileInfo();

    ModbusBootTimer = new QTimer();
    ModbusBootTimer->stop();
    ModbusBootTimer->setSingleShot(false);
    ModbusBootTimer->setInterval(PreBootTimer);

    ui->button_toboot->setEnabled(true);

    connect(ui->button_toboot,&QPushButton::clicked,this,[=]
    {
        static bool butstat =false;

        if( butstat ==false )
        {
            for(int i=0;i<vectorModbusDevice.count();i++)
            {
                if( vectorModbusDevice[i].nameCom == ui->comboBox->itemText(i))
                {
                    setModbus(vectorModbusDevice[i].modbusDev);
                    ModbusBootTimer->start();
                    ui->button_toboot->setText(tr("Отменить режим"));
                    ui->spin_adrmodbus->setEnabled(false);
                    ui->comboBox->setEnabled(false);
                    butstat = true;
                    break;
                }
            }
        }else
        {
            butstat = false;
            ui->spin_adrmodbus->setEnabled(true);
            ui->comboBox->setEnabled(true);
            ui->button_toboot->setText(tr("Перевести в режим"));
            ui->lab_info->setText("");
            ui->textBrowser->clear();

            if(bootFileOpen)
            {
                ui->button_toboot->setEnabled(true);
                ui->textBrowser->append("File open");
                ui->textBrowser->append("================");
                ui->textBrowser->append("File name: "+fileInfo->fileName());
                ui->textBrowser->append("Time created: "+ fileInfo->birthTime().toString("yyyy-MM-dd HH:mm:ss"));
                ui->textBrowser->append("Files lines: "+QString::number(hexFileStruct.length()) );
                ui->textBrowser->append("================");
            }
            LoclTableHex->Regs.RegStatus =0;
            ModbusBootTimer->stop();
            ui->button_boot->setEnabled(false);
        }
    });

    connect(ModbusBootTimer,&QTimer::timeout,this,&bootForm::enterBootMode);
    connect(ui->button_file,&QPushButton::clicked,this,&bootForm::ButtonOpenFile);
    connect(ui->button_boot,&QPushButton::clicked,this,&bootForm::ButtonBoot);
}

void bootForm::setTable(struct_listSavedDevices table)
{
    memcpy(&Bootdevice.device,&table.device,sizeof(union_tableRegsRead));
    Bootdevice.devicename = table.devicename;
    Bootdevice.modbusadr = table.modbusadr;
    Bootdevice.portname = table.portname;
}


void bootForm::setFindDev(bool flag)
{
   deviceIsfind = flag;
   if( deviceIsfind )
   {
     ui->spin_adrmodbus->setValue(Bootdevice.modbusadr.toInt());

     for(int i=0;i<vectorModbusDevice.count();i++)
     {
         if( vectorModbusDevice[i].nameCom == Bootdevice.portname)
         {
             setModbus(vectorModbusDevice[i].modbusDev);
             ui->comboBox->setCurrentIndex(i);
             break;
         }
     }
   }
   else
   {

   }
   ui->button_file->setEnabled(true);
   ui->button_toboot->setEnabled(false);
}

void bootForm::enterBootMode()
{
    LoclTableHex->Regs.RegComand = (uint16_t)CMD_READY_TO_BOOT;
    LoclTableHex->Adr[8] = 11;

    if( LoclTableHex->Regs.RegStatus == (uint16_t)ANSW_READY_TO_BOOT)
    {
        if( LoclTableHex->Regs.RegErors == ERR_NOT )
         {
            ui->textBrowser->clear();
            ui->button_toboot->setEnabled(true);
            ui->button_boot->setEnabled(true);
            ui->textBrowser->append("File open");
            ui->textBrowser->append("================");
            ui->textBrowser->append("File name: "+fileInfo->fileName());
            ui->textBrowser->append("Time created: "+ fileInfo->birthTime().toString("yyyy-MM-dd HH:mm:ss"));
            ui->textBrowser->append("Files lines: "+QString::number(hexFileStruct.length()) );
            ui->textBrowser->append("================");
            ui->textBrowser->append("Press button to start BOOT...");
            ui->lab_info->setText("Boot Mode Enable");
         }else
         {
            ui->lab_info->setText("Wait Boot Mode");
            ui->textBrowser->append("Error: ANSW_READY_TO_BOOT");
            ui->textBrowser->append("Error:"+ QString::number(LoclTableHex->Regs.RegErors));
            ui->button_boot->setEnabled(false);
         }
         LoclTableHex->Regs.RegStatus = 0;
    }else
    {
          ui->lab_info->setText("Wait Boot Mode");
          ui->button_boot->setEnabled(false);
    }

    sendRegs(START_ALL_ADR, 9);
    updateRegsBoot(START_MAIN_ADR_R, COUNT_MAIN_REGS_R);
}

void bootForm::setVector(QVector<struct_ComModbus>& vec)
{
   vectorModbusDevice.clear();
   for( int i=0;i<vec.count();i++)
   {
       vectorModbusDevice << vec[i];
       ui->comboBox->addItem(vec[i].nameCom);
   }
}

void bootForm::setModbus(QModbusClient *modbusDev)
{
   BootmodbusDevice = modbusDev;
}


QByteArray bootForm::readFileFromCode(QString *str,bool *open)
{
    QByteArray  keyarray(std::begin<char>({0x45, 0x0A, 0x0B, 0x0c, 0x02, 0x05, 0x35,0x45, 0x0A, 0x045, 0x1c, 0x32, 0x15, 0x25,0x45, 0x6A, 0x07, 0x1c, 0x32, 0x55, 0x35}), 21);
    QFile file;
    QByteArray output;
    *str = QFileDialog::getOpenFileName(this,
                                                     tr("Открыть"),
                                                     QString(),
                                                     "JSON (*.cod)");
    file.setFileName(*str);
    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        QByteArray input = file.readAll();
        const char* key = keyarray;
        int keyLength = keyarray.length();

        for ( int i = 0; i < input.length() ; i++ )
        {
            output.append(input[i] ^ key[i % keyLength + 1]);
        }
        file.close();
        *open = true;
    }else
    {     *open = false;
          ui->textBrowser->append("File not open");
    }
    return output;
}

void bootForm::ButtonOpenFile()
{
 ui->button_boot->setEnabled(false);
 currentStepBoot = 0;
 currentNumStruct = 0;
 QString loadFileName;
 bool open;
 QByteArray arrayHEX = readFileFromCode(&loadFileName,&open);
 if ( open )
 {
     QList<QByteArray> lines = arrayHEX.split('\n');
     fileInfo->setFile(loadFileName);

      QString hexstr;
      hexFileString.clear();
      hexFileStruct.clear();

      ui->lin_pathfile->setText(loadFileName);

      union_hex_str hex_struct;
      int num_str =1;
      bool hexcomplete = false;
      ui->progressBar->setValue(0);

      foreach ( const QByteArray &line, lines)
      {
          hexstr=QString::fromLatin1(line);
          hexFileString << hexstr;
          hexstr.remove(":");
          hexstr.remove("\r");
          if(hexstr.toLatin1().length()>0 )
          {
                  if( Ascii_To_HexStruct(&hex_struct,hexstr.toLatin1()) )
                  {
    #ifdef BOOT_FILE_DEBUG
                    ui->textBrowser->append(QString::number(num_str)+": HEX Line::" +hexstr);
                    ui->textBrowser->append("CountByte:"+QString::number(hex_struct.Regs.countByte,16));
                    ui->textBrowser->append("adrFlash:"+QString::number(hex_struct.Regs.adrFlash,16));
                    ui->textBrowser->append("typeStr:"+QString::number(hex_struct.Regs.typeStr,16));
                    ui->textBrowser->append("================");
    #endif
                    hexFileStruct << hex_struct;
                    hexcomplete = true;
                  }else
                  {
                    hexcomplete = false;
                    hexFileString.clear();
                    hexFileStruct.clear();
                    ui->textBrowser->append(QString::number(num_str)+": HEX Line not readed");
                    ui->textBrowser->append(hexstr);
                    break;
                  }
                  num_str++;
          }
      }
      if( hexcomplete )
      {
          bootFileOpen = true;
          ui->button_toboot->setEnabled(true);
          ui->textBrowser->append("File open");
          ui->textBrowser->append("================");
          ui->textBrowser->append("File name: "+fileInfo->fileName());
          ui->textBrowser->append("Time created: "+ fileInfo->birthTime().toString("yyyy-MM-dd HH:mm:ss"));
          ui->textBrowser->append("Files lines: "+QString::number(hexFileStruct.length()) );
          ui->textBrowser->append("================");
      }else
      {
          ui->textBrowser->append("File have problem");
      }
 }
}

bool bootForm::Ascii_To_HexStruct(union_hex_str* str_hex, QByteArray buf)
{
   bool stat = false;
   uint8_t buf_len = buf.length();
   QByteArray bufH;
   if( buf_len > 0 )
   {
        for(int i=0; i<buf_len;i++)
        {
            if(buf[i] <= '9' && buf[i] >= '0' )
            {
                buf[i] = buf[i] - 0x30;
            }
            else
            {
                buf[i] = buf[i] - 0x41 + 10;
            }
        }

       for(int i=0; i< buf_len/2;i++)
       {
          bufH.append((buf[i*2]<<4) + buf[i*2+1]);
       }

       uint8_t crc = 0;
       for(int i=0;i< (buf_len/2)-1;i++)
       {
           crc += bufH[i] ;
       }
       crc = ~crc+0x01;

       if( crc!= static_cast<uint8_t>(bufH[(buf_len/2)-1]) )
       {
           stat = false;
       }else
       {
         str_hex->Regs.countByte = bufH[0];
         str_hex->Regs.adrFlash[0] = bufH[1];
         str_hex->Regs.adrFlash[1] = bufH[2];
         str_hex->Regs.typeStr = bufH[3];
         for(int i=0;i<str_hex->Regs.countByte;i++)
         {
             str_hex->Regs.str[i] = static_cast<uint8_t>(bufH[i+4]);
         }
         str_hex->Regs.crcStr = bufH[(buf_len/2)-1];

         stat = true;
       }
   }
   return stat;
}

void bootForm::ButtonBoot()
{
     currentNumStruct = 0;
     currentStepBoot = 1;
     ui->button_boot->setEnabled(false);
     ModbusBootTimer->stop();

     currentStepBoot = 0;
     bootSteps(0);
}

void bootForm::bootSteps(int numRepeat)
{
 volatile bool flashStop = false;
 volatile int countRegsSend = COUNT_MAIN_REGS_W;
 LoclTableHex->Regs.RegComand = 0;

    switch (currentStepBoot)
    {
    case 0:
        LoclTableHex->Regs.RegComand = (uint16_t)CMD_READY_TO_BOOT;
        currentStepBoot++;
        ui->lab_info->setText(tr("Старт загрузки"));
        break;
    case 1:
        if( LoclTableHex->Regs.RegStatus == (uint16_t)ANSW_READY_TO_BOOT)
        {
            if( LoclTableHex->Regs.RegErors == ERR_NOT)
             {
                LoclTableHex->Regs.RegStatus = 0;
                currentStepBoot++;
                ui->textBrowser->append(tr("Старт загрузки"));
             }else
             {
                ui->textBrowser->append(tr("Ошибка старта загрузки"));
                ui->textBrowser->append("Error:"+ QString::number(LoclTableHex->Regs.RegErors));
                flashStop = true;
             }
        }else
        {
            countNoAnswerProtcl++;
        }
        break;
    case 2:
        LoclTableHex->Regs.RegComand = (uint16_t)CMD_START_BOOT;
        currentStepBoot++;
        ui->lab_info->setText(tr("Очистка памяти"));
        break;
    case 3:
        if( LoclTableHex->Regs.RegStatus == (uint16_t)ANSW_START_BOOT )
        {
            if(LoclTableHex->Regs.RegErors == ERR_NOT)
            {
                LoclTableHex->Regs.RegStatus = 0;
                currentStepBoot++;
                ui->textBrowser->append(tr("Очистка памяти"));
            }else
            {
                ui->textBrowser->append(tr("Ошибка очистки памяти"));
                ui->textBrowser->append("Error:"+ QString::number(LoclTableHex->Regs.RegErors));
                flashStop = true;
            }
        }
        break;
    case 4:// заполнить строку отправить команду
         {
            uint32_t byte_offset = 0;
            for(int i=0;i<COUNT_STR_BLOCK;i++)
            {
               memcpy(&LoclTableHex->Regs.BUF[byte_offset],hexFileStruct[currentNumStruct].adr,hexFileStruct[currentNumStruct].Regs.countByte+4);
               byte_offset += (hexFileStruct[currentNumStruct].Regs.countByte+4);
               LoclTableHex->Regs.BUF[byte_offset] = hexFileStruct[currentNumStruct].Regs.crcStr;
               byte_offset+=1;
               if(hexFileStruct[currentNumStruct].Regs.typeStr == 1 )
               i = COUNT_STR_BLOCK;
               currentNumStruct++;
            }
            countRegsSend = ((14+byte_offset)/2+1);

            LoclTableHex->Regs.RegComand = (uint16_t)CMD_FLASH_STR;
            LoclTableHex->Regs.RegStatus = 0;
            currentStepBoot++;
            if(currentNumStruct-1 ==0)
            ui->textBrowser->append(tr("Загрузка строк"));
            ui->lab_info->setText(tr("Загрузка строки:") +QString::number(currentNumStruct-1));
        }
        break;
    case 5:
        if( LoclTableHex->Regs.RegStatus == (uint16_t)ANSW_FLASH_STR )
        {
             if( LoclTableHex->Regs.RegErors == ERR_NOT )
             {
                 LoclTableHex->Regs.RegStatus = 0;
                 if( currentNumStruct < hexFileStruct.length() )
                 {
                     currentStepBoot = 4;
                 }else
                 {
                     currentStepBoot = 6;
                 }
             }else
             {
                 ui->textBrowser->append(tr("Ошибка загрузки строки"));
                 ui->textBrowser->append("Error: "+ QString::number(LoclTableHex->Regs.RegErors));
                 ui->textBrowser->append("Error: line"+QString::number(currentNumStruct-1-COUNT_STR_BLOCK)+"-"+QString::number(currentNumStruct-1));
                 flashStop = true;
             }
        }
        break;
    case 6:
        LoclTableHex->Regs.RegComand = (uint16_t)CMD_END_BOOT;
        currentStepBoot++;
        ui->lab_info->setText(tr("Конец загрузки"));
        break;
    case 7:
        if( LoclTableHex->Regs.RegStatus == (uint16_t)ANSW_END_BOOT )
        {
            if( LoclTableHex->Regs.RegErors == ERR_NOT )
            {
                //датчик прошит
               ui->textBrowser->append(tr("Датчик прошит"));
               currentStepBoot=0;
               ModbusBootTimer->start();
               flashStop = true;
            }else
            {
                ui->textBrowser->append(tr("Ошибка конца загрузки"));
                ui->textBrowser->append("Error:"+ QString::number(LoclTableHex->Regs.RegErors));
                flashStop = true;
            }
        }
        break;
    }

    if((!flashStop) && (numRepeat<10) && ( countNoAnswerProtcl < 100 ) )
    {
          if(LoclTableHex->Regs.RegComand!=0)
          {
              countNoAnswerProtcl =0;
              sendRegs(START_ALL_ADR, countRegsSend);
          }
          updateRegs(START_MAIN_ADR_R, COUNT_MAIN_REGS_R);

    }else
    {
        if(!(countNoAnswerProtcl < 100) )
        ui->textBrowser->append("Answer timeout, protocol is working");
        if(!(numRepeat<10) )
        {
            switch (currentStepBoot)
            {
            case 1:// ошибка с приемом команды готовности к прошивке
                ui->textBrowser->append("Error: ANSW_READY_TO_BOOT" );
                break;
            case 3: // ошибка с приемом команды очистки флеш
                ui->textBrowser->append("Error: ANSW_START_BOOT");
                break;
            case 5: // ошибка с приемом команды записи строки
                ui->textBrowser->append("Error: ANSW_FLASH_STR");
                break;
            case 7: // ошибка с приемом команды окончания прошивки
                ui->textBrowser->append("Error: ANSW_END_BOOT");
                break;
            }
        }
    }
    ui->progressBar->setValue((static_cast<float>(currentNumStruct)/hexFileStruct.length())*100);
}

void bootForm::sendRegs(int startAdr, int countregs)
{
    if ( BootmodbusDevice->state() == QModbusDevice::ConnectedState)
    {
            QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAdr, countregs);
            QModbusReply *reply = nullptr;

            // - функция заполнения writeUnit
            for (int i = 0, total = int(writeUnit.valueCount()); i < total; i++) {
                 writeUnit.setValue(i,LoclTableHex->Adr[i+writeUnit.startAddress()]);
            }

            reply =  BootmodbusDevice->sendWriteRequest(writeUnit,ui->spin_adrmodbus->value());

        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &bootForm::replyReceivedWrite);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {

        }
    } else
    {
//       emit MWSErrorString(tr("Утеряно соединение с портом!"));
//       emit DevDisconnect(device);
    }
}

void bootForm::replyReceivedWrite()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
        } else {
//            switch (currentStepBoot)
//            {
//            case 0:// ошибка с поссылкой команды готовности к прошивке
//                ui->textBrowser->append("Error: CMD_READY_TO_BOOT" );
//                break;
//            case 3: // ошибка с посылкой команды очистки флеш
//                ui->textBrowser->append("Error: CMD_START_BOOT");
//                break;
//            case 5: // ошибка с посылкой команды записи строки
//                ui->textBrowser->append("Error: CMD_FLASH_STR");
//                break;
//            }
//            ui->textBrowser->setText(tr("Ошибка канала связи ")+ "replyReceivedWrite "+ replyModbus->errorString());
        }
        replyModbus->deleteLater();
}


void bootForm::updateRegs(int startAdr, int countregs)
{
    if ( BootmodbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,startAdr,countregs);
        QModbusReply *reply = nullptr;
        reply =  BootmodbusDevice->sendReadRequest(readUnit, ui->spin_adrmodbus->value());
        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &bootForm::replyReceivedRead);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {

        }

    } else
    {
//       emit MWSErrorString(tr("Утеряно соединение с портом!"));
//       emit DevDisconnect(device);
    }
}

void bootForm::replyReceivedRead()
{
    static int numRepeat =0;
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTableHex->Adr[i+unit.startAddress()] = unit.value(i);
                }
                numRepeat=0;
                bootSteps(0);
        } else {
           // ui->textBrowser->append(tr("Ожидание ответа ")+ "replyReceivedRead "+ replyModbus->errorString());
            bootSteps(numRepeat);
            numRepeat++;
        }
        replyModbus->deleteLater();
}

void bootForm::updateRegsBoot(int startAdr, int countregs)
{
    if ( BootmodbusDevice->state() == QModbusDevice::ConnectedState)
    {
        QModbusDataUnit readUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,startAdr,countregs);
        QModbusReply *reply = nullptr;
        LoclTableHex->Regs.RegStatus = 0;
        reply =  BootmodbusDevice->sendReadRequest(readUnit, ui->spin_adrmodbus->value());
        if (reply) {
            if (!reply->isFinished())
            {
                 connect(reply, &QModbusReply::finished, this, &bootForm::replyReadBoot);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {

        }

    } else
    {
//       emit MWSErrorString(tr("Утеряно соединение с портом!"));
//       emit DevDisconnect(device);
    }
}

void bootForm::replyReadBoot()
{
    auto replyModbus = qobject_cast<QModbusReply *>(sender());
    if ( !replyModbus )
    return;
        if (replyModbus->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = replyModbus->result();

                for(int i = 0, total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
                {
                   LoclTableHex->Adr[i+unit.startAddress()] = unit.value(i);
                }

        } else {

            ui->textBrowser->setText(tr("Ожидание ответа ")+ "replyReceivedRead "+ replyModbus->errorString());
        }
   replyModbus->deleteLater();
}


bootForm::~bootForm()
{
    ModbusBootTimer->stop();
    delete ModbusBootTimer;
    delete fileInfo;
    delete ui;
}
