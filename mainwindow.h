#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCommandLinkButton>
#include "login.h"

#include <qtreewidget.h>

#include <QTimer>
#include <QTime>
#include <QProgressBar>
#include <QLabel>

#include "devicelibs.h"
#include "structs_ui.h"
#include "structs_main.h"

#include "mws.h"

#define LAST_MODBUS_ADRESS 50
#define MODBUS_TIMEOUT_REPLY 75
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

    typedef struct struct_devices
    {
      union_tableRegsRead table;
      struct_ComModbus com;
      QString devicename;
      bool isOpen;
    }struct_devices;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void DevicesSearch();
    void DevicesSearchDisable();

    void DevicesSaved();
    void DevicesRead();

    void searchModbusDevice(QList<QSerialPortInfo> listport);
    void getDeviceModbus(union_tableRegsRead table,struct_ComModbus com,QString nameconnect);
    QString findNameDevice(union_tableRegsRead table);
    void setNameDevice(QString name,int numdev);

    void butSave(int numdev);
    void butLoad(int numdev);

    QString tableToString(struct_listSavedDevices table_point);
    struct_listSavedDevices stringToTable(QString str);
    void strListTotableList();
    void tableListTostrList();

    void pollReplyModbus();
    void pollModbus();
    void pollAdrModbus();

    void additionalChange();

    void actionSaved();

    void treeItemPress(QTreeWidgetItem * item, int column);

    void prepareMenu( const QPoint & pos );

    void LoadLibDevice();
    void ViewSettingsDevice();

   void setIduser(QString str);
   void writeSettings();
private:
    QDockWidget *tree_dock = nullptr;
    QDockWidget *browser_dock = nullptr;
    QDockWidget *list_dock = nullptr;
    QDockWidget *mdi_dock = nullptr;
    QLabel *statbar_NameD;
    QLabel *statbar_Name;
    QLabel *statbar_TypeD;
    QLabel *statbar_Type;
    QLabel *statbar_SerialD;
    QLabel *statbar_Serial;
    QLabel *statbar_AppD;
    QLabel *statbar_App;
    QLabel *statbar_LogD;
    QLabel *statbar_Log;
    QLabel *statbar_ProtcD;
    QLabel *statbar_Protc;
    QLabel *statbar_AdrD;
    QLabel *statbar_Adr;
    QLabel *statbar_PortD;
    QLabel *statbar_Port;

    int textCursor;
    int numDevice;
    int CountRequestAdr;
    int CurentRequestAdr;
    QCommandLinkButton *butlogin = nullptr;
    QString idUser = "0";
private:
    Ui::MainWindow *ui;
    DeviceLibs *libs = nullptr;
    Login *login;
    QTimer *ModbusTimer = nullptr;
private:
    QVector<struct_ComModbus> vectorModbusDevice;
    int intcomModBusDevice;
    int endcomModBusDevice;
    QVector<struct_listSavedDevices> tablListSavedDevices;
    QVector<struct_devices>  tableDevices;
    QVector<int> selectedDevices;
    QList<QString> strListSavedDevices;
};
#endif // MAINWINDOW_H
