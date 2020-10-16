#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

#include "QDockWidget"
#include "QSettings"

#include <QModbusRtuSerialMaster>
#include <QPushButton>
#include <QCommandLinkButton>

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



    connect(ModbusTimer,&QTimer::timeout,this,[this]()
            {
                ModbusTimer->stop();
                ui->actionSearh->setEnabled(true);
                ui->treeWidget->clear();
                ui->treeWidget->setEnabled(true);
                Progesslabel.hide();
                ProgressPercent.hide();
                QString comname = vectorModbusDevice[intcomModBusDevice].nameCom;
                QMessageBox::warning(this, comname,"Проверьте соединение с портом");
            });

    libs = new DeviceLibs;

    tree_dock = new QDockWidget(tr("Устройства"), this);
    tree_dock->setObjectName("tree");
    tree_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    tree_dock->setWidget(ui->treeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, tree_dock);
    ui->view_ui->addAction(tree_dock->toggleViewAction());

    browser_dock = new QDockWidget(tr("Состояние"), this);
    browser_dock->setObjectName("browser");
    browser_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    browser_dock->setWidget(ui->textBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, browser_dock);
    ui->view_ui->addAction(browser_dock->toggleViewAction());

    list_dock = new QDockWidget(tr("Инфо"), this);
    list_dock->setObjectName("list");
    list_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    list_dock->setWidget(ui->listInfo);
    addDockWidget(Qt::RightDockWidgetArea, list_dock);
    ui->view_ui->addAction(list_dock->toggleViewAction());

    connect(ui->actionSearh,&QAction::triggered,this,&MainWindow::DevicesSearch);
    connect(ui->actionSaved,&QAction::triggered,this,&MainWindow::DevicesSaved);
    connect(ui->actionQuit,&QAction::triggered,this,[=]
    {
        this->close();
    });

    ui->treeWidget->setHeaderLabel(" ");
   // ui->treeWidget->headerItem()->setHidden(true);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);
    ui->actionSaved->setDisabled(true);

    ProgressPercent.setValue(0);
    Progesslabel.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    ProgressPercent.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    Progesslabel.hide();
    ProgressPercent.hide();
    ui->statusbar->addWidget(&Progesslabel);
    ui->statusbar->addWidget(&ProgressPercent);


     login = new Login(this);
     butlogin = new QCommandLinkButton(this);
     butlogin->setText("Нажмите чтоб войти");

     ui->menu->setEnabled(false);
     ui->devices->setEnabled(false);
     ui->Settings->setEnabled(false);
     ui->actionSearh->setEnabled(false);
     ui->actionConfigure->setEnabled(false);

     login->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
     connect(login,&Login::closeLogin,this,[=]
     {
        login->close();
        this->close();
     });
     connect(login,&Login::clickLogin,this,[=](QString str)
     {
         if( str.length()>0)
         {
             setIduser(str);
             butlogin->setText("Сменить пользователя( ID "+idUser+" )");
             login->close();
             ui->menu->setEnabled(true);
             ui->devices->setEnabled(true);
             ui->Settings->setEnabled(true);
             ui->actionSearh->setEnabled(true);
             ui->actionConfigure->setEnabled(true);
         }
     });
     login->checkLogin();

     connect(butlogin,&QCommandLinkButton::clicked,this,[=]
     {
       //QRect rec = QApplication::desktop()->screenGeometry();
       //QPoint poz = rec.center();
       QPoint poz = this->pos();
       QSize logsize= login->size();
       QSize mainsize= this->size();
       poz.setX(poz.x()+mainsize.width()/2 - logsize.width()/2);
       poz.setY(poz.y()+mainsize.height()/2 - logsize.height()/2);
       login->move(poz);
       login->show();
     });
     ui->toolBar->addWidget(butlogin);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete tree_dock;
    delete browser_dock;
    delete list_dock;
    delete mdi_dock;
    delete libs;
    delete ModbusTimer;
    for( int i=0;i<vectorModbusDevice.count();i++)
    {
        vectorModbusDevice[i].modbusDev->disconnectDevice();
        vectorModbusDevice[i].modbusDev->deleteLater();
    }
    vectorModbusDevice.clear();
    tablListSavedDevices.clear();
    strListSavedDevices.clear();
    ui->mdiArea->closeAllSubWindows();

    delete login;
    delete ui;
}

void MainWindow::setIduser(QString str)
{
    idUser = str;
}

void MainWindow::writeSettings()
{
    static const char* const FILE_NAME = "settings.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window = saveGeometry();
    QByteArray set = saveState();
    file.open( QIODevice::WriteOnly );
    if (file.isOpen())
    {
        stream << set;
        stream << window;
        file.close();
    }
}


void MainWindow::DevicesSearch()
{
 ui->textBrowser->clear();
 ui->treeWidget->clear();

 ui->actionSearh->setText("Идет поиск...");
 ui->actionSearh->setDisabled(true);

 ProgressPercent.setValue(0);
 Progesslabel.show();
 ProgressPercent.show();

 QList<QMdiSubWindow *> list = ui->mdiArea->subWindowList(QMdiArea::CreationOrder);
 if(list.count()!=0)
 {
     ui->mdiArea->closeAllSubWindows();
     libs->CloseAll();
 }

 tablListFindDevices.clear();
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

 CountRequestAdr = infos.count()*LAST_MODBUS_ADRESS;
 CurentRequestAdr = 0;

 ModbusTimer->stop();
 ModbusTimer->setSingleShot(true);
 ModbusTimer->setInterval(infos.count()*(LAST_MODBUS_ADRESS*MODBUS_TIMEOUT_REPLY*(MODBUS_COUNT_REPEAT))*2.5);

 ModbusTimer->start();

 ui->treeWidget->setEnabled(false);
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

           connect(newCh.modbusDev, &QModbusClient::errorOccurred, [this](QModbusDevice::Error)
           {
                   QMessageBox::warning(this, "Модбас","Проверьте соединение с портом");
           });

           connect(newCh.modbusDev, &QModbusClient::stateChanged,this,[this](QModbusDevice::State)
           {

           });

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
         vectorModbusDevice[intcomModBusDevice].modbusDev->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud19200);
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
      ui->treeWidget->setEnabled(true);
      ui->actionSearh->setText("Поиск устройств");
      ui->actionSearh->setEnabled(true);
      ModbusTimer->stop();

      ProgressPercent.setValue(0);
      Progesslabel.hide();
      ProgressPercent.hide();
    }
}

void MainWindow::pollAdrModbus()
{

    if ( vectorModbusDevice[intcomModBusDevice].currentAdr !=LAST_MODBUS_ADRESS )
    {
        QModbusDataUnit request;
        request.setRegisterType(QModbusDataUnit::InputRegisters);
        request.setStartAddress(0);
        request.setValueCount(sizeof(union_tableRegsRead)/2);
        vectorModbusDevice[intcomModBusDevice].currentAdr++;

        CurentRequestAdr++;
        ProgressPercent.setValue(CurentRequestAdr*100/CountRequestAdr);
        Progesslabel.setText(vectorModbusDevice[intcomModBusDevice].nameCom+": Адрес:"+QString::number(vectorModbusDevice[intcomModBusDevice].currentAdr));

        if (auto *reply =  vectorModbusDevice[intcomModBusDevice].modbusDev->sendReadRequest(request,  vectorModbusDevice[intcomModBusDevice].currentAdr)) {
            if (!reply->isFinished())
            {                
                connect(reply, &QModbusReply::finished, this, &MainWindow::pollReplyModbus);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
            ui->textBrowser->append("Read error: " + vectorModbusDevice[intcomModBusDevice].modbusDev->errorString());
            ui->textBrowser->append("Проблема соединения");
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
                for(int i = unit.startAddress(), total  = int(unit.valueCount()); i < total ;i++) // переписываем ответ в локальную таблицу регистров
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

   struct_listSavedDevices FindDevice;
   memcpy(&FindDevice.device,&table,sizeof(FindDevice.device));
   FindDevice.devicename = nameconnect;
   FindDevice.modbusadr = QString::number( com.currentAdr );
   FindDevice.portname = com.nameCom;
   tablListFindDevices << FindDevice;


   QString str ="№ номер: "+ QString::number(table.Regs.SerialNum);
   QTreeWidgetItem *toplevel = new QTreeWidgetItem(ui->treeWidget);

   toplevel->setText(0,str);
   toplevel->setData(0,Qt::UserRole,strRole);

   QTreeWidgetItem *itemName=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemType=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemSerial=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemApp=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemDateConnect=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *itemLog=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *Protocol=new QTreeWidgetItem(toplevel);
   QTreeWidgetItem *FisicalPort=new QTreeWidgetItem(toplevel);

   itemName->setText(0,"Имя: "+nameconnect);
   itemName->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

   itemType->setText(0,"Тип: "+QString::number(table.Regs.TypeDevice));
   itemSerial->setText(0,"Серийный №: "+QString::number(table.Regs.SerialNum));
   itemApp->setText(0,"Аппаратная версия: "+QString::number(table.Regs.VerApp));

   itemDateConnect->setText(0,"Дата связи: "+QDateTime::fromTime_t(table.Regs.timeconnect).toString("yyyy-MM-dd  HH:mm:ss"));
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
          ui->actionSaved->setEnabled(true);
     }
    }
  }
}


QString MainWindow::tableToString(struct_listSavedDevices table_point)
{
  QString str = table_point.devicename +";"+table_point.portname +";"+table_point.modbusadr +";"
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
        ui->actionSaved->setDisabled(true);
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
            QIcon icon1;
            icon1.addFile(QString::fromUtf8(":/PNG/images/connect.png"), QSize(), QIcon::Normal, QIcon::Off);
            newconnect->setIcon(icon1);
            QAction *newdisconnect = new QAction("Закрыть");
            QIcon icon2;
            icon2.addFile(QString::fromUtf8(":/PNG/images/disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
            newdisconnect->setIcon(icon2);
            newdisconnect->setEnabled(false);

            QAction *newsave = new QAction("Сохранить");
            QAction *newSetting = new QAction("Свойства");
            QMenu menu(this);
            newconnect->setData(strloc);
            connect(newconnect,&QAction::triggered,this,&MainWindow::LoadLibDevice);
            newSetting->setData(strloc);
            connect(newSetting,&QAction::triggered,this,&MainWindow::ViewSettingsDevice);

            menu.addAction(newconnect);
            menu.addAction(newdisconnect);
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
              if( modbus!=nullptr )
              {
                  libs->setIdUser( idUser );
                  if ( libs->LibOpen(str,ui->mdiArea,modbus) )
                  {

                  }
              }
         }
     }
}

void MainWindow::ViewSettingsDevice()
{
     QAction* open = qobject_cast< QAction* >( sender() );
     QDialog *dialog = new QDialog(ui->mdiArea);
     QLabel *dataconect = new QLabel(dialog);
     QLabel *datachange = new QLabel(dialog);
     QLabel *idchange = new QLabel(dialog);
     QLabel *datafactory = new QLabel(dialog);
     QLabel *idfactory = new QLabel(dialog);
     QLabel *additFil = new QLabel(dialog);
     additFil->setText("Доп.поле:");
     QLineEdit *additLin = new QLineEdit(dialog);
     additLin->setEnabled(false);
     QGridLayout * grid = new QGridLayout(dialog);
     grid->addWidget(dataconect,0,0);
     grid->addWidget(datachange,1,0);
     grid->addWidget(idchange,2,0);
     grid->addWidget(datafactory,3,0);
     grid->addWidget(idfactory,4,0);
     grid->addWidget(additFil,5,0);
     grid->addWidget(additLin,6,0);

     auto data = open->data();
     if ( data.isValid() )
     {
         if ( data.type()==QVariant::String )
         {
              QString str = QString("%1").arg(data.toString());
              struct_listSavedDevices table = stringToTable(str);
              int poz;
              for( poz=0;poz<tablListFindDevices.count();poz++ )
              {
                  if( (tablListFindDevices[poz].portname == table.portname)
                       &&(tablListFindDevices[poz].device.Regs.SerialNum == table.device.Regs.SerialNum )
                       &&(tablListFindDevices[poz].device.Regs.TypeDevice == table.device.Regs.TypeDevice)
                       &&(tablListFindDevices[poz].device.Regs.VerApp == table.device.Regs.VerApp))
                  {
                      QString strfield;
                      if( tablListFindDevices[poz].device.Regs.timeconnect<UINT64_MAX && tablListFindDevices[poz].device.Regs.timeconnect>0)
                         strfield = QDateTime::fromTime_t(tablListFindDevices[poz].device.Regs.timeconnect).toString("yyyy-MM-dd  HH:mm:ss");
                      dataconect->setText("Дата связи: "+strfield);
                      strfield.clear();
                      if(tablListFindDevices[poz].device.Regs.timechange<UINT64_MAX)
                         strfield = QDateTime::fromTime_t(tablListFindDevices[poz].device.Regs.timechange).toString("yyyy-MM-dd  HH:mm:ss");
                      datachange->setText("Дата изменения настроек: "+strfield);
                      strfield.clear();
                      if(tablListFindDevices[poz].device.Regs.idchange<UINT32_MAX)
                          strfield = QString::number(tablListFindDevices[poz].device.Regs.idchange);
                      idchange->setText("Id изменившего: "+strfield);
                      strfield.clear();
                      if(tablListFindDevices[poz].device.Regs.timedefault<UINT64_MAX)
                          strfield = QDateTime::fromTime_t(tablListFindDevices[poz].device.Regs.timedefault).toString("yyyy-MM-dd  HH:mm:ss");
                      datafactory->setText("Дата установки: "+strfield);
                      strfield.clear();
                      if(tablListFindDevices[poz].device.Regs.iddefault<UINT32_MAX)
                          strfield = QString::number(tablListFindDevices[poz].device.Regs.iddefault);
                      idfactory->setText("Id изменившего: "+strfield);
                      strfield.clear();

                      QString str;
                      QByteArray aray;
                      for (int i = 0; i < static_cast<int>(sizeof(tablListFindDevices[poz].device.Regs.mas)); ++i)
                      {
                          aray.append((const char*)(tablListFindDevices[poz].device.Regs.mas + i), sizeof(uint16_t));
                      }

//                      for(int i = 0; i<static_cast<int>(sizeof(tablListFindDevices[poz].device.Regs.mas));i++)
//                      {
//                          if(i%2)
//                          str+=QString::number((tablListFindDevices[poz].device.Regs.mas[i]) & 0x00ff);
//                          else
//                          str+=QString::number((tablListFindDevices[poz].device.Regs.mas[i]) >> 8);
//                      }
                      for ( int i=0;i<static_cast<int>(sizeof(tablListFindDevices[poz].device.Regs.mas));i++ )
                      {
                            if(aray[i].operator!=(0xFF))
                            {
                                str = QString::fromLocal8Bit(aray);
                                additLin->setText(str);
                                break;
                            }
                      }
                      break;
                  }
              }
         }
     }
     dialog->show();
}
