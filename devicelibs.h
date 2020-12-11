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
struct_listSavedDevices tableStr ;
QString idUser = "0";
public:

void setStr(struct_listSavedDevices table)
{
     tableStr = table;
};
public:

signals:
    void closed(struct_listSavedDevices table);
protected:
    void closeEvent( QCloseEvent * closeEvent )
    {
        emit closed(tableStr);
        closeEvent->accept();
    }
};

class DeviceLibs : public QObject
{
    Q_OBJECT

public:
    typedef enum{
        DEV_READY,
        DEV_BUSY,
        DEV_DISCONNECT
    }state_dev ;

public:
    explicit DeviceLibs();
    ~DeviceLibs();
    bool LibOpen(struct_listSavedDevices table,QMdiArea *mdiArea,QModbusClient *modbus);
    void devDisconnect(struct_listSavedDevices table);
    void devReady(struct_listSavedDevices table);
    void devBusy(struct_listSavedDevices table);
    state_dev devStatus(struct_listSavedDevices table);

    void devSettingsAccept(struct_listSavedDevices table,QJsonObject json);

    bool CloseAll();
    bool CloseDev(struct_listSavedDevices table);
    QJsonObject getSetting(struct_listSavedDevices table);
    bool setSetting(struct_listSavedDevices table,QJsonObject json,QString idSet,QString timeset);
    void setIdUser(QString str)
    {
       idUser = str;
    };
signals:
    void closed(struct_listSavedDevices table);
    void SettingsAccept(struct_listSavedDevices table,QJsonObject json);
private:
    QString idUser = "0";
    typedef struct struct_DialofInfo
    {
        struct_listSavedDevices table;
        state_dev StateConnect = DEV_DISCONNECT;
        QModbusClient *modbus;
        QMdiArea      *mdiArea;
        MWS           *dialog;
        MyQMdiSubWindow *subWin;
    }struct_DialofInfo;
    QVector<struct_DialofInfo> vectorDialogs;
};

#endif // DEVICELIBS_H


