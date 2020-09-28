#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <qtreewidget.h>

#include <QTimer>
#include <QTime>

#include "devicelibs.h"
#include "structs_ui.h"

#include "mws.h"

#define LAST_MODBUS_ADRESS 50
#define MODBUS_TIMEOUT_REPLY 20
#define MODBUS_COUNT_REPEAT  1

class QModbusClient;
class QModbusReply;
class DeviceLibs;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void libsAdd();
    void LibsView();
    void DevicesSearch();
    void DevicesSaved();
    void DevicesRead();

    void searchModbusDevice(QList<QSerialPortInfo> listport);
    void getDeviceModbus(union_tableRegsRead table,struct_ComModbus com,QString nameconnect);
    QString findNameDevice(union_tableRegsRead table);
    void setNameDevice(struct_tableRegsRead table,QString name);

    QString tableToString(struct_listSavedDevices table_point);
    struct_listSavedDevices stringToTable(QString str);
    void strListTotableList();
    void tableListTostrList();

    void pollReplyModbus();
    void pollModbus();
    void pollAdrModbus();

    void treeItemChange(QTreeWidgetItem * item, int column);

    void prepareMenu( const QPoint & pos );

    void LoadLibDevice();
private:
    void readSettings();
    void writeSettings();
private:
    QDockWidget *tree_dock = nullptr;
    QDockWidget *browser_dock = nullptr;
    QDockWidget *list_dock = nullptr;
    QDockWidget *mdi_dock = nullptr;
private:
    Ui::MainWindow *ui;
    DeviceLibs *libs = nullptr;
    QTimer *ModbusTimer = nullptr;
    QVector<struct_ComModbus> vectorModbusDevice;
    int intcomModBusDevice;
    int endcomModBusDevice;
    QVector<struct_listSavedDevices> tablListSavedDevices;
    QList<QString> strListSavedDevices;

    QVector<int> testvec;

};
#endif // MAINWINDOW_H
