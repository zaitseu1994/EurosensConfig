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
public:
    explicit MyQMdiSubWindow(QMdiSubWindow *parent = nullptr);
    ~MyQMdiSubWindow();
private:
QString str = "0";
public:
void setStr(QString st)
{
  str = st;
};
signals:
    void closed(QString str);
protected:
    void closeEvent( QCloseEvent * closeEvent )
    {
        emit closed(str);
        closeEvent->accept();
    }
};

class DeviceLibs : public QObject
{
    Q_OBJECT
public:
    explicit DeviceLibs();
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


