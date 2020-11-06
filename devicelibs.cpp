#include "devicelibs.h"
#include <QLibrary>
#include <QAction>
#include "libtype4.h"
#include "mws.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "QMdiSubWindow"

#include <QMessageBox>

#include <QFile>

MyQMdiSubWindow::MyQMdiSubWindow(QMdiSubWindow *parent) :
        QMdiSubWindow(parent)
{

}

MyQMdiSubWindow::~MyQMdiSubWindow()
{

}

DeviceLibs::DeviceLibs()
{

}

DeviceLibs::~DeviceLibs()
{
    CloseAll();
}

bool DeviceLibs::setSetting(struct_listSavedDevices table,QJsonObject json)
{
  bool stat =false;
  for(int i=0;i<vectorDialogs.count();i++)
  {
      if( vectorDialogs[i].table.device.Regs.SerialNum == table.device.Regs.SerialNum &&
          vectorDialogs[i].table.device.Regs.TypeDevice == table.device.Regs.TypeDevice &&
          vectorDialogs[i].table.device.Regs.VerApp == table.device.Regs.VerApp)
      {
          stat = vectorDialogs[i].dialog->setSetting(json);
          break;
      }
  }
  return stat;
}

QJsonObject DeviceLibs::getSetting(struct_listSavedDevices table)
{
  QJsonObject json;
  for(int i=0;i<vectorDialogs.count();i++)
  {
      if( vectorDialogs[i].table.device.Regs.SerialNum == table.device.Regs.SerialNum &&
          vectorDialogs[i].table.device.Regs.TypeDevice == table.device.Regs.TypeDevice &&
          vectorDialogs[i].table.device.Regs.VerApp == table.device.Regs.VerApp)
      {
          json = vectorDialogs[i].dialog->getSetting();
          break;
      }
  }
  return json;
}

void DeviceLibs::devDisconnect(struct_listSavedDevices table)
{
  for(int i=0;i<vectorDialogs.count();i++)
  {
      if( vectorDialogs[i].table.device.Regs.SerialNum == table.device.Regs.SerialNum &&
          vectorDialogs[i].table.device.Regs.TypeDevice == table.device.Regs.TypeDevice &&
          vectorDialogs[i].table.device.Regs.VerApp == table.device.Regs.VerApp)
      {
          vectorDialogs[i].StateConnect = DEV_DISCONNECT;
          QMessageBox::information(vectorDialogs[i].dialog,vectorDialogs[i].table.devicename,"Датчик не отвечает");
          CloseDev(table);
          break;
      }
  }
}

DeviceLibs::state_dev DeviceLibs::devStatus(struct_listSavedDevices table)
{
   state_dev stat = DEV_DISCONNECT;
   for(int i=0;i<vectorDialogs.count();i++)
   {
       if( vectorDialogs[i].table.device.Regs.SerialNum == table.device.Regs.SerialNum &&
           vectorDialogs[i].table.device.Regs.TypeDevice == table.device.Regs.TypeDevice &&
           vectorDialogs[i].table.device.Regs.VerApp == table.device.Regs.VerApp)
       {
           stat = vectorDialogs[i].StateConnect;
           if ( !vectorDialogs[i].dialog->isEnabled() )
           stat = DEV_BUSY;
           else
           stat = DEV_READY;
           break;
       }
   }
   return stat;
}

bool DeviceLibs:: LibOpen(struct_listSavedDevices table,QMdiArea *mdiArea,QModbusClient *modbus)
{
    bool stat =false;
    MWS *m_settings = new MWS();

    connect(m_settings,&MWS::DevDisconnect,this,&DeviceLibs::devDisconnect);

    MyQMdiSubWindow *mysub = new MyQMdiSubWindow();

    mysub->setStr(table);
    mysub->setWidget(m_settings);
    mdiArea->addSubWindow(mysub);
    connect(mysub,&MyQMdiSubWindow::closed,this,[=](struct_listSavedDevices tableStr)
    {
            static const char* const FILE_NAME = "mws.bin";
            QFile file( FILE_NAME );
            QDataStream stream( &file );
            QByteArray window = mysub->saveGeometry();
            file.open( QIODevice::WriteOnly );
            if (file.isOpen())
            {
                stream << window;
                file.close();
            }

            for(int i=0;i<vectorDialogs.count();i++)
            {
                if( vectorDialogs[i].table.device.Regs.SerialNum == tableStr.device.Regs.SerialNum &&
                    vectorDialogs[i].table.device.Regs.TypeDevice == tableStr.device.Regs.TypeDevice &&
                    vectorDialogs[i].table.device.Regs.VerApp == tableStr.device.Regs.VerApp)
                {
                    if( vectorDialogs[i].dialog!=nullptr )
                    {
                       vectorDialogs[i].dialog->deleteLater();
                    }
                    if ( vectorDialogs[i].subWin!=nullptr )
                    {
                         vectorDialogs[i].subWin->deleteLater();
                    }
                    emit closed(vectorDialogs[i].table);
                    vectorDialogs.remove(i);
                    break;
                }
            }
    });

    static const char* const FILE_NAME = "mws.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray window;
    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
        stream >> window;
        file.close();
        mysub->restoreGeometry(window);
    }

    m_settings->setId(idUser);
    m_settings->getTable(table);
    m_settings->start(modbus);
    m_settings->setWindowTitle("Dev:"+table.devicename+"/"+table.modbusadr+"/"+table.portname);
    m_settings->show();

    struct_DialofInfo infidialog;
    infidialog.dialog = m_settings;
    infidialog.subWin = mysub;
    infidialog.modbus = modbus;
    infidialog.table = table;
    infidialog.StateConnect = DEV_DISCONNECT;
    infidialog.mdiArea = mdiArea;
    vectorDialogs << infidialog;

    stat =true;
    return  stat;
}

bool DeviceLibs::CloseDev(struct_listSavedDevices table)
{
    bool stat =false;
    for(int i=0;i<vectorDialogs.count();i++)
    {
        if( vectorDialogs[i].table.device.Regs.SerialNum == table.device.Regs.SerialNum &&
            vectorDialogs[i].table.device.Regs.TypeDevice == table.device.Regs.TypeDevice &&
            vectorDialogs[i].table.device.Regs.VerApp == table.device.Regs.VerApp)
        {

            if( vectorDialogs[i].dialog!=nullptr )
            {
                vectorDialogs[i].dialog->deleteLater();
            }
            if( vectorDialogs[i].subWin!=nullptr )
            {
                vectorDialogs[i].subWin->deleteLater();
            }
            emit closed(vectorDialogs[i].table);
            vectorDialogs.remove(i);
            stat = true;
            break;
        }
    }
    return stat;
}

bool DeviceLibs:: CloseAll()
{
    for(int i =0;i<vectorDialogs.count();i++)
    {
         if( vectorDialogs[i].dialog!=nullptr )
         {
             vectorDialogs[i].dialog->deleteLater();
         }
         if( vectorDialogs[i].subWin!=nullptr)
         {
             vectorDialogs[i].subWin->deleteLater();
         }
    }
    vectorDialogs.clear();
    return true;
}
