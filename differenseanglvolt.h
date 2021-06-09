#ifndef DIFFERENSEANGLVOLT_H
#define DIFFERENSEANGLVOLT_H

#include <QWidget>

#include <qcustomplot.h>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

#include "struct_lib_dif.h"

#include "dev_base.h"

#define DEEP_GRAPHS_VOLTS_POINTS 50
#define MODBUS_TIMEOUT_PACKET 150
#define MODBUS_COUNT_REPAET 1

#define TIMER_100ms   100
#define TIMER_1000ms  1000

#define CMD_SAVE     (0x43)

namespace Ui {
class DifferenseAnglVolt;
}

class DifferenseAnglVolt : public dev_base
{
    Q_OBJECT
    typedef struct VoltChanels
    {
        uint16_t chanel1;
        uint16_t chanel2;
        uint16_t chanel3;
        uint16_t chanel4;
    }VoltChanels;
    typedef struct AnglChanels
    {
        uint8_t angl_x;
        uint8_t angl_y;
        uint8_t angl_z;
    }AnglChanels;
public:
    explicit DifferenseAnglVolt(dev_base *parent = nullptr);
    ~DifferenseAnglVolt();
public:
    void getTable(struct_listSavedDevices table) override;
    void start(QModbusClient *modbusDev) override;
    void setId(QString str) override;
    void retranslate() override;
    QJsonObject getSetting() override;
    bool setSetting(QJsonObject json, QString idset,QString timeset) override;
private:
    void WritePointsGraphs();
    void InitGraphsVoltsChanel();
    void sendRegs(int startAdr, int countregs);
    void replyReceivedWrite();
    void updateRegs(int startAdr, int countregs);
    void replyReceivedRead();
    void DataToQueue();
    void DataToView();
    void sendCalibration();
    void logSave();
private:
    QTimer *Tim10PerSecond = nullptr;
    QTimer *Tim1PerSecond = nullptr;
    QQueue <VoltChanels> Volts;
    QQueue <AnglChanels> AnglsFirst;
    QQueue <AnglChanels> AnglsSecond;

private: // вектора для каналов c ацп
    QVector<double> graph_amplitudeTIME;
    QVector<double> graph_amplitudeCH1;
    QVector<double> graph_amplitudeCH2;
    QVector<double> graph_amplitudeCH3;
    QVector<double> graph_amplitudeCH4;

    QVector<double> graph_amplitudeANGLX;
    QVector<double> graph_amplitudeANGLY;
    QVector<double> graph_amplitudeANGLZ;

protected:
    bool flgSave = false;
    QXlsx::Document *LogFileXlsx = NULL;
    int XSLXcountLine =1;
    QTime startLogTime;
    QTime endLogTime;
    uinon_tableRegsWriteDIF  Wtable;
    struct_listSavedDevices device;
    QModbusClient *modbusDevice = nullptr;
    QString idUser = "0";
    QDateTime startTime;
private:
    Ui::DifferenseAnglVolt *ui;
};

#endif // DIFFERENSEANGLVOLT_H
