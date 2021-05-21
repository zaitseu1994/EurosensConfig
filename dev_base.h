#ifndef DEV_BASE_H
#define DEV_BASE_H

#include <QObject>
#include <QWidget>
#include "structs_main.h"
#include "qjsonobject.h"
#include <QModbusClient>
#include "mouseenter.h"

class dev_base : public QWidget
{
    Q_OBJECT
public:
    explicit dev_base(QWidget *parent = nullptr);
    ~dev_base();
signals:
    void DevDisconnect(struct_listSavedDevices device);// сигнал при определении производного класса о потери соединения с датчиком
    void DevReady(struct_listSavedDevices device);// сигнал при готовности устройства к работе
    void DevBusy(struct_listSavedDevices device);// сигнал при неготовности устройства к работе
    void DevSettingAccept(struct_listSavedDevices device,QJsonObject json);// при применении настроек устройству
    void DEVMouseEvent(QString str);// всплывающая подсказка при заходе в поле
    void DEVErrorString(QString str);// всплывающая подсказка при выходе из поля
public:
    virtual void setId(QString str) = 0;// передать id пользователя
    virtual void getTable(struct_listSavedDevices table) =0; // передать классу информацию об устройстве
    virtual void start(QModbusClient *modbusDev) =0;// старт работы
    virtual void retranslate() =0;// перевод класса
    virtual QJsonObject getSetting()=0;// получить настройки устройства в json
    virtual bool setSetting(QJsonObject json, QString idset,QString timeset)=0;// установка настроек устройства
protected:
    Mouseenter *MouseEvent =nullptr;
//public:
//    void (*PgetTable)(struct_listSavedDevices table) = NULL;
//    void (*Pstart)(QModbusClient *modbusDev)= NULL;
//    void (*PsetId)(QString str)= NULL;
//    void (*Pretranslate)()= NULL;
//    QJsonObject (*PgetSetting)()= NULL;
//    bool (*PsetSetting)(QJsonObject json, QString idset,QString timeset)= NULL;
};

#endif // DEV_BASE_H
