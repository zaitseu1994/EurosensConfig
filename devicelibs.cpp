#include "devicelibs.h"
#include <QLibrary>
#include <QAction>
#include "libtype4.h"
#include "mws.h"


DeviceLibs::DeviceLibs()
{

}

DeviceLibs::~DeviceLibs()
{

}

void DeviceLibs:: SubWindowClose(QString str)
{
     for(int i=0;i<vectorDialogs.count();i++)
     {
         if ( vectorDialogs[i].str == str )
         {
              vectorDialogs.remove(i);
         }
     }
}

bool DeviceLibs:: LibOpen(QString str,QMdiArea *mdiArea,QModbusClient *modbus)
{
    bool stat =false;
    MWS *m_settings = new MWS(mdiArea);
    mdiArea->addSubWindow(m_settings);
    m_settings->getStr(str);
    m_settings->start(modbus);
    m_settings->setWindowTitle(str);
    m_settings->show();

    struct_DialofInfo infidialog;
    infidialog.dialog = m_settings;
    infidialog.modbus = modbus;
    infidialog.str = str;
    infidialog.mdiArea = mdiArea;
    connect(m_settings,&MWS::closeWindow,this,&DeviceLibs::SubWindowClose);
    vectorDialogs << infidialog;
    return  stat;
}

bool DeviceLibs:: CloseAll()
{
    for(int i =0;i<vectorDialogs.count();i++)
    {
         if( vectorDialogs[i].dialog )
         {
             vectorDialogs[i].dialog->deleteLater();
             vectorDialogs[i].dialog->hide();
         }
    }
    vectorDialogs.clear();
    return true;
}
