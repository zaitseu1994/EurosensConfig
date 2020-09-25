#ifndef DEVICELIBS_H
#define DEVICELIBS_H

#include <QString>
#include <QVector>
#include <QLibrary>
#include <QMdiArea>

#include "structs_ui.h"
#include "structs_main.h"
#include "mws.h"

class DeviceLibs : public QObject
{
    Q_OBJECT
public:
     DeviceLibs();
    ~DeviceLibs();
    bool LibOpen(QString str,QMdiArea *mdiArea,QModbusClient *modbus);
    bool CloseAll();
    void SubWindowClose(QString str);
private:
    typedef struct struct_DialofInfo
    {
        QString str;
        QModbusClient *modbus;
        QMdiArea      *mdiArea;
        MWS           *dialog;
    }struct_DialofInfo;
    QVector<struct_DialofInfo> vectorDialogs;
};

#endif // DEVICELIBS_H
