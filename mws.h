#ifndef MWS_H
#define MWS_H

#include <QWidget>
#include "structs_lib.h"
#include "structs_main.h"
#include <QModbusClient>
#include <QDialog>

#include <QPointF>

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
    typedef enum Action
    {
       NO_ACTION,
       SEND_TO_SAVE_CONFIG,
       SEND_TO_UPDATE_CONFIG,
       SEND_TO_CHECK_PASSWORD,
       WRITE_TABLE,
       READ_TABLE
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
       MODBUS_CMD_TABLE_WRITE_END  =8 /*!< команда   окончания записи тарировочной таблицы*/
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
    } cmd_status;

    typedef struct struct_pointTableCalibration
    {
                   uint16_t pointDistanse;
                   uint16_t pointVolume;
    }struct_pointTableCalibration;

private:
    Action CurrentAction = NO_ACTION;
    int CounterStepAction;
private:
    struct_listSavedDevices stringToTable(QString str);
    void updateRegs();
    void replyReceivedRead();
    void sendRegs();
    void replyReceivedWrite();
    void upperModbusCheck();
    void setupAction(Action Action);
    void ActionWriteTable();
    void ActionReadTable();
    void readTableWidget();
    void writeTableWidget();
    void updatePlotWidget(int s,int k);
    void updateTableWidget(int countPoints);
    void updateAllSettingsView(union_tableRegsWrite Table);
    void updateAllSettingsTable(union_tableRegsWrite *Table);
    void startView();

    double Lagranj (double X);
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
    bool firstRequest;
};

#endif // MWS_H
