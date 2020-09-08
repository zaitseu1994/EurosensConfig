#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QSerialPortInfo>

#include <qtreewidget.h>

#define LAST_MODBUS_ADRESS 50

class QModbusClient;
class QModbusReply;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    #pragma pack(push, 1)
    typedef struct struct_tableRegsRead
    {
                   uint16_t TypeDevice; // тип устройства 		        // 2b
                   uint32_t SerialNum;  // серийный номер устройства    // 4b
                   uint32_t VerApp;     // аппаратная версия устройства // 4b
                   uint32_t LastDate;   // дата/время последней связи   // 4b
                   uint16_t LogError;   // код логов ошибок             // 2b
    }struct_tableRegsRead;
    #pragma pack(pop)

    typedef union union_tableRegsRead {
                  struct_tableRegsRead Regs;
                  uint16_t             Adr[sizeof(struct_tableRegsRead)/2];
    }union_tableRegsRead;

    typedef struct struct_ComModbus
    {
        QModbusClient *modbusDev;
        QString        nameCom;
        int            currentAdr;
        QString        description;
        QString        manufacturer;
        quint16        productIdentifier;
        quint16        vendorIdentifier;
    } struct_ComModbus;

    typedef struct struct_listSavedDevices
    {
        struct_tableRegsRead device;
        QString              name;
    }struct_listSavedDevices;

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

    void treeDoubleClick(QTreeWidgetItem * item, int column);
private:
    Ui::MainWindow *ui;
    QModbusReply *lastRequest = nullptr;
    QVector<struct_ComModbus> vectorModbusDevice;
    QVector<struct_listSavedDevices> tablListSavedDevices;
    QList<QString> strListSavedDevices;



};
#endif // MAINWINDOW_H
