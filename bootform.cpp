#include "bootform.h"
#include "ui_bootform.h"

#include <QFile>
#include <QFileDialog>

#include <string.h>
#include <cstring>

bootForm::bootForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bootForm)
{
    ui->setupUi(this);
    connect(ui->button_file,&QPushButton::clicked,this,&bootForm::ButtonOpenFile);
}

void bootForm::setTable(struct_listSavedDevices table)
{
    memcpy(&Bootdevice,&table,sizeof(struct_listSavedDevices));
    struct_listSavedDevices Bootdevice;
}

void bootForm::setModbus(QModbusClient *modbusDev)
{
   BootmodbusDevice = modbusDev;
}


void bootForm::ButtonOpenFile()
{
 QFile file;
 QString loadFileName = QFileDialog::getOpenFileName(this,
                                                  tr("Открыть"),
                                                  QString(),
                                                  "JSON (*.hex)");
  QFileInfo fileInfo(loadFileName);
  file.setFileName(loadFileName);
  QString hexstr;
  hexFileString.clear();

  if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
  {
      ui->lin_pathfile->setText(loadFileName);

      struct_hex_str hex_struct;

      while(!file.atEnd())
      {
          hexstr=file.readLine();
          hexFileString << hexstr;
          if(Ascii_To_Hex(&hex_struct,hexstr) )
          {
            hexFileStruct << hex_struct;
          }else
          {

            ui->textBrowser->setText("HEX not readed");
            break;
          }
          ui->textBrowser->setText(hexstr);
         // ui->textBrowser->append(hexstr);
      }

      ui->textBrowser->append("File open");
      file.close();
  }else
  {
    ui->textBrowser->append("File not open");
  }
}

bool bootForm::Ascii_To_Hex(struct_hex_str* str_hex, QString str)
{
   bool stat = false;
   str.remove(":");
   str.remove("\r\n");
//    uint8_t i;

//    for(i=0; i<count;i++)
//    {
//        if(buff[i] <= '9' && buff[i] >= '0' )
//        {
//            buff[i] -= 0x30;
//        }
//        else
//        {
//            buff[i] = buff[i] - 0x41 + 10;
//        }
//    }



    return stat;
}


bootForm::~bootForm()
{
    delete ui;
}
