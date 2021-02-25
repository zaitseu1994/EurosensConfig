#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCommandLinkButton>
#include "login.h"

#include <qtreewidget.h>

#include <QTranslator>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QTimer>
#include <QTime>
#include <QProgressBar>
#include <QLabel>

#include "devicelibs.h"
#include "structs_ui.h"
#include "structs_main.h"

#include "mws.h"

#define LAST_MODBUS_ADRESS 30
#define MODBUS_TIMEOUT_REPLY 120
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
      bool isOpen = false;
      QJsonDocument jsonWebDoc;
      QJsonDocument jsonFileDoc;
      bool SetIsEnable = false;
    }struct_devices;

    typedef struct struct_filejsonload
    {
        uint32_t SerialNum;
        uint32_t TypeDevice;
        uint32_t VerApp ;
        uint32_t ID;
        QString filename;
        QDateTime actualTime;
    }struct_filejsonload;


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

    void sendSettingWeb(struct_listSavedDevices table,QJsonObject json);
    void checkSettingWeb(int numDev);
    struct_filejsonload verifyWebJson(QString answStr,QJsonDocument* jsondoc);
    struct_filejsonload chooseFile(int numdev,QFile* file);
    QString butSave(int numdev);

    QString tableToString(struct_listSavedDevices table_point);
    struct_listSavedDevices stringToTable(QString str);
    void strListTotableList();
    void tableListTostrList();

    void pollReplyModbus();
    void pollModbus();
    void pollAdrModbus();

    void webSettingsChange();

    void additionalChange();

    void actionSaved();

    void treeItemPress(QTreeWidgetItem * item, int column);

    void prepareMenu( const QPoint & pos );

    void LoadLibDevice();
    void ViewSettingsDevice();

    void SetLanguage();

   void setIduser(QString str);
   void writeSettings();
   void saveLanguage();
   void restoreLanguage();
protected:
    void changeEvent(QEvent * event) override;
private:
    QTranslator qtLanguageTranslator;
private:
    QMenu *menu_view = nullptr;
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
    QString curLang = "RU";
    QFile fileJsonLoad;
    QJsonDocument JsonFileDocument;

    QNetworkAccessManager *httpNetwork = nullptr;
    QString webserver  = "https://service.mechatronics.by/api/set";
    QString timeformat = "yyyy-MM-dd HH:mm:ss";
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
