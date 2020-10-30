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
                QString comname = vectorModbusDevice[intcomModBusDevice].nameCom;
                QMessageBox::warning(this, comname,"Проверьте соединение с портом");
            });

    libs = new DeviceLibs;

    QMenu *menu_view = new QMenu(this);
    menu_view->setTitle("Вид");

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
    //connect(ui->actionSaved,&QAction::triggered,this,&MainWindow::DevicesSaved);
    connect(ui->actionCancel,&QAction::triggered,this,&MainWindow::DevicesSearchDisable);

    connect(ui->actionQuit,&QAction::triggered,this,[=]
    {
        this->close();
    });

    ui->treeWidget->setHeaderLabel(" ");
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,&QTreeWidget::customContextMenuRequested,this,&MainWindow::prepareMenu);

    statbar_NameD.setText("Имя:");
    statbar_TypeD.setText("Тип:");
    statbar_SerialD.setText("Серийный:");
    statbar_AppD.setText("Аппаратная версия:");
    statbar_LogD.setText("Лог:");
    statbar_ProtcD.setText("Протокол:");
    statbar_AdrD.setText("Адрес:");
    statbar_PortD.setText("Порт:");

    statbar_NameD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_TypeD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_SerialD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_AppD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_LogD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_ProtcD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_AdrD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_PortD.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));

    statbar_Name.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Type.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Serial.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_App.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Log.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Protc.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Adr.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    statbar_Port.setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));

    ui->statusbar->addWidget(&statbar_NameD);
    ui->statusbar->addWidget(&statbar_Name);
    ui->statusbar->addWidget(&statbar_TypeD);
    ui->statusbar->addWidget(&statbar_Type);
    ui->statusbar->addWidget(&statbar_SerialD);
    ui->statusbar->addWidget(&statbar_Serial);
    ui->statusbar->addWidget(&statbar_AppD);
    ui->statusbar->addWidget(&statbar_App);
    ui->statusbar->addWidget(&statbar_LogD);
    ui->statusbar->addWidget(&statbar_Log);
    ui->statusbar->addWidget(&statbar_ProtcD);
    ui->statusbar->addWidget(&statbar_Protc);
    ui->statusbar->addWidget(&statbar_AdrD);
    ui->statusbar->addWidget(&statbar_Adr);
    ui->statusbar->addWidget(&statbar_PortD);
    ui->statusbar->addWidget(&statbar_Port);

     login = new Login(this);
     butlogin = new QCommandLinkButton(this);
     butlogin->setText("Нажмите чтоб войти");

     ui->devices->setEnabled(false);
     ui->actionSearh->setEnabled(false);
     ui->actionCancel->setEnabled(false);
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

void MainWindow::DevicesSearch()
{
 ui->textBrowser->clear();
 ui->treeWidget->clear();

 ui->actionSearh->setText("Идет поиск...");
 ui->actionSearh->setDisabled(true);

 QList<QMdiSubWindow *> list = ui->mdiArea->subWindowList(QMdiArea::CreationOrder);
 if(list.count()!=0)
 {
     ui->mdiArea->closeAllSubWindows();
     libs->CloseAll();
 }

 statbar_Name.setText(" ");
 statbar_Type.setText(" ");
 statbar_Serial.setText(" ");
 statbar_App.setText(" ");
 statbar_Log.setText(" ");
 statbar_Protc.setText(" ");
 statbar_Adr.setText(" ");
 statbar_Port.setText(" ");

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
     ui->treeWidget->setHeaderLabel("Найденные устройства...");
     textCursor = ui->textBrowser->textCursor().position();
     ui->textBrowser->append(infos[0].portName()+":...");
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

//           connect(newCh.modbusDev, &QModbusClient::stateChanged,this,[this](QModbusDevice::State)
//           {

//           });

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
        cur.insertText(vectorModbusDevice[intcomModBusDevice].nameCom+": Адрес:"+QString::number(vectorModbusDevice[intcomModBusDevice].currentAdr) +
                       " ..."+ QString::number(CurentRequestAdr*100/CountRequestAdr)+"%");

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
    if(flag)
    for( int i=0;i<vectorModbusDevice.count();i++ )
    {
        vectorModbusDevice[i].currentAdr =  LAST_MODBUS_ADRESS;
        ui->textBrowser->append("Поиск прерван пользователем...");
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

   struct_devices inDevice;
   memcpy(&inDevice.com,&com,sizeof(inDevice.com));
   memcpy(&inDevice.table,&table,sizeof(inDevice.table));
   inDevice.devicename = nameconnect;
   tableDevices << inDevice;
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

                 statbar_Name.setText(tableDevices[numDev].devicename);
                 statbar_Type.setText(QString::number(tableDevices[numDev].table.Regs.TypeDevice));
                 statbar_Serial.setText(QString::number(tableDevices[numDev].table.Regs.SerialNum));
                 statbar_App.setText(QString::number(tableDevices[numDev].table.Regs.VerApp));
                 statbar_Log.setText(QString::number(tableDevices[numDev].table.Regs.LogError));
                 statbar_Protc.setText("MODBUS");
                 statbar_Adr.setText(QString::number(tableDevices[numDev].com.currentAdr));
                 statbar_Port.setText(tableDevices[numDev].com.nameCom);
             }
        }
      }
    }
}

void MainWindow::additionalChange()
{
    QDialog *dialog = new QDialog(ui->mdiArea);

    QComboBox *devcombo = new QComboBox();

    for(int i=0;i<selectedDevices.count();i++)
    {
        devcombo->addItem(QString::number(selectedDevices[i]+1)+") Serial: "+QString::number(tableDevices[i].table.Regs.SerialNum)+"; Имя: "+tableDevices[i].devicename);
    }
    devcombo->setCurrentIndex(0);

    QLabel *labname = new QLabel("Изменить имя");
    QLineEdit *linname = new QLineEdit();
    QPushButton *butname = new QPushButton("Сменить");
    connect(butname,&QPushButton::clicked,this,[=]
    {
       if( linname->text().length()>=1 )
       {
           QStringList strli = devcombo->currentText().split(") Serial:",Qt::SkipEmptyParts);
           int devnum = strli[0].toInt()-1;
           tableDevices[devnum].devicename = linname->text();
           setNameDevice(linname->text(),devnum);

           devcombo->setItemText(devcombo->currentIndex(),QString::number(devnum+1)+") Serial: "+QString::number(tableDevices[devnum].table.Regs.SerialNum)+"; Имя: "+tableDevices[devnum].devicename);
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
    groupinterf->setTitle("Интерфейс");

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

      int rowCount = selectedDevices.count();
      int columnCount = 6;

      QTableWidgetItem *itemHead1 = new QTableWidgetItem("Серийный");
      QTableWidgetItem *itemHead2 = new QTableWidgetItem("Тип");
      QTableWidgetItem *itemHead3 = new QTableWidgetItem("Версия");
      QTableWidgetItem *itemHead4 = new QTableWidgetItem("Дата сохранения");
      QTableWidgetItem *itemHead5 = new QTableWidgetItem("Id пользователя");
      QTableWidgetItem *itemHead6 = new QTableWidgetItem("Действие");

      tableWidget->setColumnCount(columnCount);
      tableWidget->setRowCount(rowCount);

      tableWidget->setHorizontalHeaderItem(0,itemHead1);
      tableWidget->setHorizontalHeaderItem(1,itemHead2);
      tableWidget->setHorizontalHeaderItem(2,itemHead3);
      tableWidget->setHorizontalHeaderItem(3,itemHead4);
      tableWidget->setHorizontalHeaderItem(4,itemHead5);
      tableWidget->setHorizontalHeaderItem(5,itemHead6);

      for(int kol=0; kol!=tableWidget->rowCount(); ++kol){
          QTableWidgetItem *newItem[columnCount];
          for( int i=0;i<columnCount-1;i++ )
          {
               newItem[i] = new QTableWidgetItem();
          }
          int numdev = selectedDevices[kol];

          newItem[0]->setText(QString::number(tableDevices[numdev].table.Regs.SerialNum));
          newItem[1]->setText(QString::number(tableDevices[numdev].table.Regs.TypeDevice));
          newItem[2]->setText(QString::number(tableDevices[numdev].table.Regs.VerApp));

          QDateTime actualTime = QDateTime::currentDateTime();
          newItem[3]->setText(actualTime.toString("yyyy-MM-dd"));

          newItem[4]->setText(idUser);

          QPushButton *but = new QPushButton("Сохранить");

          for( int i=0;i<columnCount-1;i++ )
          {
               tableWidget->setItem(kol, i, newItem[i]);
          }
          tableWidget->setCellWidget(kol,columnCount-1,but);
      }

      tableWidget->horizontalHeader()->setStretchLastSection(true);
      tableWidget->resizeColumnsToContents();

      QGridLayout * grid = new QGridLayout(dialog);
      grid->addWidget(tableWidget,0,0);


      QPoint poz = this->pos();
      QSize mainsize= ui->mdiArea->size();
      QSize mainui= this->size();
      dialog->setFixedWidth(mainsize.width()/2);
      QSize logsize= dialog->size();

      poz.setX(poz.x()+mainui.width()/2 - logsize.width()/2);
      poz.setY(poz.y()+mainsize.height()/2 - logsize.height()/2);
      dialog->move(poz);

      //dialog->setBaseSize(ui->mdiArea->width()/2,dialog->height()/2);
      dialog->show();
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

            QMenu menu(this);
            newconnect->setData(strloc);
            connect(newconnect,&QAction::triggered,this,&MainWindow::LoadLibDevice);

            menu.addAction(newconnect);
            menu.addAction(newdisconnect);
            menu.addAction(ui->actionDownload);
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

                  QString string = tableToString(table);

                  if( tableDevices[numDev].com.modbusDev!=nullptr  )
                  {
                      libs->setIdUser( idUser );
                      if ( libs->LibOpen(string,ui->mdiArea,tableDevices[numDev].com.modbusDev) )
                      {
                           tableDevices[numDev].isOpen = true;
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
          dateFrom->setText("Период с:");
          QLabel *dateTo = new QLabel(dialog);
          dateTo->setText("Период по:");
          QDateTimeEdit *dateFromWidget = new QDateTimeEdit(dialog);
          QDateTimeEdit *dateToWidget = new QDateTimeEdit(dialog);
          QTableWidget *tableWidget = new QTableWidget(dialog);

          int rowCount = selectedDevices.count();
          int columnCount = 7;

          QTableWidgetItem *itemHead1 = new QTableWidgetItem("Serial");
          QTableWidgetItem *itemHead2 = new QTableWidgetItem("Время соединения");
          QTableWidgetItem *itemHead3 = new QTableWidgetItem("Время настройки");
          QTableWidgetItem *itemHead4 = new QTableWidgetItem("Id пользователя");
          QTableWidgetItem *itemHead5 = new QTableWidgetItem("Время основных настроек");
          QTableWidgetItem *itemHead6 = new QTableWidgetItem("Id пользователя");
          QTableWidgetItem *itemHead7 = new QTableWidgetItem("Доп поле");

          tableWidget->setColumnCount(columnCount);
          tableWidget->setRowCount(rowCount);

          tableWidget->setHorizontalHeaderItem(0,itemHead1);
          tableWidget->setHorizontalHeaderItem(1,itemHead2);
          tableWidget->setHorizontalHeaderItem(2,itemHead3);
          tableWidget->setHorizontalHeaderItem(3,itemHead4);
          tableWidget->setHorizontalHeaderItem(4,itemHead5);
          tableWidget->setHorizontalHeaderItem(5,itemHead6);
          tableWidget->setHorizontalHeaderItem(6,itemHead7);

          for(int kol=0; kol!=tableWidget->rowCount(); ++kol){
                  QTableWidgetItem *newItem[columnCount];
                  for( int i=0;i<columnCount;i++ )
                  {
                       newItem[i] = new QTableWidgetItem();
                  }
                  int row = selectedDevices[kol];
                  newItem[0]->setText(QString::number(tableDevices[row].table.Regs.SerialNum));

                  if( tableDevices[row].table.Regs.timeconnect<UINT64_MAX && tableDevices[row].table.Regs.timeconnect>0)
                  newItem[1]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timeconnect).toString("yyyy-MM-dd  HH:mm:ss"));

                  if(tableDevices[row].table.Regs.timechange<UINT64_MAX)
                  newItem[2]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timechange).toString("yyyy-MM-dd  HH:mm:ss"));

                  if(tableDevices[row].table.Regs.idchange<UINT32_MAX)
                  newItem[3]->setText(QString::number(tableDevices[row].table.Regs.idchange));

                  if(tableDevices[row].table.Regs.timedefault<UINT64_MAX)
                  newItem[4]->setText(QDateTime::fromTime_t(tableDevices[row].table.Regs.timedefault).toString("yyyy-MM-dd  HH:mm:ss"));

                  if(tableDevices[row].table.Regs.iddefault<UINT32_MAX)
                  newItem[5]->setText(QString::number(tableDevices[row].table.Regs.iddefault));

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
                            newItem[6]->setText(str);
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
