#ifndef MWS_H
#define MWS_H

#include <QWidget>
#include "structs_lib.h"
#include "structs_main.h"
#include "mouseenter.h"
#include <QModbusClient>
#include <QDialog>
#include <QDateTime>

#include <QPointF>

#include <QQueue>
#include <QDateTime>

#include "popup.h"

#include <qcustomplot.h>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

#include "dev_base.h"

using namespace QXlsx;

namespace Ui {
class MWS;
}

class MWS : public dev_base
{
    Q_OBJECT
public:
    explicit MWS(dev_base *parent = nullptr);
    ~MWS();
public:
    void getTable(struct_listSavedDevices table) override;
    void start(QModbusClient *modbusDev) override;
    void setId(QString str) override;
    void retranslate() override;
    QJsonObject getSetting() override;
    bool setSetting(QJsonObject json, QString idset,QString timeset) override;
private:
    void setToolTips();
private:
    QString idUser = "0";
    QDateTime startTime;
private:
    QAction *LogXSLS;
private:
    void logInit();
    void logSave();
    void bootloadTimeout();
    QXlsx::Document LogFileXlsx;
    uint32_t countLine = 1;
    uint32_t countTimesave = 0;
    bool LogEnable = false;
    PopUp *poup;
private:
    typedef enum Action
    {
       NO_ACTION,
       SEND_TO_SAVE_CONFIG,
       SEND_TO_UPDATE_CONFIG,
       SEND_TO_CHECK_PASSWORD,
       WRITE_TABLE,
       READ_TABLE,
       UPDATE_ADDREGS,            // без записи команд в регистр
       SEND_TO_SAVE_FACTORY,
       SEND_DATA_CONNECT
    }Action;

    typedef enum cmd_command
    {
       MODBUS_NO_ACTION            =0,/*!< нет действий*/
       MODBUS_SAVE_CONFIG          =1,/*!< команда сохранить пришедшие параметры настройки и тарировачную таблицу */
       MODBUS_UPDATE_CONFIG        =2,/*!< команда обновить дублирующие параметры настройки и тарировочную таблицу текущими*/
       MODBUS_RECEIVE_PAS          =3,/*!< команда  "Был получен пароль"*/
       MODBUS_CMD_TABLE_CLEAR      =4,/*!< команда   очистки дублирующей таблицы и постановки позиции текущей точки тарированной таблицы в 0*/
       MODBUS_CMD_TABLE_RECEIVE    =5,/*!< команда   отправления под запись точки тарировочной таблицы*/
       MODBUS_CMD_TABLE_POZITION   =6,/*!< команда   постановки позиции текущей точки тарированной таблицы в 0*/
       MODBUS_CMD_TABLE_TRANSMIT   =7,/*!< команда   запроса сохраненой точки тарировочной таблицы*/
       MODBUS_CMD_TABLE_WRITE_END  =8,/*!< команда   окончания записи тарировочной таблицы*/
       MODBUS_CMD_SAVE_FACTORY     =9,/*!< команда   сохранить параметры (serial, type,app ...)*/
       MODBUS_CMD_DATA_CONNECT     =10/*!< команда    принять время соединения*/
    }cmd_command;

    typedef enum cmd_status
    {
      STAT_CONFIG_SAVE              =1,/*!< подтверждение сохранения пришедших параметров настройки и тарировочной таблицы*/
      STAT_CONFIG_UPDATE            =2,/*!< подтверждение обновления дублирующих параметров настройки и тарировочной таблицы*/
      STAT_PAS_RECEIVE              =3,/*!< подтверждение принятия пароля*/
      STAT_CMD_TABLE_CLEAR          =4,/*!< подтверждение очистки дублирующей тарировочной таблицы и установки позиции текущей точки тарировочной таблицы в 0*/
      STAT_CMD_TABLE_RECEIVE        =5,/*!< подтверждение записи точки тарировочной таблицы */
      STAT_CMD_TABLE_POZITION       =6,/*!< подтверждение установки позиции текущей точки тарировочной таблицы в 0*/
      STAT_CMD_TABLE_TRANSMIT       =7,/*!< подтверждение выставления в регистры передачи текущей точки тарировочной таблицы*/
      STAT_CMD_TABLE_END            =8,/*!< окончание тарировочной таблицы*/
      STAT_CMD_SAVE_FACTORY         =9, /*!< подтверждение сохранения параметров (serial, type,app ...)*/
      STAT_CMD_DATA_CONNECT         =10/*!< подтверждение  принятия время соединения*/
    } cmd_status;

    typedef enum type_send
    {
          ONLY_MEASURE,
          RW_TABLE,
          UPDATE_SETTINGS,
          ANOTHER_ACTIONS,
          ADD_TABLE
    }type_send;

    typedef struct struct_pointTableCalibration
    {
                   uint16_t pointDistanse;
                   float pointVolume;
    }struct_pointTableCalibration;

    typedef  enum  stat_readwrite
    {
        NO,
        WRITE,
        READ
    }stat_readwrite;
//signals:
//    void DevDisconnect(struct_listSavedDevices device);
//    void DevReady(struct_listSavedDevices device);
//    void DevBusy(struct_listSavedDevices device);
//    void DevSettingAccept(struct_listSavedDevices device,QJsonObject json);
private:
    Action CurrentAction = NO_ACTION;
    QQueue <Action> queueAction;
    int CounterStepAction;
private:
    struct_listSavedDevices stringToTable(QString str);
    void updateRegs(int startAdr, int countregs);
    void replyReceivedRead();
    void sendRegs(int startAdr, int countregs);
    void replyReceivedWrite();
    stat_readwrite upperModbusCheck();
    void setupAction(Action Action);
    bool isCurrentAction();
    stat_readwrite ActionWriteTable();
    stat_readwrite ActionReadTable();
    stat_readwrite ActionSaveFactory();
    stat_readwrite ActionSaveDataConnect();
    stat_readwrite ActionSaveConfig();
    stat_readwrite ActionReadConfig();
    void readTableWidget();
    void writeTableWidget();

    void readPlotGraph();
    void addLagranj(double maxX,double minX);
    void addLinear(double maxX,double minX);
    void addAmplitude(double distanse);
    void addMeasure(double distatanse,double volume);
    void addPointMeasure(double distatanse,double volume);
    void updatePlotWidget(int s,int k);
    void updateTableWidget(int countPoints);
    void updateFirstSettingsView(union_tableRegsWrite* Table);
    void updateAllSettingsView( union_tableRegsWrite* Table);
    bool updateAllSettingsTable( union_tableRegsWrite *Table);
    void startView();

    qreal Lagranj (double X);
    qreal Linear (double X);
private:
    QQueue <QString> queueSignal;
private:
    union_tableRegsWrite LoclTableRecieve;
    QVector<struct_pointTableCalibration> TableCalibration;

    int currentPointTableCalibration = 0;
private:
    QVector<QPointF> graphTable;

    QVector<double> graph_amplitudeX{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    QVector<double> graph_amplitudeY{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    QCPBars *barGraph;
protected:
    struct_listSavedDevices device;
private:
    Ui::MWS *ui;
    QTimer *ModbusRegsTimer = nullptr;
    QTimer *BootTimeout = nullptr;
    QTimer *LogFileTimer = nullptr;
    QModbusClient *modbusDevice = nullptr;
    bool firstRequest = true;
//    Mouseenter *MouseEvent =nullptr;
};

#endif // MWS_H
