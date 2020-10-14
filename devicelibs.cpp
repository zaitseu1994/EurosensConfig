#include "devicelibs.h"
#include <QLibrary>
#include <QAction>
#include "libtype4.h"
#include "mws.h"

#include "QMdiSubWindow"

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

bool DeviceLibs:: LibOpen(QString str,QMdiArea *mdiArea,QModbusClient *modbus)
{
    bool stat =false;
    MWS *m_settings = new MWS(mdiArea);

//    MdiSubWindowEventFilter * p_mdiSubWindowEventFilter;
//    QMdiSubWindow * subWindow = mdiArea->addSubWindow(m_settings);

    MyQMdiSubWindow *mysub = new MyQMdiSubWindow();
    mysub->setStr(str);
    mysub->setWidget(m_settings);
    mdiArea->addSubWindow(mysub);
    connect(mysub,&MyQMdiSubWindow::closed,this,[=](QString strWindow)
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
                if( vectorDialogs[i].str == strWindow )
                {
                    if( vectorDialogs[i].dialog!=nullptr )
                    {
                       vectorDialogs[i].dialog->deleteLater();
                    }
                    if ( vectorDialogs[i].subWin!=nullptr )
                    {
                         vectorDialogs[i].subWin->deleteLater();
                    }
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
    m_settings->getStr(str);
    m_settings->start(modbus);
    m_settings->setWindowTitle(str);
    m_settings->show();

    struct_DialofInfo infidialog;
    infidialog.dialog = m_settings;
    infidialog.subWin = mysub;
    infidialog.modbus = modbus;
    infidialog.str = str;
    infidialog.mdiArea = mdiArea;
    vectorDialogs << infidialog;

    return  stat;
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
