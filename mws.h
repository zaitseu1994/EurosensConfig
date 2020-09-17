#ifndef MWS_H
#define MWS_H

#include <QWidget>
#include "structs_lib.h"
#include "structs_main.h"
#include <QModbusClient>
#include <QDialog>

namespace Ui {
class MWS;
}

class MWS : public QDialog
{
    Q_OBJECT
public:
    explicit MWS(QWidget *parent = nullptr);
    ~MWS();
public:
    void getStr(QString str);
    void start(QModbusClient *modbusDev);
private:
    struct_listSavedDevices stringToTable(QString str);
    void updateRegs();
    void replyReceivedRead();
    void sendRegs();
    void replyReceivedWrite();
    void updateAllSettingsView(union_tableRegsWrite Table);
    void updateMainSettingsView(union_tableRegsWrite Table);
    void updateAllSettingsTable(union_tableRegsWrite *Table);
    void startView();
private:
    Ui::MWS *ui;
    QTimer *ModbusRegsTimer = nullptr;
    struct_listSavedDevices device;
    union_tableRegsWrite LoclTableRecieve;
    QModbusClient *modbusDevice = nullptr;
    bool firstRequest;
};

#endif // MWS_H
