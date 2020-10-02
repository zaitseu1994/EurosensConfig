/********************************************************************************
** Form generated from reading UI file 'mws.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MWS_H
#define UI_MWS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MWS
{
public:
    QGridLayout *gridLayout_11;
    QGridLayout *gridLayout_10;
    QSplitter *splitter_3;
    QSplitter *splitter;
    QTabWidget *tabWidget;
    QWidget *settings;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_6;
    QLabel *lab_TypeApproximation;
    QPushButton *button_password;
    QLabel *lab_AdrModbus;
    QLabel *lab_TypeAverage;
    QSlider *slid_IntAverage;
    QLabel *lab_IntAverage;
    QSpinBox *spin_IntAverage;
    QSpinBox *spin_AdrModbus;
    QSlider *slid_AdrModbus;
    QComboBox *comb_TypeApproximation;
    QComboBox *comb_TypeAverage;
    QLineEdit *edit_Password;
    QLabel *lab_Password;
    QWidget *sensor;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QSlider *slid_StartMeasure;
    QSpinBox *spin_EndMeasure;
    QSlider *slid_EndMeasure;
    QSlider *slid_ReceiverGain;
    QLabel *lab_PowerSaveMode;
    QComboBox *comb_PowerSaveMode;
    QSlider *slid_HWAAS;
    QComboBox *comb_AsynchMeasure;
    QSlider *slid_RunningAverage;
    QComboBox *comb_TXDisable;
    QComboBox *comb_Noize;
    QSlider *slid_IdSensor;
    QSpinBox *spin_IdSensor;
    QSpinBox *spin_StartMeasure;
    QDoubleSpinBox *spin_ReceiverGain;
    QSpinBox *spin_HWAAS;
    QDoubleSpinBox *spin_RunningAverage;
    QLabel *lab_IdSensor;
    QLabel *lab_StartMeasure;
    QLabel *lab_EndMeasure;
    QLabel *lab_ReceiverGain;
    QLabel *lab_HWAAS;
    QLabel *lab_RunningAverage;
    QLabel *lab_TXDisable;
    QComboBox *comb_MaximizeSignal;
    QComboBox *comb_DownSampFactor;
    QComboBox *comb_RepetitionMode;
    QComboBox *comb_Profile;
    QLabel *lab_RepetitionMode;
    QLabel *lab_Profile;
    QLabel *lab_AsynchMeasure;
    QLabel *lab_MaximizeSignal;
    QLabel *lab_DownSampFactor;
    QLabel *lab_Noize;
    QWidget *table;
    QGridLayout *gridLayout_8;
    QHBoxLayout *horizontalLayout_15;
    QLabel *lab_CountPoint;
    QSpinBox *spin_CountPoint;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_16;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *button_SendTable;
    QPushButton *button_ReseiveTable;
    QWidget *addit;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_9;
    QLabel *lab_lastconnect;
    QLabel *lab_idchange;
    QLabel *lab_typedev;
    QLabel *lab_serial;
    QLabel *lab_verapp;
    QVBoxLayout *verticalLayout_8;
    QDateTimeEdit *dat_lastconnect;
    QLineEdit *lin_idchange;
    QLineEdit *lin_typedev;
    QLineEdit *lin_serial;
    QLineEdit *lin_verapp;
    QLabel *lab_addit;
    QLineEdit *lin_addit;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *button_savefactory;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout_5;
    QLabel *lab_Distance;
    QLabel *lcd_Distance;
    QLabel *lab_Volume;
    QLabel *lcd_Volume;
    QCustomPlot *graph_distanse;
    QSplitter *splitter_2;
    QCustomPlot *graph_table;
    QGroupBox *group_Approximation;
    QGridLayout *gridLayout;
    QCheckBox *check_Linear;
    QCheckBox *check_Lagranj;
    QSpacerItem *horizontalSpacer;
    QPushButton *button_Update;
    QPushButton *button_Accept;

    void setupUi(QWidget *MWS)
    {
        if (MWS->objectName().isEmpty())
            MWS->setObjectName(QString::fromUtf8("MWS"));
        MWS->resize(536, 463);
        gridLayout_11 = new QGridLayout(MWS);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        gridLayout_10 = new QGridLayout();
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        splitter_3 = new QSplitter(MWS);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Vertical);
        splitter = new QSplitter(splitter_3);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        settings = new QWidget();
        settings->setObjectName(QString::fromUtf8("settings"));
        gridLayout_3 = new QGridLayout(settings);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        lab_TypeApproximation = new QLabel(settings);
        lab_TypeApproximation->setObjectName(QString::fromUtf8("lab_TypeApproximation"));

        gridLayout_6->addWidget(lab_TypeApproximation, 1, 0, 1, 4);

        button_password = new QPushButton(settings);
        button_password->setObjectName(QString::fromUtf8("button_password"));

        gridLayout_6->addWidget(button_password, 4, 7, 1, 2);

        lab_AdrModbus = new QLabel(settings);
        lab_AdrModbus->setObjectName(QString::fromUtf8("lab_AdrModbus"));

        gridLayout_6->addWidget(lab_AdrModbus, 0, 0, 1, 4);

        lab_TypeAverage = new QLabel(settings);
        lab_TypeAverage->setObjectName(QString::fromUtf8("lab_TypeAverage"));

        gridLayout_6->addWidget(lab_TypeAverage, 2, 0, 1, 4);

        slid_IntAverage = new QSlider(settings);
        slid_IntAverage->setObjectName(QString::fromUtf8("slid_IntAverage"));
        slid_IntAverage->setMaximum(20);
        slid_IntAverage->setOrientation(Qt::Horizontal);

        gridLayout_6->addWidget(slid_IntAverage, 3, 7, 1, 2);

        lab_IntAverage = new QLabel(settings);
        lab_IntAverage->setObjectName(QString::fromUtf8("lab_IntAverage"));

        gridLayout_6->addWidget(lab_IntAverage, 3, 0, 1, 4);

        spin_IntAverage = new QSpinBox(settings);
        spin_IntAverage->setObjectName(QString::fromUtf8("spin_IntAverage"));
        spin_IntAverage->setMaximum(20);

        gridLayout_6->addWidget(spin_IntAverage, 3, 4, 1, 3);

        spin_AdrModbus = new QSpinBox(settings);
        spin_AdrModbus->setObjectName(QString::fromUtf8("spin_AdrModbus"));
        spin_AdrModbus->setMaximum(247);

        gridLayout_6->addWidget(spin_AdrModbus, 0, 4, 1, 3);

        slid_AdrModbus = new QSlider(settings);
        slid_AdrModbus->setObjectName(QString::fromUtf8("slid_AdrModbus"));
        slid_AdrModbus->setMaximum(247);
        slid_AdrModbus->setOrientation(Qt::Horizontal);

        gridLayout_6->addWidget(slid_AdrModbus, 0, 7, 1, 2);

        comb_TypeApproximation = new QComboBox(settings);
        comb_TypeApproximation->setObjectName(QString::fromUtf8("comb_TypeApproximation"));

        gridLayout_6->addWidget(comb_TypeApproximation, 1, 4, 1, 5);

        comb_TypeAverage = new QComboBox(settings);
        comb_TypeAverage->setObjectName(QString::fromUtf8("comb_TypeAverage"));

        gridLayout_6->addWidget(comb_TypeAverage, 2, 4, 1, 5);

        edit_Password = new QLineEdit(settings);
        edit_Password->setObjectName(QString::fromUtf8("edit_Password"));

        gridLayout_6->addWidget(edit_Password, 4, 4, 1, 3);

        lab_Password = new QLabel(settings);
        lab_Password->setObjectName(QString::fromUtf8("lab_Password"));

        gridLayout_6->addWidget(lab_Password, 4, 0, 1, 4);


        gridLayout_3->addLayout(gridLayout_6, 0, 0, 1, 1);

        tabWidget->addTab(settings, QString());
        sensor = new QWidget();
        sensor->setObjectName(QString::fromUtf8("sensor"));
        gridLayout_4 = new QGridLayout(sensor);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        slid_StartMeasure = new QSlider(sensor);
        slid_StartMeasure->setObjectName(QString::fromUtf8("slid_StartMeasure"));
        slid_StartMeasure->setMaximumSize(QSize(100, 16777215));
        slid_StartMeasure->setMinimum(-70);
        slid_StartMeasure->setMaximum(700);
        slid_StartMeasure->setSingleStep(1);
        slid_StartMeasure->setValue(-70);
        slid_StartMeasure->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_StartMeasure, 1, 4, 1, 1);

        spin_EndMeasure = new QSpinBox(sensor);
        spin_EndMeasure->setObjectName(QString::fromUtf8("spin_EndMeasure"));
        spin_EndMeasure->setMaximumSize(QSize(50, 16777215));
        spin_EndMeasure->setMaximum(770);

        gridLayout_2->addWidget(spin_EndMeasure, 2, 3, 1, 1);

        slid_EndMeasure = new QSlider(sensor);
        slid_EndMeasure->setObjectName(QString::fromUtf8("slid_EndMeasure"));
        slid_EndMeasure->setMaximumSize(QSize(100, 16777215));
        slid_EndMeasure->setMaximum(770);
        slid_EndMeasure->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_EndMeasure, 2, 4, 1, 1);

        slid_ReceiverGain = new QSlider(sensor);
        slid_ReceiverGain->setObjectName(QString::fromUtf8("slid_ReceiverGain"));
        slid_ReceiverGain->setMaximumSize(QSize(100, 16777215));
        slid_ReceiverGain->setMaximum(100);
        slid_ReceiverGain->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_ReceiverGain, 3, 4, 1, 1);

        lab_PowerSaveMode = new QLabel(sensor);
        lab_PowerSaveMode->setObjectName(QString::fromUtf8("lab_PowerSaveMode"));
        QFont font;
        font.setPointSize(7);
        lab_PowerSaveMode->setFont(font);

        gridLayout_2->addWidget(lab_PowerSaveMode, 3, 5, 1, 3);

        comb_PowerSaveMode = new QComboBox(sensor);
        comb_PowerSaveMode->setObjectName(QString::fromUtf8("comb_PowerSaveMode"));

        gridLayout_2->addWidget(comb_PowerSaveMode, 3, 8, 1, 1);

        slid_HWAAS = new QSlider(sensor);
        slid_HWAAS->setObjectName(QString::fromUtf8("slid_HWAAS"));
        slid_HWAAS->setMaximumSize(QSize(100, 16777215));
        slid_HWAAS->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_HWAAS, 4, 4, 1, 1);

        comb_AsynchMeasure = new QComboBox(sensor);
        comb_AsynchMeasure->setObjectName(QString::fromUtf8("comb_AsynchMeasure"));

        gridLayout_2->addWidget(comb_AsynchMeasure, 4, 8, 1, 1);

        slid_RunningAverage = new QSlider(sensor);
        slid_RunningAverage->setObjectName(QString::fromUtf8("slid_RunningAverage"));
        slid_RunningAverage->setMaximumSize(QSize(100, 16777215));
        slid_RunningAverage->setMaximum(100);
        slid_RunningAverage->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_RunningAverage, 5, 4, 1, 1);

        comb_TXDisable = new QComboBox(sensor);
        comb_TXDisable->setObjectName(QString::fromUtf8("comb_TXDisable"));

        gridLayout_2->addWidget(comb_TXDisable, 6, 3, 1, 2);

        comb_Noize = new QComboBox(sensor);
        comb_Noize->setObjectName(QString::fromUtf8("comb_Noize"));

        gridLayout_2->addWidget(comb_Noize, 0, 8, 1, 1);

        slid_IdSensor = new QSlider(sensor);
        slid_IdSensor->setObjectName(QString::fromUtf8("slid_IdSensor"));
        slid_IdSensor->setMaximumSize(QSize(100, 16777215));
        slid_IdSensor->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(slid_IdSensor, 0, 4, 1, 1);

        spin_IdSensor = new QSpinBox(sensor);
        spin_IdSensor->setObjectName(QString::fromUtf8("spin_IdSensor"));
        spin_IdSensor->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(spin_IdSensor, 0, 3, 1, 1);

        spin_StartMeasure = new QSpinBox(sensor);
        spin_StartMeasure->setObjectName(QString::fromUtf8("spin_StartMeasure"));
        spin_StartMeasure->setMaximumSize(QSize(50, 16777215));
        spin_StartMeasure->setMinimum(-70);
        spin_StartMeasure->setMaximum(700);

        gridLayout_2->addWidget(spin_StartMeasure, 1, 3, 1, 1);

        spin_ReceiverGain = new QDoubleSpinBox(sensor);
        spin_ReceiverGain->setObjectName(QString::fromUtf8("spin_ReceiverGain"));
        spin_ReceiverGain->setMaximumSize(QSize(50, 16777215));
        spin_ReceiverGain->setMaximum(1.000000000000000);
        spin_ReceiverGain->setSingleStep(0.010000000000000);

        gridLayout_2->addWidget(spin_ReceiverGain, 3, 3, 1, 1);

        spin_HWAAS = new QSpinBox(sensor);
        spin_HWAAS->setObjectName(QString::fromUtf8("spin_HWAAS"));
        spin_HWAAS->setMaximumSize(QSize(50, 16777215));

        gridLayout_2->addWidget(spin_HWAAS, 4, 3, 1, 1);

        spin_RunningAverage = new QDoubleSpinBox(sensor);
        spin_RunningAverage->setObjectName(QString::fromUtf8("spin_RunningAverage"));
        spin_RunningAverage->setMaximumSize(QSize(50, 16777215));
        spin_RunningAverage->setMaximum(1.000000000000000);
        spin_RunningAverage->setSingleStep(0.010000000000000);

        gridLayout_2->addWidget(spin_RunningAverage, 5, 3, 1, 1);

        lab_IdSensor = new QLabel(sensor);
        lab_IdSensor->setObjectName(QString::fromUtf8("lab_IdSensor"));
        lab_IdSensor->setFont(font);

        gridLayout_2->addWidget(lab_IdSensor, 0, 0, 1, 3);

        lab_StartMeasure = new QLabel(sensor);
        lab_StartMeasure->setObjectName(QString::fromUtf8("lab_StartMeasure"));
        lab_StartMeasure->setFont(font);

        gridLayout_2->addWidget(lab_StartMeasure, 1, 0, 1, 3);

        lab_EndMeasure = new QLabel(sensor);
        lab_EndMeasure->setObjectName(QString::fromUtf8("lab_EndMeasure"));
        lab_EndMeasure->setFont(font);

        gridLayout_2->addWidget(lab_EndMeasure, 2, 0, 1, 3);

        lab_ReceiverGain = new QLabel(sensor);
        lab_ReceiverGain->setObjectName(QString::fromUtf8("lab_ReceiverGain"));
        lab_ReceiverGain->setFont(font);

        gridLayout_2->addWidget(lab_ReceiverGain, 3, 0, 1, 3);

        lab_HWAAS = new QLabel(sensor);
        lab_HWAAS->setObjectName(QString::fromUtf8("lab_HWAAS"));
        lab_HWAAS->setFont(font);

        gridLayout_2->addWidget(lab_HWAAS, 4, 0, 1, 3);

        lab_RunningAverage = new QLabel(sensor);
        lab_RunningAverage->setObjectName(QString::fromUtf8("lab_RunningAverage"));
        lab_RunningAverage->setFont(font);

        gridLayout_2->addWidget(lab_RunningAverage, 5, 0, 1, 3);

        lab_TXDisable = new QLabel(sensor);
        lab_TXDisable->setObjectName(QString::fromUtf8("lab_TXDisable"));
        lab_TXDisable->setFont(font);

        gridLayout_2->addWidget(lab_TXDisable, 6, 0, 1, 3);

        comb_MaximizeSignal = new QComboBox(sensor);
        comb_MaximizeSignal->setObjectName(QString::fromUtf8("comb_MaximizeSignal"));

        gridLayout_2->addWidget(comb_MaximizeSignal, 2, 8, 1, 1);

        comb_DownSampFactor = new QComboBox(sensor);
        comb_DownSampFactor->setObjectName(QString::fromUtf8("comb_DownSampFactor"));

        gridLayout_2->addWidget(comb_DownSampFactor, 1, 8, 1, 1);

        comb_RepetitionMode = new QComboBox(sensor);
        comb_RepetitionMode->setObjectName(QString::fromUtf8("comb_RepetitionMode"));

        gridLayout_2->addWidget(comb_RepetitionMode, 5, 8, 1, 1);

        comb_Profile = new QComboBox(sensor);
        comb_Profile->setObjectName(QString::fromUtf8("comb_Profile"));

        gridLayout_2->addWidget(comb_Profile, 6, 8, 1, 1);

        lab_RepetitionMode = new QLabel(sensor);
        lab_RepetitionMode->setObjectName(QString::fromUtf8("lab_RepetitionMode"));
        lab_RepetitionMode->setFont(font);

        gridLayout_2->addWidget(lab_RepetitionMode, 5, 5, 1, 3);

        lab_Profile = new QLabel(sensor);
        lab_Profile->setObjectName(QString::fromUtf8("lab_Profile"));
        lab_Profile->setFont(font);

        gridLayout_2->addWidget(lab_Profile, 6, 5, 1, 3);

        lab_AsynchMeasure = new QLabel(sensor);
        lab_AsynchMeasure->setObjectName(QString::fromUtf8("lab_AsynchMeasure"));
        lab_AsynchMeasure->setFont(font);

        gridLayout_2->addWidget(lab_AsynchMeasure, 4, 5, 1, 3);

        lab_MaximizeSignal = new QLabel(sensor);
        lab_MaximizeSignal->setObjectName(QString::fromUtf8("lab_MaximizeSignal"));
        lab_MaximizeSignal->setFont(font);

        gridLayout_2->addWidget(lab_MaximizeSignal, 2, 5, 1, 3);

        lab_DownSampFactor = new QLabel(sensor);
        lab_DownSampFactor->setObjectName(QString::fromUtf8("lab_DownSampFactor"));
        lab_DownSampFactor->setFont(font);

        gridLayout_2->addWidget(lab_DownSampFactor, 1, 5, 1, 3);

        lab_Noize = new QLabel(sensor);
        lab_Noize->setObjectName(QString::fromUtf8("lab_Noize"));
        lab_Noize->setFont(font);

        gridLayout_2->addWidget(lab_Noize, 0, 5, 1, 3);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);

        tabWidget->addTab(sensor, QString());
        table = new QWidget();
        table->setObjectName(QString::fromUtf8("table"));
        gridLayout_8 = new QGridLayout(table);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        lab_CountPoint = new QLabel(table);
        lab_CountPoint->setObjectName(QString::fromUtf8("lab_CountPoint"));

        horizontalLayout_15->addWidget(lab_CountPoint);

        spin_CountPoint = new QSpinBox(table);
        spin_CountPoint->setObjectName(QString::fromUtf8("spin_CountPoint"));

        horizontalLayout_15->addWidget(spin_CountPoint);


        gridLayout_8->addLayout(horizontalLayout_15, 0, 0, 1, 1);

        tableWidget = new QTableWidget(table);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        gridLayout_8->addWidget(tableWidget, 1, 0, 1, 1);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        horizontalSpacer_2 = new QSpacerItem(178, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_2);

        button_SendTable = new QPushButton(table);
        button_SendTable->setObjectName(QString::fromUtf8("button_SendTable"));

        horizontalLayout_16->addWidget(button_SendTable);

        button_ReseiveTable = new QPushButton(table);
        button_ReseiveTable->setObjectName(QString::fromUtf8("button_ReseiveTable"));

        horizontalLayout_16->addWidget(button_ReseiveTable);


        gridLayout_8->addLayout(horizontalLayout_16, 2, 0, 1, 1);

        tabWidget->addTab(table, QString());
        addit = new QWidget();
        addit->setObjectName(QString::fromUtf8("addit"));
        gridLayout_7 = new QGridLayout(addit);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        lab_lastconnect = new QLabel(addit);
        lab_lastconnect->setObjectName(QString::fromUtf8("lab_lastconnect"));

        verticalLayout_9->addWidget(lab_lastconnect);

        lab_idchange = new QLabel(addit);
        lab_idchange->setObjectName(QString::fromUtf8("lab_idchange"));

        verticalLayout_9->addWidget(lab_idchange);

        lab_typedev = new QLabel(addit);
        lab_typedev->setObjectName(QString::fromUtf8("lab_typedev"));

        verticalLayout_9->addWidget(lab_typedev);

        lab_serial = new QLabel(addit);
        lab_serial->setObjectName(QString::fromUtf8("lab_serial"));

        verticalLayout_9->addWidget(lab_serial);

        lab_verapp = new QLabel(addit);
        lab_verapp->setObjectName(QString::fromUtf8("lab_verapp"));

        verticalLayout_9->addWidget(lab_verapp);


        horizontalLayout_2->addLayout(verticalLayout_9);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        dat_lastconnect = new QDateTimeEdit(addit);
        dat_lastconnect->setObjectName(QString::fromUtf8("dat_lastconnect"));

        verticalLayout_8->addWidget(dat_lastconnect);

        lin_idchange = new QLineEdit(addit);
        lin_idchange->setObjectName(QString::fromUtf8("lin_idchange"));

        verticalLayout_8->addWidget(lin_idchange);

        lin_typedev = new QLineEdit(addit);
        lin_typedev->setObjectName(QString::fromUtf8("lin_typedev"));

        verticalLayout_8->addWidget(lin_typedev);

        lin_serial = new QLineEdit(addit);
        lin_serial->setObjectName(QString::fromUtf8("lin_serial"));

        verticalLayout_8->addWidget(lin_serial);

        lin_verapp = new QLineEdit(addit);
        lin_verapp->setObjectName(QString::fromUtf8("lin_verapp"));

        verticalLayout_8->addWidget(lin_verapp);


        horizontalLayout_2->addLayout(verticalLayout_8);


        gridLayout_7->addLayout(horizontalLayout_2, 0, 0, 1, 2);

        lab_addit = new QLabel(addit);
        lab_addit->setObjectName(QString::fromUtf8("lab_addit"));

        gridLayout_7->addWidget(lab_addit, 1, 0, 1, 1);

        lin_addit = new QLineEdit(addit);
        lin_addit->setObjectName(QString::fromUtf8("lin_addit"));

        gridLayout_7->addWidget(lin_addit, 1, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_3, 2, 0, 1, 1);

        button_savefactory = new QPushButton(addit);
        button_savefactory->setObjectName(QString::fromUtf8("button_savefactory"));

        gridLayout_7->addWidget(button_savefactory, 2, 1, 1, 1);

        tabWidget->addTab(addit, QString());
        splitter->addWidget(tabWidget);
        groupBox = new QGroupBox(splitter);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_9 = new QGridLayout(groupBox);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        lab_Distance = new QLabel(groupBox);
        lab_Distance->setObjectName(QString::fromUtf8("lab_Distance"));

        gridLayout_5->addWidget(lab_Distance, 0, 0, 1, 1);

        lcd_Distance = new QLabel(groupBox);
        lcd_Distance->setObjectName(QString::fromUtf8("lcd_Distance"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Segoe Print"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        lcd_Distance->setFont(font1);

        gridLayout_5->addWidget(lcd_Distance, 1, 0, 1, 1);

        lab_Volume = new QLabel(groupBox);
        lab_Volume->setObjectName(QString::fromUtf8("lab_Volume"));

        gridLayout_5->addWidget(lab_Volume, 2, 0, 1, 1);

        lcd_Volume = new QLabel(groupBox);
        lcd_Volume->setObjectName(QString::fromUtf8("lcd_Volume"));
        lcd_Volume->setFont(font1);

        gridLayout_5->addWidget(lcd_Volume, 3, 0, 1, 1);

        graph_distanse = new QCustomPlot(groupBox);
        graph_distanse->setObjectName(QString::fromUtf8("graph_distanse"));

        gridLayout_5->addWidget(graph_distanse, 4, 0, 1, 1);

        gridLayout_5->setRowStretch(0, 1);
        gridLayout_5->setRowStretch(1, 1);
        gridLayout_5->setRowStretch(2, 1);
        gridLayout_5->setRowStretch(3, 1);
        gridLayout_5->setRowStretch(4, 5);

        gridLayout_9->addLayout(gridLayout_5, 0, 0, 1, 1);

        splitter->addWidget(groupBox);
        splitter_3->addWidget(splitter);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        graph_table = new QCustomPlot(splitter_2);
        graph_table->setObjectName(QString::fromUtf8("graph_table"));
        splitter_2->addWidget(graph_table);
        group_Approximation = new QGroupBox(splitter_2);
        group_Approximation->setObjectName(QString::fromUtf8("group_Approximation"));
        gridLayout = new QGridLayout(group_Approximation);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        check_Linear = new QCheckBox(group_Approximation);
        check_Linear->setObjectName(QString::fromUtf8("check_Linear"));

        gridLayout->addWidget(check_Linear, 0, 0, 1, 1);

        check_Lagranj = new QCheckBox(group_Approximation);
        check_Lagranj->setObjectName(QString::fromUtf8("check_Lagranj"));

        gridLayout->addWidget(check_Lagranj, 1, 0, 1, 1);

        splitter_2->addWidget(group_Approximation);
        splitter_3->addWidget(splitter_2);

        gridLayout_10->addWidget(splitter_3, 0, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(376, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_10->addItem(horizontalSpacer, 1, 0, 1, 1);

        button_Update = new QPushButton(MWS);
        button_Update->setObjectName(QString::fromUtf8("button_Update"));

        gridLayout_10->addWidget(button_Update, 1, 1, 1, 1);

        button_Accept = new QPushButton(MWS);
        button_Accept->setObjectName(QString::fromUtf8("button_Accept"));

        gridLayout_10->addWidget(button_Accept, 1, 2, 1, 1);


        gridLayout_11->addLayout(gridLayout_10, 0, 0, 1, 1);


        retranslateUi(MWS);

        tabWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MWS);
    } // setupUi

    void retranslateUi(QWidget *MWS)
    {
        MWS->setWindowTitle(QCoreApplication::translate("MWS", "Form", nullptr));
        lab_TypeApproximation->setText(QCoreApplication::translate("MWS", "\320\242\320\270\320\277 \320\260\320\277\320\277\321\200\320\276\320\272\321\201\320\270\320\274\320\260\321\206\320\270\320\270", nullptr));
        button_password->setText(QCoreApplication::translate("MWS", "\320\222\320\262\320\276\320\264", nullptr));
        lab_AdrModbus->setText(QCoreApplication::translate("MWS", "\320\220\320\264\321\200\320\265\321\201 \320\274\320\276\320\264\320\261\320\260\321\201", nullptr));
        lab_TypeAverage->setText(QCoreApplication::translate("MWS", "\320\242\320\270\320\277 \321\203\321\201\321\200\320\265\320\264\320\275\320\265\320\275\320\270\321\217", nullptr));
        lab_IntAverage->setText(QCoreApplication::translate("MWS", "\320\230\320\275\321\202\320\265\321\200\320\262\320\260\320\273 \321\203\321\201\321\200\320\265\320\264\320\275\320\265\320\275\320\270\321\217", nullptr));
        lab_Password->setText(QCoreApplication::translate("MWS", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(settings), QCoreApplication::translate("MWS", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \320\264\320\260\321\202\321\207\320\270\320\272\320\260", nullptr));
        lab_PowerSaveMode->setText(QCoreApplication::translate("MWS", "\320\240\320\265\320\266\320\270\320\274 \321\215\320\275\320\265\321\200\320\263\320\276\321\201\320\261\320\265\321\200\320\265\320\266\320\265\320\275\320\270\321\217:", nullptr));
        lab_IdSensor->setText(QCoreApplication::translate("MWS", "ID \321\201\320\265\320\275\321\201\320\276\321\200\320\260", nullptr));
        lab_StartMeasure->setText(QCoreApplication::translate("MWS", "<html><head/><body><p>\320\235\320\260\321\207\320\260\320\273\320\276 \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\321\217:<br/>(\320\274\320\274)</p></body></html>", nullptr));
        lab_EndMeasure->setText(QCoreApplication::translate("MWS", "<html><head/><body><p>\320\232\320\276\320\275\320\265\321\206 \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\320\271:<br/>(\320\274\320\274)</p></body></html>", nullptr));
        lab_ReceiverGain->setText(QCoreApplication::translate("MWS", "\320\243\321\201\320\270\320\273\320\265\320\275\320\270\320\265 \321\201\320\270\320\263\320\275\320\260\320\273\320\260:", nullptr));
        lab_HWAAS->setText(QCoreApplication::translate("MWS", "HWAAS:", nullptr));
        lab_RunningAverage->setText(QCoreApplication::translate("MWS", "\320\221\320\265\320\263\321\203\321\211\320\265\320\265 \321\201\321\200\320\265\320\264\320\275\320\265\320\265:", nullptr));
        lab_TXDisable->setText(QCoreApplication::translate("MWS", "\320\222\321\213\320\272\320\273\321\216\321\207\320\270\321\202\321\214 TXD:", nullptr));
        lab_RepetitionMode->setText(QCoreApplication::translate("MWS", "\320\240\320\265\320\266\320\270\320\274 \320\277\320\276\320\262\321\202\320\276\321\200\320\265\320\275\320\270\321\217:", nullptr));
        lab_Profile->setText(QCoreApplication::translate("MWS", "\320\237\321\200\320\276\321\204\320\270\320\273\321\214:", nullptr));
        lab_AsynchMeasure->setText(QCoreApplication::translate("MWS", "\320\220\321\201\320\270\320\275\321\205\321\200. \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\320\265:", nullptr));
        lab_MaximizeSignal->setText(QCoreApplication::translate("MWS", "\320\234\320\260\320\272\321\201 \321\201\320\270\320\263\320\275\320\260\320\273:", nullptr));
        lab_DownSampFactor->setText(QCoreApplication::translate("MWS", "\320\224\320\265\320\273\320\270\321\202\320\265\320\273\321\214:", nullptr));
        lab_Noize->setText(QCoreApplication::translate("MWS", "\320\250\321\203\320\274:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(sensor), QCoreApplication::translate("MWS", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \321\201\320\265\320\275\321\201\320\276\321\200\320\260", nullptr));
        lab_CountPoint->setText(QCoreApplication::translate("MWS", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\202\320\276\321\207\320\265\320\272", nullptr));
        button_SendTable->setText(QCoreApplication::translate("MWS", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", nullptr));
        button_ReseiveTable->setText(QCoreApplication::translate("MWS", "\320\241\321\207\320\270\321\202\320\260\321\202\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(table), QCoreApplication::translate("MWS", "\320\242\320\260\321\200\320\270\321\200\320\276\320\262\320\276\321\207\320\275\320\260\321\217 \321\202\320\260\320\261\320\273\320\270\321\206\320\260", nullptr));
        lab_lastconnect->setText(QCoreApplication::translate("MWS", "\320\224\320\260\321\202\320\260 \320\277\320\276\321\201\320\273\320\265\320\264\320\275\320\265\320\271 \321\201\320\262\321\217\320\267\320\270", nullptr));
        lab_idchange->setText(QCoreApplication::translate("MWS", "<html><head/><body><p>Id \320\277\320\276\320\273\321\214\320\267\320\276\320\262\320\260\321\202\320\265\320\273\321\217 </p></body></html>", nullptr));
        lab_typedev->setText(QCoreApplication::translate("MWS", "\320\242\320\270\320\277 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        lab_serial->setText(QCoreApplication::translate("MWS", "\320\241\320\265\321\200\320\270\320\271\320\275\321\213\320\271 \320\275\320\276\320\274\320\265\321\200 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        lab_verapp->setText(QCoreApplication::translate("MWS", "\320\260\320\277\320\277\320\260\321\200\320\260\321\202\320\275\320\260\321\217 \320\262\320\265\321\200\321\201\320\270\321\217 \321\203\321\201\321\202\321\200\320\276\321\201\321\202\320\262\320\260", nullptr));
        lab_addit->setText(QCoreApplication::translate("MWS", "\320\224\320\276\320\277.\320\277\320\276\320\273\320\265 20 \321\201\320\270\320\274\320\262\320\276\320\273\320\276\320\262 \320\274\320\260\320\272\321\201:", nullptr));
        button_savefactory->setText(QCoreApplication::translate("MWS", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(addit), QCoreApplication::translate("MWS", "\320\227\320\260\320\262\320\276\320\264\321\201\320\272\320\270\320\265", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MWS", "\320\242\320\265\320\272\321\203\321\211\320\270\320\265 \320\277\320\260\321\200\320\274\320\265\321\202\321\200\321\213", nullptr));
        lab_Distance->setText(QCoreApplication::translate("MWS", "\320\240\320\260\321\201\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\274\320\274:", nullptr));
        lcd_Distance->setText(QCoreApplication::translate("MWS", "100", nullptr));
        lab_Volume->setText(QCoreApplication::translate("MWS", "\320\236\320\261\321\214\320\265\320\274 \320\273:", nullptr));
        lcd_Volume->setText(QCoreApplication::translate("MWS", "100", nullptr));
        group_Approximation->setTitle(QCoreApplication::translate("MWS", "\320\220\320\277\320\277\321\200\320\276\320\272\321\201\320\270\320\274\320\260\321\206\320\270\321\217", nullptr));
        check_Linear->setText(QCoreApplication::translate("MWS", "\320\272\321\203\321\201\320\276\321\207\320\275\320\276-\320\273\320\270\320\275\320\265\320\271\320\275\320\260\321\217", nullptr));
        check_Lagranj->setText(QCoreApplication::translate("MWS", "\320\277\320\276\320\273\320\270\320\275.\320\233\320\260\320\263\321\200\320\260\320\275\320\266\320\260", nullptr));
        button_Update->setText(QCoreApplication::translate("MWS", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", nullptr));
        button_Accept->setText(QCoreApplication::translate("MWS", "\320\237\321\200\320\270\320\274\320\265\320\275\320\270\321\202\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MWS: public Ui_MWS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MWS_H
