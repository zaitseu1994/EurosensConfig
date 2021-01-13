#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

#include "QDockWidget"
#include "QSettings"

#include <QModbusRtuSerialMaster>
#include <QPushButton>
#include <QCommandLinkButton>

#include <QDesktopServices>
#include <QUrl>

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

#define WEB_CONFIRM_GET "ANSWER_GET_CONFIRM"
#define WEB_CONFIRM_SET "ANSWER_SET_CONFIRM"

//#define DEBUG_WEB
#define DEBUG_MWS

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ModbusTimer = new QTimer;
    statbar_NameD = new QLabel(this);
    statbar_Name= new QLabel(this);
    statbar_TypeD= new QLabel(this);
    statbar_Type= new QLabel(this);
    statbar_SerialD= new QLabel(this);
    statbar_Serial= new QLabel(this);
    statbar_AppD= new QLabel(this);
    statbar_App= new QLabel(this);
    statbar_LogD= new QLabel(this);
    statbar_Log= new QLabel(this);
    statbar_ProtcD= new QLabel(this);
    statbar_Protc= new QLabel(this);
    statbar_AdrD = new QLabel(this);
    statbar_Adr = new QLabel(this);
    statbar_PortD= new QLabel(this);
    statbar_Port= new QLabel(this);

    httpNetwork = new QNetworkAccessManager(this);

    connect(ModbusTimer,&QTimer::timeout,this,[this]()
            {
                ModbusTimer->stop();
                ui->actionSearh->setEnabled(true);
                ui->treeWidget->clear();
                ui->treeWidget->setEnabled(true);
                QString comname = vectorModbusDevice[intcomModBusDevice].nameCom;
                QMessageBox::warning(this, comname,tr("Проверьте соединение с портом"));
            });

    libs = new DeviceLibs();

    connect(libs,&DeviceLibs::closed,this,[=](struct_listSavedDevices table)
    {
        for( int i=0;i<tableDevices.count();i++ )
        {
             if ( tableDevices[i].table.Regs.SerialNum == table.device.Regs.SerialNum &&
                  tableDevices[i].table.Regs.TypeDevice == table.device.Regs.TypeDevice &&
                  tableDevices[i].table.Regs.VerApp == table.device.Regs.VerApp )
             {
                  tableDevices[i].isOpen =false;
             }
        }

    });
    connect(libs,&DeviceLibs::SettingsAccept,this,&MainWindow::sendSettingWeb);

    connect(libs,&DeviceLibs::DeviceToolTip,this,[=](QString str)
    {
            ui->listInfo->clear();
            ui->listInfo->append(str);
    });

#ifdef DEBUG_MWS
    connect(libs,&DeviceLibs::DeviceErrorString,this,[=](QString str)
    {
         ui->textBrowser->append(str);
    });
#endif

    QMenu *menu_view = new QMenu(this);
    menu_view->setTitle(tr("Вид"));

    tree_dock = new QDockWidget(tr("Устройства"), this);
    tree_dock->setObjectName("tree");
    tree_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    tree_dock->setWidget(ui->treeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, tree_dock);
    menu_view->addAction(tree_dock->toggleViewAction());

    browser_dock = new QDockWidget(tr("Состояние"), this);
    browser_dock->setObjectName("browser");
    browser_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    browser_dock->setWidget(ui->textBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, browser_dock);
    menu_view->addAction(browser_dock->toggleViewAction());

    list_dock = new QDockWidget(tr("Инфо"), this);
    list_dock->setObjectName("list");
    list_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
    list_dock->setWidget(ui->listInfo);
    addDockWidget(Qt::RightDockWidgetArea, list_dock);
    menu_view->addAction(list_dock->toggleViewAction());

    ui->menuSettings->addMenu(menu_view);

    connect(ui->actionSearh,&QAction::triggered,this,&MainWindow::DevicesSearch);
    connect(ui->actionCancel,&QAction::triggered,this,&MainWindow::DevicesSearchDisable);

    connect(ui->actionMechatronics_help,&QAction::triggered,this,[=]
    {
        QString link="https://ru.eurosenstelematics.com";
        QDesktopServices::openUrl(QUrl(link));
    });

    connect(ui->actionContacts,&QAction::triggered,this,[=]
    {
        QString link="mailto:zaitseu1994@gmail.com";
        QDesktopServices::openUrl(QUrl(link));
    });

    ui->treeWidget->setHeaderLabel(" ");
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);

    statbar_NameD->setText(tr("Имя:"));
    statbar_TypeD->setText(tr("Тип:"));
    statbar_SerialD->setText(tr("Серийный:"));
    statbar_AppD->setText(tr("Аппаратная версия:"));
    statbar_LogD->setText(tr("Лог:"));
    statbar_ProtcD->setText(tr("Протокол:"));
    statbar_AdrD->setText(tr("Адрес:"));
    statbar_PortD->setText(tr("Порт:"));

    statbar_NameD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_TypeD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_SerialD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_AppD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_LogD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_ProtcD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_AdrD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_PortD->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));

    statbar_Name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Type->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Serial->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_App->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Log->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Protc->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Adr->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Port->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));

    ui->statusbar->addWidget(statbar_NameD);
    ui->statusbar->addWidget(statbar_Name);
    ui->statusbar->addWidget(statbar_TypeD);
    ui->statusbar->addWidget(statbar_Type);
    ui->statusbar->addWidget(statbar_SerialD);
    ui->statusbar->addWidget(statbar_Serial);
    ui->statusbar->addWidget(statbar_AppD);
    ui->statusbar->addWidget(statbar_App);
    ui->statusbar->addWidget(statbar_LogD);
    ui->statusbar->addWidget(statbar_Log);
    ui->statusbar->addWidget(statbar_ProtcD);
    ui->statusbar->addWidget(statbar_Protc);
    ui->statusbar->addWidget(statbar_AdrD);
    ui->statusbar->addWidget(statbar_Adr);
    ui->statusbar->addWidget(statbar_PortD);
    ui->statusbar->addWidget(statbar_Port);

     login = new Login(this);
     butlogin = new QCommandLinkButton(this);
     butlogin->setText(tr("Нажмите чтоб войти"));

     ui->devices->setEnabled(false);
     ui->actionSearh->setEnabled(false);
     ui->actionCancel->setEnabled(false);
     ui->actionConfigure->setEnabled(false);
     ui->actionUpdate->setEnabled(false);

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
             butlogin->setText(tr("Сменить пользователя")+ "( ID "+idUser+" )");
             login->close();
             ui->devices->setEnabled(true);
             ui->actionSearh->setEnabled(true);
             ui->actionCancel->setEnabled(true);
             ui->actionConfigure->setEnabled(true);
         }
     });
     login->checkLogin();

     connect(butlogin,&QCommandLinkButton::clicked,this,[=]
     {
       QPoint poz = this->pos();
       QSize logsize= login->size();
       QSize mainsize= this->size();
       poz.setX(poz.x()+mainsize.width()/2 - logsize.width()/2);
       poz.setY(poz.y()+mainsize.height()/2 - logsize.height()/2);
       login->move(poz);
       login->show();
     });
     ui->toolBar->addWidget(butlogin);

     ui->devices->setEnabled(false);
     connect(ui->actionHistory,&QAction::triggered,this,&MainWindow::ViewSettingsDevice);
     connect(ui->actionAditional,&QAction::triggered,this,&MainWindow::additionalChange);
     connect(ui->actionSaved,&QAction::triggered,this,&MainWindow::actionSaved);
     connect(ui->actionWebSettings,&QAction::triggered,this,&MainWindow::webSettingsChange);

     connect(ui->actionlangEN,&QAction::triggered,this,&MainWindow::SetLanguage);
     connect(ui->actionlangRU,&QAction::triggered,this,&MainWindow::SetLanguage);

     restoreLanguage();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);    // переведём окно заново
    }
}

void MainWindow::SetLanguage()
{
     QIcon icon1;
     QAction* actionlang = qobject_cast< QAction* >( sender() );
     QString lang = actionlang->text();
     if( lang == "RU"  )
     {
         qtLanguageTranslator.load(QString("QtLanguage_") + QString("ru_RU"));
         qApp->installTranslator(&qtLanguageTranslator);
         icon1.addFile(QString::fromUtf8(":/PNG/images/langRU.png"), QSize(), QIcon::Normal, QIcon::Off);
         curLang = "RU";
     }
     if( lang == "EN"  )
     {
         qtLanguageTranslator.load(QString("QtLanguage_") + QString("en_US"));
         qApp->installTranslator(&qtLanguageTranslator);
         icon1.addFile(QString::fromUtf8(":/PNG/images/langEN.png"), QSize(), QIcon::Normal, QIcon::Off);
         curLang = "EN";
     }
     ui->actionCurlang->setIcon(icon1);
}

MainWindow::~MainWindow()
{
    saveLanguage();
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
    tableDevices.clear();
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

void MainWindow::saveLanguage()
{
    static const char* const FILE_NAME = "language.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window = curLang.toLocal8Bit();
    file.open( QIODevice::WriteOnly );
    if (file.isOpen())
    {
        stream << window;
        file.close();
    }
}

void MainWindow::restoreLanguage()
{
    static const char* const FILE_NAME = "language.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window;
    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
        stream >> window;
        file.close();
    }
    QString str = QString::fromLocal8Bit(window);
    if( str == "RU" )
    {
        ui->actionlangRU->trigger();
    }
    if ( str == "EN" )
    {
        ui->actionlangEN->trigger();
    }
}


void MainWindow::DevicesSearch()
{
 ui->textBrowser->clear();
 ui->treeWidget->clear();

 ui->actionSearh->setText(tr("Идет поиск..."));
 ui->actionSearh->setDisabled(true);

 QList<QMdiSubWindow *> list = ui->mdiArea->subWindowList(QMdiArea::CreationOrder);
 if(list.count()!=0)
 {
     ui->mdiArea->closeAllSubWindows();
     libs->CloseAll();
 }

 statbar_Name->setText(" ");
 statbar_Type->setText(" ");
 statbar_Serial->setText(" ");
 statbar_App->setText(" ");
 statbar_Log->setText(" ");
 statbar_Protc->setText(" ");
 statbar_Adr->setText(" ");
 statbar_Port->setText(" ");

 ui->devices->setEnabled(false);

 tablListSavedDevices.clear();
 strListSavedDevices.clear();
 tableDevices.clear();
 numDevice = 0;
 DevicesRead();

 const auto infos = QSerialPortInfo::availablePorts();
 QString avlPorts;
 for ( const QSerialPortInfo &info : infos ) {
       avlPorts +=info.portName();
       avlPorts +=";";
 };
 if ( infos.count()>0  )
 {
     ui->treeWidget->setHeaderLabel(tr("Найденные устройства")+"...");
     textCursor = ui->textBrowser->textCursor().position();
     ui->textBrowser->append(infos[0].portName()+":...");
     ui->textBrowser->append(tr("Найденые порты")+": "+avlPorts);
 } else
 {
     ui->treeWidget->setHeaderLabel(" ");
     ui->textBrowser->append(tr("Портов не найдено")+"..."+avlPorts);
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
           if( newCh.modbusDev!=nullptr )
           {
               connect(newCh.modbusDev, &QModbusClient::errorOccurred, [this](QModbusDevice::Error)
               {
                       QMessageBox::warning(this, tr("Модбас"),tr("Проверьте соединение с портом"));
               });

               connect(newCh.modbusDev, &QModbusClient::stateChanged,this,[=](QModbusDevice::State)
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

           }else
           {
               QMessageBox::warning(this, tr("Модбас"),tr("Проблема heap"));
           }
    }
}

void MainWindow::pollModbus()
{
    if( intcomModBusDevice<vectorModbusDevice.count() )
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
              ui->textBrowser->append(tr("Соединение с портом ") + vectorModbusDevice[intcomModBusDevice].nameCom +tr(" установленно..."));
              pollAdrModbus();
         }

    }else
    {
      ui->treeWidget->setEnabled(true);
      ui->actionSearh->setEnabled(true);
      ModbusTimer->stop();
      ui->textBrowser->append(tr("Поиск завершен... "));
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

        QTextCursor cur = ui->textBrowser->textCursor();
        cur.setPosition(textCursor);
        cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
        cur.select(QTextCursor::LineUnderCursor);
        cur.removeSelectedText();
        cur.setPosition(textCursor);
        cur.insertText(vectorModbusDevice[intcomModBusDevice].nameCom+tr(": Адрес:")+QString::number(vectorModbusDevice[intcomModBusDevice].currentAdr) +
                       " ..."+ QString::number(CurentRequestAdr*100/CountRequestAdr)+"%");

        if (auto *reply =  vectorModbusDevice[intcomModBusDevice].modbusDev->sendReadRequest(request,  vectorModbusDevice[intcomModBusDevice].currentAdr)) {
            if (!reply->isFinished())
            {                
                connect(reply, &QModbusReply::finished, this, &MainWindow::pollReplyModbus);
            }
            else
                delete reply; // broadcast replies return immediately
        } else {
            ui->textBrowser->append(tr("Ошибка чтения")+": " + vectorModbusDevice[intcomModBusDevice].modbusDev->errorString());
            ui->textBrowser->append(tr("Проблема соединения"));
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
                ui->textBrowser->append(tr("Найдено устройство")+"; "+tr("Протокол")+": "+tr("MODBUS-rtu")+"; "+tr("Адрес")+":"+ QString::number(replyModbus->serverAddress()));
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
                 ui->textBrowser->append(tr("Закончен опрос порта")+": "+ vectorModbusDevice[intcomModBusDevice].nameCom);
            }
        }
        replyModbus->deleteLater();
        pollAdrModbus();
}

void MainWindow::DevicesSearchDisable()
{
    bool flag = false;

    for( int i=0;i<vectorModbusDevice.count();i++ )
    {
         if( vectorModbusDevice[i].currentAdr < LAST_MODBUS_ADRESS )
         {
             flag = true;
             break;
         }
    }
    if ( flag )
    {
        for( int i=0;i<vectorModbusDevice.count();i++ )
        {
            vectorModbusDevice[i].currentAdr =  LAST_MODBUS_ADRESS;

        }
        ui->textBrowser->append(tr("Поиск прерван пользователем")+"...");
    }
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

void MainWindow::setNameDevice(QString name,int numdev)
{
    bool nameBool = false;
    for ( int i=0;i<tablListSavedDevices.count();i++ )
    {
          if ( (tablListSavedDevices[i].device.Regs.SerialNum == tableDevices[numdev].table.Regs.SerialNum) &&

               (tablListSavedDevices[i].device.Regs.TypeDevice == tableDevices[numdev].table.Regs.TypeDevice) &&

               (tablListSavedDevices[i].device.Regs.VerApp == tableDevices[numdev].table.Regs.VerApp)  )
          {
                tablListSavedDevices[i].devicename = name;
                nameBool  = true;
                break;
          }
    }
    if ( !nameBool )
    {
        struct_listSavedDevices newDev;
        memcpy(&newDev.device,&tableDevices[numdev].table.Regs,sizeof(newDev.device));
        newDev.devicename = name;
        tablListSavedDevices << newDev;
    }

    for( int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i )
     {
         QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
         auto strloc = item->data(0,Qt::UserRole);
         if ( strloc.isValid() )
         {
          if ( strloc.type() == QVariant::String )
          {
               int num = strloc.toInt();

               if ( num == numdev )
               {
                    QString str = QString::number(num+1)+") №: "+ QString::number(tableDevices[numdev].table.Regs.SerialNum)+" ; "+ name;
                    item->setText(0,str);
                    break;
               }
          }
         }

     }
}

void MainWindow::getDeviceModbus(union_tableRegsRead table, struct_ComModbus com,QString nameconnect)
{
   disconnect(ui->treeWidget,&QTreeWidget::itemPressed,this,&MainWindow::treeItemPress);

   QString str = QString::number(numDevice+1)+") №: "+ QString::number(table.Regs.SerialNum)+" ; "+ nameconnect;

   QTreeWidgetItem *toplevel = new QTreeWidgetItem(ui->treeWidget);

   toplevel->setText(0,str);

   if( table.Regs.idset == MAXWORD )
   {
       table.Regs.idset = 0;
   }
   if(table.Regs.timechange == MAXDWORD)
   {
       table.Regs.timechange = 0;
   }

   struct_devices inDevice;
   inDevice.com.currentAdr = com.currentAdr;
   inDevice.com.description = com.description;
   inDevice.com.isFind = com.isFind;
   inDevice.com.manufacturer = com.manufacturer;
   inDevice.com.modbusDev = com.modbusDev;
   inDevice.com.nameCom = com.nameCom;
   inDevice.com.productIdentifier = com.productIdentifier;
   inDevice.com.vendorIdentifier = com.vendorIdentifier;

   memcpy(&inDevice.table,&table,sizeof(inDevice.table));
   inDevice.devicename = nameconnect;
   tableDevices << inDevice;
   checkSettingWeb(numDevice);
   toplevel->setData(0,Qt::UserRole,QString::number(numDevice));
   numDevice++;

   connect(ui->treeWidget,&QTreeWidget::itemPressed,this,&MainWindow::treeItemPress);
}

void MainWindow::treeItemPress(QTreeWidgetItem * item, int column)
{
    if ( item!=NULL )
    {
       auto strloc = item->data(column,Qt::UserRole);
       if ( strloc.isValid() )
       {
        if ( strloc.type() == QVariant::String )
        {
             int numDev = strloc.toInt();
             if ( tableDevices.count()>= numDev )
             {
                 ui->devices->setEnabled(true);
                 selectedDevices.clear();
                 for( int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i )
                 {
                    QTreeWidgetItem *it = ui->treeWidget->topLevelItem( i );
                    if( it->isSelected()  )
                    {
                        auto strl = it->data(column,Qt::UserRole);
                        if ( strl.isValid() )
                        {
                             if ( strl.type() == QVariant::String )
                             {
                                  int num = strl.toInt();
                                  selectedDevices << num;
                             }
                        }
                    }
                 }

                 statbar_Name->setText(tableDevices[numDev].devicename);
                 statbar_Type->setText(QString::number(tableDevices[numDev].table.Regs.TypeDevice));
                 statbar_Serial->setText(QString::number(tableDevices[numDev].table.Regs.SerialNum));
                 statbar_App->setText(QString::number(tableDevices[numDev].table.Regs.VerApp));
                 statbar_Log->setText(QString::number(tableDevices[numDev].table.Regs.LogError));
                 statbar_Protc->setText("MODBUS");
                 statbar_Adr->setText(QString::number(tableDevices[numDev].com.currentAdr));
                 statbar_Port->setText(tableDevices[numDev].com.nameCom);
             }
        }
      }
    }
}


void MainWindow::webSettingsChange()
{
    QDialog *dialog = new QDialog(ui->mdiArea);

    QLabel *lab_WebLinkNameCurAdr = new QLabel(tr("Текущий адрес:"));
    QLabel *lab_WebLinkAdrCur = new QLabel(webserver);

    QLabel *lab_WebChangeNameAdr = new QLabel(tr("Новый:"));
    QLineEdit *lin_WebChangeAdr = new QLineEdit();
    QPushButton *butChange = new QPushButton("Сменить");

    connect(butChange,&QPushButton::clicked,this,[=]
    {
       if( lin_WebChangeAdr->text().length()>=1 )
       {
           webserver = lin_WebChangeAdr->text();
           lab_WebLinkAdrCur->setText(lin_WebChangeAdr->text());
           lin_WebChangeAdr->clear();
           dialog->close();
       }
    });

    QGridLayout * grid = new QGridLayout(dialog);

    grid->addWidget(lab_WebLinkNameCurAdr,0,1);
    grid->addWidget(lab_WebLinkAdrCur,0,2);
    grid->addWidget(lab_WebChangeNameAdr,1,1);
    grid->addWidget(lin_WebChangeAdr,1,2);
    grid->addWidget(butChange,1,3);

    dialog->setBaseSize(dialog->width(),dialog->height()/2);
    dialog->show();
}

void MainWindow::additionalChange()
{
    QDialog *dialog = new QDialog(ui->mdiArea);

    QComboBox *devcombo = new QComboBox();

    for(int i=0;i<selectedDevices.count();i++)
    {
        devcombo->addItem(QString::number(selectedDevices[i]+1)+") Serial: "+QString::number(tableDevices[i].table.Regs.SerialNum)+"; "+tr("Имя")+": "+tableDevices[i].devicename);
    }
    devcombo->setCurrentIndex(0);

    QLabel *labname = new QLabel(tr("Изменить имя"));
    QLineEdit *linname = new QLineEdit();
    QPushButton *butname = new QPushButton(tr("Сменить"));
    connect(butname,&QPushButton::clicked,this,[=]
    {
       if( linname->text().length()>=1 )
       {
           QStringList strli = devcombo->currentText().split(") Serial:",Qt::SkipEmptyParts);
           int devnum = strli[0].toInt()-1;
           tableDevices[devnum].devicename = linname->text();
           setNameDevice(linname->text(),devnum);

           devcombo->setItemText(devcombo->currentIndex(),QString::number(devnum+1)+") Serial: "+QString::number(tableDevices[devnum].table.Regs.SerialNum)+"; "+tr("Имя")+": "+tableDevices[devnum].devicename);
           linname->clear();

           DevicesSaved();
       }
    });

    QGroupBox * groupinterf = new QGroupBox();
    QListWidget *listinterf = new QListWidget();

    listinterf->setMinimumSize(QSize(1, 1));

    QVBoxLayout *vboxI = new QVBoxLayout();

    vboxI->addWidget(listinterf);

    groupinterf->setLayout(vboxI);
    groupinterf->setTitle(tr("Интерфейс"));

    QGridLayout * grid = new QGridLayout(dialog);
    grid->addWidget(devcombo,0,0);
    grid->addWidget(labname,0,1);
    grid->addWidget(linname,0,2);
    grid->addWidget(butname,0,3);

    grid->addWidget(groupinterf,1,0,1,4);

    for( int i=0;i<selectedDevices.count();i++ )
    {
         int row = selectedDevices[i];
         QListWidgetItem *item = new QListWidgetItem();
         item->setText("Device: "+QString::number(tableDevices[row].table.Regs.SerialNum)+":");
         item->setForeground(Qt::red);
         item->setBackground(Qt::cyan);
         listinterf->addItem(item);

         listinterf->addItem(" - Serial port: "+tableDevices[row].com.nameCom);
         listinterf->addItem(" - Description: "+tableDevices[row].com.description);
         listinterf->addItem(" - Manufacturer: "+tableDevices[row].com.manufacturer);
         listinterf->addItem(" - ProdId: "+QString::number(tableDevices[row].com.productIdentifier));
         listinterf->addItem(" - VendId: "+QString::number(tableDevices[row].com.vendorIdentifier));

    }
    listinterf->setMinimumWidth(listinterf->sizeHintForColumn(0));

    dialog->setBaseSize(dialog->width(),dialog->height()/2);
    dialog->show();
}

void MainWindow::actionSaved()
{
      QDialog *dialog = new QDialog(ui->mdiArea);

      QTableWidget *tableWidget = new QTableWidget(dialog);

      int rowCount = selectedDevices.count()*3;
      int columnCount = 8;

      QTableWidgetItem *itemHead[columnCount];

      itemHead[0] = new QTableWidgetItem(tr("Источник"));
      itemHead[1] = new QTableWidgetItem(tr("Серийный"));
      itemHead[2] = new QTableWidgetItem(tr("Тип"));
      itemHead[3] = new QTableWidgetItem(tr("Версия"));
      itemHead[4] = new QTableWidgetItem(tr("Дата сохранения"));
      itemHead[5] = new QTableWidgetItem(tr("Id пользователя"));
      itemHead[6] = new QTableWidgetItem(tr("Файл"));
      itemHead[7] = new QTableWidgetItem(tr("Действие"));

      tableWidget->setColumnCount(columnCount);
      tableWidget->setRowCount(rowCount);

      for(int i=0;i<columnCount;i++)
      {
           tableWidget->setHorizontalHeaderItem(i,itemHead[i]);
      }

      for(int kol=0; kol!=selectedDevices.count(); ++kol){

          QTableWidgetItem *saveItem[columnCount];
          QTableWidgetItem *loadItem[columnCount];
          QTableWidgetItem *webItem[columnCount];

          for( int i=0;i<columnCount-1;i++ )
          {
               saveItem[i] = new QTableWidgetItem();
               loadItem[i] = new QTableWidgetItem();
               webItem[i] = new QTableWidgetItem();
          }
          int numdev = selectedDevices[kol];

          saveItem[0]->setText(tr("Текущие настройки"));
          saveItem[1]->setText(QString::number(tableDevices[numdev].table.Regs.SerialNum));
          saveItem[2]->setText(QString::number(tableDevices[numdev].table.Regs.TypeDevice));
          saveItem[3]->setText(QString::number(tableDevices[numdev].table.Regs.VerApp));

          QDateTime actualTime = QDateTime::fromTime_t(tableDevices[numdev].table.Regs.timechange,Qt::UTC,0);
          saveItem[4]->setText(actualTime.toString(timeformat));

          QString strSet = "Null";
          if( tableDevices[numdev].table.Regs.idset!=0 )
          strSet = QString::number(tableDevices[numdev].table.Regs.idset);
          saveItem[5]->setText(strSet);

          QTableWidgetItem *nameFile = saveItem[6];
          QPushButton *buttonSave = new QPushButton(tr("Сохранить"));
          connect(buttonSave,&QPushButton::clicked,this,[=]
          {
                  QString str = butSave(numdev);
                  if ( str.length()>=1 )
                  {
                       nameFile->setText(str);
                  }
          });

          loadItem[0]->setText(tr("Из файла"));

          QPushButton *buttonLoad = new QPushButton(tr("Загрузить файл"));
          QPushButton *buttonAccept = new QPushButton(tr("Загрузить"));
          buttonAccept->setEnabled(false);
          QWidget *widgetLoad = new QWidget(tableWidget);
          QGridLayout * gridLoad = new QGridLayout(widgetLoad);
          gridLoad->addWidget(buttonLoad,0,0);
          gridLoad->addWidget(buttonAccept,0,1);
          gridLoad->setSpacing(0);
          gridLoad->setContentsMargins(0,0,0,0);

          QTableWidgetItem *nameloadFile = loadItem[6];
          QTableWidgetItem *idloadFile = loadItem[5];
          QTableWidgetItem *dateloadFile = loadItem[4];
          QTableWidgetItem *verloadFile = loadItem[3];
          QTableWidgetItem *typeloadFile = loadItem[2];
          QTableWidgetItem *serialloadFile = loadItem[1];

          connect( buttonLoad,&QPushButton::clicked,this,[=]
          {
                 struct_filejsonload filejson = chooseFile(numdev,&fileJsonLoad);
                 if(  filejson.filename>=1 )
                 {
                      nameloadFile->setText(filejson.filename);
                      idloadFile->setText(QString::number(filejson.ID));
                      dateloadFile->setText(filejson.actualTime.toString(timeformat));
                      verloadFile->setText(QString::number(filejson.VerApp));
                      typeloadFile->setText(QString::number(filejson.TypeDevice));
                      serialloadFile->setText(QString::number(filejson.SerialNum));
                      buttonAccept->setEnabled(true);
                      tableWidget->resizeColumnsToContents();
                      tableWidget->resizeRowsToContents();
                      int wsize = 0;
                      for ( int i=0;i<columnCount;i++ )
                      {
                           wsize += tableWidget->columnWidth(i);
                      }
                      dialog->setFixedWidth( wsize+40 );
                 }
          });

          connect(buttonAccept,&QPushButton::clicked,this,[=]
          {
              if ( tableDevices[numdev].isOpen )
              {
                       QJsonObject textObject = tableDevices[numdev].jsonFileDoc.object();
                       QJsonObject jsonObject;
                       jsonObject = textObject.value("Setting").toObject();

                       struct_listSavedDevices table;
                       memcpy(&table.device,&tableDevices[numdev].table,sizeof(table.device));
                       table.devicename = tableDevices[numdev].devicename;
                       table.modbusadr = QString::number(tableDevices[numdev].com.currentAdr);
                       table.portname = tableDevices[numdev].com.nameCom;
                       QString idset  = idloadFile->text();

                       QDateTime ofsettime = QDateTime::fromString(dateloadFile->text(),timeformat);
                       QString timeset = QString::number(ofsettime.addSecs(ofsettime.offsetFromUtc()).toTime_t());

                       if ( libs->devStatus(table) == DeviceLibs::DEV_READY )
                       {

                           if ( libs->setSetting( table,jsonObject,idset,timeset ) )
                           {
                                QMessageBox::information(this, tableDevices[numdev].devicename,tr("Настройки применены"));

                                tableDevices[numdev].table.Regs.idset = idset.toUInt();
                                tableDevices[numdev].table.Regs.timechange = timeset.toULongLong();

                                nameloadFile->setText("");
                                idloadFile->setText("");
                                dateloadFile->setText("");
                                verloadFile->setText("");
                                typeloadFile->setText("");
                                serialloadFile->setText("");
                                buttonAccept->setEnabled(false);
                                tableWidget->resizeColumnsToContents();
                                tableWidget->resizeRowsToContents();
                                int wsize = 0;
                                for ( int i=0;i<columnCount;i++ )
                                {
                                     wsize += tableWidget->columnWidth(i);
                                }
                                dialog->setFixedWidth(wsize+40);
                                dialog->close();
                           }else
                           {
                              QMessageBox::information(this, tableDevices[numdev].devicename,tr("Ошибка с приминением настроек"));
                           }
                       }else
                       {
                           QMessageBox::information(this, tableDevices[numdev].devicename,tr("Программа считывает настройки"));
                       }
              }else
              {
                QMessageBox::information(this, tableDevices[numdev].devicename,tr("Необходимо открыть устройство"));
              }
          });

          webItem[0]->setText(tr("Веб сервер"));

          QPushButton *webLoad = new QPushButton(tr("Проверить"));
          QPushButton *webAccept = new QPushButton(tr("Загрузить"));
          webAccept->setEnabled(false);
          QWidget *widgetWeb = new QWidget(tableWidget);
          QGridLayout * gridWeb = new QGridLayout(widgetWeb);
          gridWeb->addWidget(webLoad,0,0);
          gridWeb->addWidget(webAccept,0,1);
          gridWeb->setSpacing(0);
          gridWeb->setContentsMargins(0,0,0,0);
          widgetWeb->setContentsMargins(0,0,0,0);

          QTableWidgetItem *namewebFile = webItem[6];
          QTableWidgetItem *idwebFile = webItem[5];
          QTableWidgetItem *datewebFile = webItem[4];
          QTableWidgetItem *verwebFile = webItem[3];
          QTableWidgetItem *typewebFile = webItem[2];
          QTableWidgetItem *serialwebFile = webItem[1];

          if( tableDevices[numdev].SetIsEnable )
          {
              struct_filejsonload webjson = verifyWebJson(WEB_CONFIRM_GET,&tableDevices[numdev].jsonWebDoc);

              namewebFile->setText(webjson.filename);
              idwebFile->setText(QString::number(webjson.ID));
              datewebFile->setText(webjson.actualTime.toString(timeformat));
              verwebFile->setText(QString::number(webjson.VerApp));
              typewebFile->setText(QString::number(webjson.TypeDevice));
              serialwebFile->setText(QString::number(webjson.SerialNum));

              if ( webjson.SerialNum == tableDevices[numdev].table.Regs.SerialNum &&
                   webjson.TypeDevice == tableDevices[numdev].table.Regs.TypeDevice &&
                   webjson.VerApp == tableDevices[numdev].table.Regs.VerApp )
              {
                   webAccept->setEnabled(true);
                   namewebFile->setText(tr("Доступно")+"!");
              }else
              {
                   namewebFile->setText(tr("Не найдено"));
              }
              tableWidget->resizeColumnsToContents();
              tableWidget->resizeRowsToContents();
              int wsize = 0;
              for (int i=0;i<columnCount;i++)
              {
                  wsize += tableWidget->columnWidth(i);
              }
              dialog->setFixedWidth(wsize+40);
          }

          connect(webLoad,&QPushButton::clicked,this,[=]
          {
              webLoad->setEnabled(false);
              const QUrl url(webserver);
              QNetworkRequest request(url);
              request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

              QJsonObject obj;
              obj["REQUEST"] = "GET";
              obj["Serial"] = QString::number(tableDevices[numdev].table.Regs.SerialNum);
              obj["Type"] = QString::number(tableDevices[numdev].table.Regs.TypeDevice);
              obj["VerAp"] = QString::number(tableDevices[numdev].table.Regs.VerApp);
              QDateTime actualTime = QDateTime::fromTime_t(tableDevices[numdev].table.Regs.timechange,Qt::UTC,0);
              obj["Date"] = actualTime.toString(timeformat);
              obj["Id"] = idUser;

              QJsonDocument doc(obj);
              QByteArray data = doc.toJson();

              QNetworkReply *reply = httpNetwork->post(request, data);

#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("RED");
    ui->textBrowser->append(data);
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif
              connect(reply, &QNetworkReply::finished, [=](){
                  if ( reply->error() == QNetworkReply::NoError ) {
                        tableDevices[numdev].jsonWebDoc = QJsonDocument::fromJson(reply->readAll());
                        struct_filejsonload webjson = verifyWebJson(WEB_CONFIRM_GET,&tableDevices[numdev].jsonWebDoc);


                        namewebFile->setText(webjson.filename);
                        idwebFile->setText(QString::number(webjson.ID));
                        datewebFile->setText(webjson.actualTime.toString(timeformat));
                        verwebFile->setText(QString::number(webjson.VerApp));
                        typewebFile->setText(QString::number(webjson.TypeDevice));
                        serialwebFile->setText(QString::number(webjson.SerialNum));

                        if ( webjson.SerialNum == tableDevices[numdev].table.Regs.SerialNum &&
                             webjson.TypeDevice == tableDevices[numdev].table.Regs.TypeDevice &&
                             webjson.VerApp == tableDevices[numdev].table.Regs.VerApp )
                        {
                             webAccept->setEnabled(true);
                             namewebFile->setText(tr("Доступно")+"!");
                        }else
                        {
                             namewebFile->setText(tr("Не найдено"));
                        }
#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("BLUE");
    ui->textBrowser->append(tableDevices[numdev].jsonWebDoc.toJson());
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif
                        tableWidget->resizeColumnsToContents();
                        tableWidget->resizeRowsToContents();
                        int wsize = 0;
                        for (int i=0;i<columnCount;i++)
                        {
                            wsize += tableWidget->columnWidth(i);
                        }
                        dialog->setFixedWidth(wsize+40);
                  }
                  else
                  {
                      QString err = reply->errorString();
                      ui->textBrowser->append("Web: "+err);
                  }
                  reply->deleteLater();
                  webLoad->setEnabled(true);
              });

          });

          connect(webAccept,&QPushButton::clicked,this,[=]
          {
              if ( tableDevices[numdev].isOpen )
              {
                  QJsonObject textObject = tableDevices[numdev].jsonWebDoc.object();
                  QJsonObject jsonObject;
                  jsonObject = textObject.value("Setting").toObject();

                  struct_listSavedDevices table;
                  memcpy(&table.device,&tableDevices[numdev].table,sizeof(table.device));
                  table.devicename = tableDevices[numdev].devicename;
                  table.modbusadr = QString::number(tableDevices[numdev].com.currentAdr);
                  table.portname = tableDevices[numdev].com.nameCom;

                  QString idset  = idwebFile->text();
                  QDateTime ofsettime = QDateTime::fromString(datewebFile->text(),timeformat);
                  QString timeset = QString::number(ofsettime.addSecs(ofsettime.offsetFromUtc()).toTime_t());

                  if ( libs->devStatus(table) == DeviceLibs::DEV_READY )
                  {
                      if ( libs->setSetting( table,jsonObject,idset,timeset ) )
                      {
                           QMessageBox::information(this, tableDevices[numdev].devicename,tr("Настройки применены"));

                           tableDevices[numdev].table.Regs.idset = idset.toUInt();
                           tableDevices[numdev].table.Regs.timechange = timeset.toULongLong();

                           namewebFile->setText("");
                           idwebFile->setText("");
                           datewebFile->setText("");
                           verwebFile->setText("");
                           typewebFile->setText("");
                           serialwebFile->setText("");
                           webAccept->setEnabled(false);
                           tableWidget->resizeColumnsToContents();
                           tableWidget->resizeRowsToContents();
                           int wsize = 0;
                           for ( int i=0;i<columnCount;i++ )
                           {
                                wsize += tableWidget->columnWidth(i);
                           }
                           dialog->setFixedWidth(wsize+40);
                           dialog->close();
                      }else
                      {
                         QMessageBox::information(this, tableDevices[numdev].devicename,tr("Ошибка с приминением настроек"));
                      }
                  }else
                  {
                      QMessageBox::information(this, tableDevices[numdev].devicename,tr("Программа считывает настройки"));
                  }

              } else
              {
                  QMessageBox::information(this, tableDevices[numdev].devicename,tr("Необходимо открыть устройство, программа считает настройки автоматически"));
              }

          });

          int pozItemSave = kol*3;
          int pozItemLoad = pozItemSave+1;
          int pozItemWeb =  pozItemSave+2;

          for( int i=0;i<columnCount-1;i++ )
          {
               tableWidget->setItem(pozItemSave, i, saveItem[i]);
               tableWidget->setItem(pozItemLoad, i, loadItem[i]);
               tableWidget->setItem(pozItemWeb, i, webItem[i]);
               saveItem[i]->setFlags(Qt::NoItemFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
               loadItem[i]->setFlags(Qt::NoItemFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
               webItem[i]->setFlags(Qt::NoItemFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
          }

          tableWidget->setCellWidget(pozItemSave,columnCount-1,buttonSave);
          tableWidget->setCellWidget(pozItemLoad,columnCount-1,widgetLoad);
          tableWidget->setCellWidget(pozItemWeb,columnCount-1,widgetWeb);
      }

      tableWidget->horizontalHeader()->setStretchLastSection(true);
      tableWidget->resizeColumnsToContents();
      tableWidget->resizeRowsToContents();

      QGridLayout * grid = new QGridLayout(dialog);
      grid->addWidget(tableWidget,0,0);

      int wsize = 0;
      for (int i=0;i<columnCount;i++)
      {
          wsize += tableWidget->columnWidth(i);
      }

      QPoint poz = this->pos();
      QSize mainsize= ui->mdiArea->size();
      QSize mainui= this->size();
      dialog->setFixedWidth(wsize+40);

      QSize logsize= dialog->size();

      poz.setX(poz.x()+mainui.width()/2 - logsize.width()/2);
      poz.setY(poz.y()+mainsize.height()/2 - logsize.height()/2);
      dialog->move(poz);

      dialog->show();
}

void MainWindow::sendSettingWeb(struct_listSavedDevices table,QJsonObject json)
{
        for(int i=0;i<tableDevices.count();i++)
        {
            if (  tableDevices[i].table.Regs.SerialNum == table.device.Regs.SerialNum &&
                  tableDevices[i].table.Regs.TypeDevice == table.device.Regs.TypeDevice &&
                  tableDevices[i].table.Regs.VerApp == table.device.Regs.VerApp )
            {
                  memcpy(&tableDevices[i].table,&table.device,sizeof(tableDevices[i].table));
                  break;
            }
        }
           QJsonObject textObject;
           textObject["REQUEST"] = "SET";
           textObject["Serial"] = QString::number(table.device.Regs.SerialNum);
           textObject["Type"] = QString::number(table.device.Regs.TypeDevice);
           textObject["VerAp"] = QString::number(table.device.Regs.VerApp);
           QDateTime actualTime = QDateTime::fromTime_t(table.device.Regs.timechange,Qt::UTC,0);
           textObject["Date"] = actualTime.toString(timeformat);
           textObject["Id"] = idUser;
           textObject["Setting"] = json;

           const QUrl url(webserver);
           QNetworkRequest request(url);
           request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

           QByteArray data = QJsonDocument(textObject).toJson(QJsonDocument::Indented);

           QNetworkReply *reply = httpNetwork->post(request, data);

#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("RED");
    ui->textBrowser->append(data);
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif
           connect(reply, &QNetworkReply::finished, [=](){
               if ( reply->error() == QNetworkReply::NoError ) {
                     QJsonDocument webAnswer = QJsonDocument::fromJson(reply->readAll());
                     struct_filejsonload webjson = verifyWebJson(WEB_CONFIRM_SET,&webAnswer);
                     if ( webjson.SerialNum == table.device.Regs.SerialNum )
                     {

                     }else
                     {

                     }
#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("BLUE");
    ui->textBrowser->append(webAnswer.toJson());
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif
               }
               else
               {
                   QString err = reply->errorString();
                   ui->textBrowser->append("Web: "+err);
               }
               reply->deleteLater();
           });

          // ui->textBrowser->append(QJsonDocument(textObject).toJson(QJsonDocument::Indented));
}


void MainWindow::checkSettingWeb(int numDev)
{
    const QUrl url(webserver);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["REQUEST"] = "GET";
    obj["Serial"] = QString::number(tableDevices[numDev].table.Regs.SerialNum);
    obj["Type"] = QString::number(tableDevices[numDev].table.Regs.TypeDevice);
    obj["VerAp"] = QString::number(tableDevices[numDev].table.Regs.VerApp);
    QDateTime actualTime = QDateTime::fromTime_t(tableDevices[numDev].table.Regs.timechange,Qt::UTC,0);
    obj["Date"] = actualTime.toString(timeformat);
    obj["Id"] = idUser;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = httpNetwork->post(request, data);

#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("RED");
    ui->textBrowser->append(data);
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif

    connect(reply, &QNetworkReply::finished, [=](){
        if ( reply->error() == QNetworkReply::NoError ) {
              tableDevices[numDev].jsonWebDoc = QJsonDocument::fromJson(reply->readAll());
              struct_filejsonload webjson = verifyWebJson(WEB_CONFIRM_GET,&tableDevices[numDev].jsonWebDoc);
#ifdef DEBUG_WEB
    ui->textBrowser->setTextColor("BLUE");
    ui->textBrowser->append(tableDevices[numDev].jsonWebDoc.toJson());
    ui->textBrowser->setTextColor("BLACK");
    ui->textBrowser->append("");
#endif
              if ( webjson.SerialNum == tableDevices[numDev].table.Regs.SerialNum )
              {
                   tableDevices[numDev].SetIsEnable = true;
                   ui->textBrowser->append(QString::number(tableDevices[numDev].table.Regs.SerialNum)+": "+tr("Есть доступные настройки на сервере")+"!");
              }else
              {
                   ui->textBrowser->append(QString::number(tableDevices[numDev].table.Regs.SerialNum)+": "+tr("бновление настроек не требуется"));
              }
        }
        else
        {
            QString err = reply->errorString();
            ui->textBrowser->append("Web: "+err);
        }
        reply->deleteLater();
    });
}

MainWindow::struct_filejsonload MainWindow::verifyWebJson(QString answStr,QJsonDocument* jsondoc)
{
    struct_filejsonload fileload;
    fileload.ID =0;
    fileload.SerialNum =0;
    fileload.TypeDevice =0;
    fileload.VerApp =0;
      QJsonObject textObject = jsondoc->object();
      if( !textObject.value("Serial").isNull() &&
          !textObject.value("Type").isNull()   &&
          !textObject.value("VerAp").isNull()  &&
          !textObject.value("Id").isNull()     &&
          !textObject.value("Date").isNull()      )
      {
          QString strj = textObject.value("REQUEST").toString();
          if ( strj == answStr )
          {
              uint32_t SerialNum = textObject.value("Serial").toVariant().toUInt();
              uint32_t TypeDevice = textObject.value("Type").toVariant().toUInt();
              uint32_t VerApp = textObject.value("VerAp").toVariant().toUInt();
              uint32_t ID = textObject.value("Id").toVariant().toUInt();
              QString datetime = textObject.value("Date").toString();
              QDateTime actualTime = QDateTime::fromString(datetime,timeformat);

              fileload.SerialNum = SerialNum;
              fileload.TypeDevice = TypeDevice;
              fileload.VerApp = VerApp;
              fileload.ID = ID;
              fileload.actualTime = actualTime;
          }
      }
  return fileload;
}

MainWindow::struct_filejsonload MainWindow::chooseFile(int numdev,QFile* file)
{
    struct_filejsonload fileload;

             QString loadFileName = QFileDialog::getOpenFileName(this,
                                                              tr("Открыть"),
                                                              QString(),
                                                              "JSON (*.json)");
              QFileInfo fileInfo(loadFileName);
              file->setFileName(loadFileName);

              if (!file->open(QIODevice::ReadOnly))
              {

              }else
              {
                tableDevices[numdev].jsonFileDoc = QJsonDocument::fromJson(file->readAll());
                QJsonObject textObject =  tableDevices[numdev].jsonFileDoc.object();
                if( !textObject.value("Serial").isNull() &&
                    !textObject.value("Type").isNull()   &&
                    !textObject.value("VerAp").isNull()  &&
                    !textObject.value("Id").isNull()     &&
                    !textObject.value("Date").isNull()      )
                {
                    uint32_t SerialNum = textObject.value("Serial").toVariant().toUInt();
                    uint32_t TypeDevice = textObject.value("Type").toVariant().toUInt();
                    uint32_t VerApp = textObject.value("VerAp").toVariant().toUInt();
                    uint32_t ID = textObject.value("Id").toVariant().toUInt();
                    QString datetime = textObject.value("Date").toString();
                    QDateTime actualTime = QDateTime::fromString(datetime,timeformat);

                    if ( SerialNum == tableDevices[numdev].table.Regs.SerialNum &&
                         TypeDevice == tableDevices[numdev].table.Regs.TypeDevice &&
                         VerApp == tableDevices[numdev].table.Regs.VerApp )
                    {

                        fileload.SerialNum = SerialNum;
                        fileload.TypeDevice = TypeDevice;
                        fileload.VerApp = VerApp;
                        fileload.ID = ID;
                        fileload.actualTime = actualTime;
                        fileload.filename = fileInfo.baseName();
                    }else
                    {
                        QMessageBox::information(this, tableDevices[numdev].devicename,tr("Настройки не соответствуют датчику"));
                    }
                }
               file->close();
              }
    return fileload;
}


QString MainWindow::butSave(int numdev)
{
    QString answer;
    if ( tableDevices[numdev].isOpen )
    {
         struct_listSavedDevices table;
         memcpy(&table.device,&tableDevices[numdev].table,sizeof(table.device));
         table.devicename = tableDevices[numdev].devicename;
         table.modbusadr = QString::number(tableDevices[numdev].com.currentAdr);
         table.portname = tableDevices[numdev].com.nameCom;

         if ( libs->devStatus(table) == DeviceLibs::DEV_READY )
         {
              QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Сохранить"),
                                                                QString(),
                                                                tr("JSON (*.json)"));
              QFileInfo fileInfo(saveFileName);
              QFile jsonFile(saveFileName);

              if (!jsonFile.open(QIODevice::WriteOnly))
              {

              }else
              {
                QJsonObject textObject;
                textObject["Serial"] = QString::number(tableDevices[numdev].table.Regs.SerialNum);
                textObject["Type"] = QString::number(tableDevices[numdev].table.Regs.TypeDevice);
                textObject["VerAp"] = QString::number(tableDevices[numdev].table.Regs.VerApp);
                QDateTime actualTime = QDateTime::currentDateTimeUtc();
                textObject["Date"] = actualTime.toString(timeformat);
                textObject["Id"] = idUser;

                textObject["Setting"] = libs->getSetting(table);
                jsonFile.write(QJsonDocument(textObject).toJson(QJsonDocument::Indented));
                jsonFile.close();
                answer = fileInfo.baseName();
              }
         }else
         {
             QMessageBox::information(this, tableDevices[numdev].devicename,tr("Программа считывает настройки"));
         }
    }else
    {
        QMessageBox::information(this, tableDevices[numdev].devicename,tr("Необходимо открыть устройство, программа считает настройки автоматически"));
    }
    return answer;
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
         if( tablstr.count()==4 )
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
        ui->treeWidget->setHeaderLabel(tr("Найденные устройства")+"...");
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
            int numdev = strloc.toInt();

            QAction *newconnect = new QAction(tr("Открыть"));
            QIcon icon1;
            icon1.addFile(QString::fromUtf8(":/PNG/images/connect.png"), QSize(), QIcon::Normal, QIcon::Off);
            newconnect->setIcon(icon1);
            QAction *newdisconnect = new QAction(tr("Закрыть"));
            QIcon icon2;
            icon2.addFile(QString::fromUtf8(":/PNG/images/disconnect.png"), QSize(), QIcon::Normal, QIcon::Off);
            newdisconnect->setIcon(icon2);

            if( tableDevices[numdev].isOpen )
            {
              newdisconnect->setEnabled(true);
              newconnect->setEnabled(false);
            }else
            {
                newdisconnect->setEnabled(false);
                newconnect->setEnabled(true);
            }

            QMenu menu(this);
            newconnect->setData(strloc);

            connect(newconnect,&QAction::triggered,this,&MainWindow::LoadLibDevice);
            connect(newdisconnect,&QAction::triggered,this,[=]
            {
                    struct_listSavedDevices table;
                    memcpy(&table.device,&tableDevices[numdev].table,sizeof(table.device));
                    table.devicename = tableDevices[numdev].devicename;
                    table.modbusadr = QString::number(tableDevices[numdev].com.currentAdr);
                    table.portname = tableDevices[numdev].com.nameCom;
                    libs->CloseDev(table);
            });

            menu.addAction(newconnect);
            menu.addAction(newdisconnect);
            menu.addAction(ui->actionSaved);
            menu.addAction(ui->actionUpdate);
            menu.addAction(ui->actionAditional);
            menu.addSection("new");
            menu.addAction(ui->actionHistory);
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

             int numDev = str.toInt();
             if ( tableDevices.count()>= numDev )
             {
                  struct_listSavedDevices table;
                  memcpy(&table.device,&tableDevices[numDev].table,sizeof(table.device));
                  table.devicename = tableDevices[numDev].devicename;
                  table.modbusadr = QString::number(tableDevices[numDev].com.currentAdr);
                  table.portname = tableDevices[numDev].com.nameCom;

                  if( tableDevices[numDev].com.modbusDev!=nullptr  )
                  {
                      if ( !tableDevices[numDev].isOpen )
                      {
                          libs->setIdUser( idUser );
                          if ( libs->LibOpen(table,ui->mdiArea,tableDevices[numDev].com.modbusDev) )
                          {
                               tableDevices[numDev].isOpen = true;
                          }
                      }
                  }
             }
         }
     }
}

void MainWindow::ViewSettingsDevice()
{
     QDialog *dialog = new QDialog(ui->mdiArea);

          QLabel *dateFrom = new QLabel(dialog);
          dateFrom->setText(tr("Период с")+":");
          QLabel *dateTo = new QLabel(dialog);
          dateTo->setText(tr("Период по")+":");
          QDateTimeEdit *dateFromWidget = new QDateTimeEdit(dialog);
          QDateTimeEdit *dateToWidget = new QDateTimeEdit(dialog);
          QTableWidget *tableWidget = new QTableWidget(dialog);

          int rowCount = selectedDevices.count();
          int columnCount = 8;

          QTableWidgetItem *itemHead1 = new QTableWidgetItem(tr("Серийный"));
          QTableWidgetItem *itemHead2 = new QTableWidgetItem(tr("Время соединения"));
          QTableWidgetItem *itemHead3 = new QTableWidgetItem(tr("Id пользователя"));
          QTableWidgetItem *itemHead4 = new QTableWidgetItem(tr("Время настройки"));
          QTableWidgetItem *itemHead5 = new QTableWidgetItem(tr("Id пользов. настроек"));
          QTableWidgetItem *itemHead6 = new QTableWidgetItem(tr("Время основных настроек"));
          QTableWidgetItem *itemHead7 = new QTableWidgetItem(tr("Id пользователя"));
          QTableWidgetItem *itemHead8 = new QTableWidgetItem(tr("Доп поле"));

          tableWidget->setColumnCount(columnCount);
          tableWidget->setRowCount(rowCount);

          tableWidget->setHorizontalHeaderItem(0,itemHead1);
          tableWidget->setHorizontalHeaderItem(1,itemHead2);
          tableWidget->setHorizontalHeaderItem(2,itemHead3);
          tableWidget->setHorizontalHeaderItem(3,itemHead4);
          tableWidget->setHorizontalHeaderItem(4,itemHead5);
          tableWidget->setHorizontalHeaderItem(5,itemHead6);
          tableWidget->setHorizontalHeaderItem(6,itemHead7);
          tableWidget->setHorizontalHeaderItem(7,itemHead8);

          for(int kol=0; kol!=tableWidget->rowCount(); ++kol){
                  QTableWidgetItem *newItem[columnCount];
                  for( int i=0;i<columnCount;i++ )
                  {
                       newItem[i] = new QTableWidgetItem();
                  }
                  int row = selectedDevices[kol];
                  newItem[0]->setText(QString::number(tableDevices[row].table.Regs.SerialNum));

                  if( tableDevices[row].table.Regs.timeconnect<UINT64_MAX && tableDevices[row].table.Regs.timeconnect>0)
                  newItem[1]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timeconnect,Qt::UTC,0).toString(timeformat));

                  if(tableDevices[row].table.Regs.idchange<UINT32_MAX)
                  newItem[2]->setText(QString::number(tableDevices[row].table.Regs.idchange));

                  if(tableDevices[row].table.Regs.timechange<UINT64_MAX)
                  newItem[3]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timechange,Qt::UTC,0).toString(timeformat));

                  if(tableDevices[row].table.Regs.idset<UINT32_MAX)
                  newItem[4]->setText(QString::number(tableDevices[row].table.Regs.idset));

                  if(tableDevices[row].table.Regs.timedefault<UINT64_MAX)
                  newItem[5]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timedefault,Qt::UTC,0).toString(timeformat));

                  if(tableDevices[row].table.Regs.iddefault<UINT32_MAX)
                  newItem[6]->setText(QString::number(tableDevices[row].table.Regs.iddefault));

                  QString str;
                  QByteArray aray;
                  for (int i = 0; i < static_cast<int>(sizeof(tableDevices[row].table.Regs.mas)); ++i)
                  {
                        aray.append((const char*)(tableDevices[row].table.Regs.mas + i), sizeof(uint16_t));
                  }
                  for ( int i=0;i<static_cast<int>(sizeof(tableDevices[row].table.Regs.mas));i++ )
                  {
                        if(aray[i].operator!=(0xFF))
                        {
                            str = QString::fromLocal8Bit(aray);
                            newItem[7]->setText(str);
                            break;
                        }
                  }

                  for( int i=0;i<columnCount;i++ )
                  {
                       tableWidget->setItem(kol, i, newItem[i]);
                  }
          }

          QGridLayout * grid = new QGridLayout(dialog);
          grid->addWidget(dateFrom,0,0);
          grid->addWidget(dateFromWidget,0,1);
          grid->addWidget(dateTo,0,2);
          grid->addWidget(dateToWidget,0,3);
          grid->addWidget(tableWidget,1,0,1,4);

          tableWidget->horizontalHeader()->setStretchLastSection(true);
          tableWidget->resizeColumnsToContents();

          QPoint poz = this->pos();
          QSize mainsize= ui->mdiArea->size();
          QSize mainui= this->size();
          dialog->setFixedWidth(mainsize.width());
          QSize logsize= dialog->size();

          poz.setX(poz.x()+mainui.width()/2 - logsize.width()/2);
          poz.setY(poz.y()+mainsize.height()/2 - logsize.height()/2);
          dialog->move(poz);

     dialog->show();
}
