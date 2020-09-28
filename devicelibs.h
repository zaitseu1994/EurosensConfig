#ifndef DEVICELIBS_H
#define DEVICELIBS_H

#include <QMdiSubWindow>
#include <QCloseEvent>

#include <QString>
#include <QVector>
#include <QLibrary>
#include <QMdiArea>

#include "structs_ui.h"
#include "structs_main.h"
#include "mws.h"


class MyQMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

signals:
    void closed();

protected:
    void closeEvent( QCloseEvent * closeEvent )
    {
        emit closed();
        closeEvent->accept();
    }
};


class DeviceLibs : public QObject
{
    Q_OBJECT
public:
     DeviceLibs();
    ~DeviceLibs();
    bool LibOpen(QString str,QMdiArea *mdiArea,QModbusClient *modbus);
    bool CloseAll();
private:
    typedef struct struct_DialofInfo
    {
        QString str;
        QModbusClient *modbus;
        QMdiArea      *mdiArea;
        MWS           *dialog;
        MyQMdiSubWindow *subWin;
    }struct_DialofInfo;
    QVector<struct_DialofInfo> vectorDialogs;
};

#endif // DEVICELIBS_H


