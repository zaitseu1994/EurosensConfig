#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>

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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void libsAdd();
    void LibsView();
    void DevicesSearch();
    void DevicesSaved();

    void getNewDevice(union_tableRegsRead table,int adress);
    void sendModbusRequest(QModbusDataUnit request,int adress);
    void readyModbusRequest();

private:
    Ui::MainWindow *ui;
    QModbusClient *modbusDevice = nullptr;
    QModbusReply *lastRequest = nullptr;
};
#endif // MAINWINDOW_H
