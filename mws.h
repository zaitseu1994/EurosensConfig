#ifndef MWS_H
#define MWS_H

#include <QWidget>
#include "structs_lib.h"
#include "structs_main.h"
#include <QModbusClient>
#include <QDialog>

#include <QPointF>

#include <QQueue>
#include <QDateTime>

namespace Ui {
class MWS;
}

class MWS : public QWidget
{
    Q_OBJECT
public:
    explicit MWS(QWidget *parent = nullptr);
    ~MWS();
public:
    void getStr(QString str);
    void start(QModbusClient *modbusDev);
private:
    QString strWindow;
    QString idUser = "1000";
    QDateTime startTime;
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
    void addMeasure(double distatanse,double volume);
    void addPointMeasure(double distatanse,double volume);
    void updatePlotWidget(int s,int k);
    void updateTableWidget(int countPoints);
    void updateAllSettingsView( union_tableRegsWrite Table);
    bool updateAllSettingsTable( union_tableRegsWrite *Table);
    void startView();

    qreal Lagranj (double X);
    qreal Linear (double X);
private:
    union_tableRegsWrite LoclTableRecieve;
    QVector<struct_pointTableCalibration> TableCalibration;

    int currentPointTableCalibration = 0;
private:
    QVector<double> GraphsMainDistanse;
    QVector<double> GraphsMainVolume;

    QVector<QPointF> graphTable;
private:
    Ui::MWS *ui;
    QTimer *ModbusRegsTimer = nullptr;
    struct_listSavedDevices device;
    QModbusClient *modbusDevice = nullptr;
    bool firstRequest = true;
};

#endif // MWS_H
