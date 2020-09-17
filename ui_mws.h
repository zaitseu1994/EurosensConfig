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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MWS
{
public:
    QGridLayout *gridLayout;
    QCustomPlot *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_2;
    QLabel *lab_AdrModbus;
    QLabel *lab_TypeApproximation;
    QLabel *lab_TypeAverage;
    QLabel *lab_IntAverage;
    QLabel *lab_Password;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *spin_AdrModbus;
    QSlider *slid_AdrModbus;
    QComboBox *comb_TypeApproximation;
    QComboBox *comb_TypeAverage;
    QHBoxLayout *horizontalLayout_4;
    QSpinBox *spin_IntAverage;
    QSlider *slid_IntAverage;
    QLineEdit *edit_Password;
    QWidget *tab_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_14;
    QHBoxLayout *horizontalLayout_12;
    QVBoxLayout *verticalLayout_4;
    QLabel *lab_IdSensor;
    QLabel *lab_StartMeasure;
    QLabel *lab_EndMeasure;
    QLabel *lab_ReceiverGain;
    QLabel *lab_HWAAS;
    QLabel *lab_RunningAverage;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_9;
    QSpinBox *spin_IdSensor;
    QSlider *slid_IdSensor;
    QHBoxLayout *horizontalLayout_6;
    QSpinBox *spin_StartMeasure;
    QSlider *slid_StartMeasure;
    QHBoxLayout *horizontalLayout_10;
    QSpinBox *spin_EndMeasure;
    QSlider *slid_EndMeasure;
    QHBoxLayout *horizontalLayout_7;
    QDoubleSpinBox *spin_ReceiverGain;
    QSlider *slid_ReceiverGain;
    QHBoxLayout *horizontalLayout_11;
    QSpinBox *spin_HWAAS;
    QSlider *slid_HWAAS;
    QHBoxLayout *horizontalLayout_8;
    QDoubleSpinBox *spin_RunningAverage;
    QSlider *slid_RunningAverage;
    QHBoxLayout *horizontalLayout_13;
    QVBoxLayout *verticalLayout_6;
    QLabel *lab_Noize;
    QLabel *lab_DownSampFactor;
    QLabel *lab_MaximizeSignal;
    QLabel *lab_PowerSaveMode;
    QLabel *lab_AsynchMeasure;
    QLabel *lab_RepetitionMode;
    QLabel *lab_TXDisable;
    QLabel *lab_Profile;
    QVBoxLayout *verticalLayout_7;
    QComboBox *comb_Noize;
    QComboBox *comb_DownSampFactor;
    QComboBox *comb_MaximizeSignal;
    QComboBox *comb_PowerSaveMode;
    QComboBox *comb_AsynchMeasure;
    QComboBox *comb_RepetitionMode;
    QComboBox *comb_TXDisable;
    QComboBox *comb_Profile;
    QWidget *tab_3;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_15;
    QLabel *lab_CountPoint;
    QSpinBox *spin_CountPoint;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_16;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *button_SendTable;
    QPushButton *button_ReseiveTable;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *button_Update;
    QPushButton *button_Accept;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_5;
    QLabel *lab_Distance;
    QLCDNumber *lcd_Distance;
    QLabel *lab_Volume;
    QLCDNumber *lcd_Volume1;
    QCustomPlot *widget_4;

    void setupUi(QWidget *MWS)
    {
        if (MWS->objectName().isEmpty())
            MWS->setObjectName(QString::fromUtf8("MWS"));
        MWS->resize(731, 549);
        gridLayout = new QGridLayout(MWS);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget_2 = new QCustomPlot(MWS);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));

        gridLayout->addWidget(widget_2, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(MWS);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_3 = new QGridLayout(tab);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lab_AdrModbus = new QLabel(tab);
        lab_AdrModbus->setObjectName(QString::fromUtf8("lab_AdrModbus"));

        verticalLayout_2->addWidget(lab_AdrModbus);

        lab_TypeApproximation = new QLabel(tab);
        lab_TypeApproximation->setObjectName(QString::fromUtf8("lab_TypeApproximation"));

        verticalLayout_2->addWidget(lab_TypeApproximation);

        lab_TypeAverage = new QLabel(tab);
        lab_TypeAverage->setObjectName(QString::fromUtf8("lab_TypeAverage"));

        verticalLayout_2->addWidget(lab_TypeAverage);

        lab_IntAverage = new QLabel(tab);
        lab_IntAverage->setObjectName(QString::fromUtf8("lab_IntAverage"));

        verticalLayout_2->addWidget(lab_IntAverage);

        lab_Password = new QLabel(tab);
        lab_Password->setObjectName(QString::fromUtf8("lab_Password"));

        verticalLayout_2->addWidget(lab_Password);


        horizontalLayout_5->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        spin_AdrModbus = new QSpinBox(tab);
        spin_AdrModbus->setObjectName(QString::fromUtf8("spin_AdrModbus"));
        spin_AdrModbus->setMaximum(247);

        horizontalLayout_3->addWidget(spin_AdrModbus);

        slid_AdrModbus = new QSlider(tab);
        slid_AdrModbus->setObjectName(QString::fromUtf8("slid_AdrModbus"));
        slid_AdrModbus->setMaximum(247);
        slid_AdrModbus->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(slid_AdrModbus);

        horizontalLayout_3->setStretch(0, 2);
        horizontalLayout_3->setStretch(1, 5);

        verticalLayout_3->addLayout(horizontalLayout_3);

        comb_TypeApproximation = new QComboBox(tab);
        comb_TypeApproximation->setObjectName(QString::fromUtf8("comb_TypeApproximation"));

        verticalLayout_3->addWidget(comb_TypeApproximation);

        comb_TypeAverage = new QComboBox(tab);
        comb_TypeAverage->setObjectName(QString::fromUtf8("comb_TypeAverage"));

        verticalLayout_3->addWidget(comb_TypeAverage);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        spin_IntAverage = new QSpinBox(tab);
        spin_IntAverage->setObjectName(QString::fromUtf8("spin_IntAverage"));
        spin_IntAverage->setMaximum(20);

        horizontalLayout_4->addWidget(spin_IntAverage);

        slid_IntAverage = new QSlider(tab);
        slid_IntAverage->setObjectName(QString::fromUtf8("slid_IntAverage"));
        slid_IntAverage->setMaximum(20);
        slid_IntAverage->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(slid_IntAverage);

        horizontalLayout_4->setStretch(0, 2);
        horizontalLayout_4->setStretch(1, 5);

        verticalLayout_3->addLayout(horizontalLayout_4);

        edit_Password = new QLineEdit(tab);
        edit_Password->setObjectName(QString::fromUtf8("edit_Password"));

        verticalLayout_3->addWidget(edit_Password);


        horizontalLayout_5->addLayout(verticalLayout_3);


        gridLayout_3->addLayout(horizontalLayout_5, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_2 = new QGridLayout(tab_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        lab_IdSensor = new QLabel(tab_2);
        lab_IdSensor->setObjectName(QString::fromUtf8("lab_IdSensor"));
        QFont font;
        font.setPointSize(7);
        lab_IdSensor->setFont(font);

        verticalLayout_4->addWidget(lab_IdSensor);

        lab_StartMeasure = new QLabel(tab_2);
        lab_StartMeasure->setObjectName(QString::fromUtf8("lab_StartMeasure"));
        lab_StartMeasure->setFont(font);

        verticalLayout_4->addWidget(lab_StartMeasure);

        lab_EndMeasure = new QLabel(tab_2);
        lab_EndMeasure->setObjectName(QString::fromUtf8("lab_EndMeasure"));
        lab_EndMeasure->setFont(font);

        verticalLayout_4->addWidget(lab_EndMeasure);

        lab_ReceiverGain = new QLabel(tab_2);
        lab_ReceiverGain->setObjectName(QString::fromUtf8("lab_ReceiverGain"));
        lab_ReceiverGain->setFont(font);

        verticalLayout_4->addWidget(lab_ReceiverGain);

        lab_HWAAS = new QLabel(tab_2);
        lab_HWAAS->setObjectName(QString::fromUtf8("lab_HWAAS"));
        lab_HWAAS->setFont(font);

        verticalLayout_4->addWidget(lab_HWAAS);

        lab_RunningAverage = new QLabel(tab_2);
        lab_RunningAverage->setObjectName(QString::fromUtf8("lab_RunningAverage"));
        lab_RunningAverage->setFont(font);

        verticalLayout_4->addWidget(lab_RunningAverage);


        horizontalLayout_12->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        spin_IdSensor = new QSpinBox(tab_2);
        spin_IdSensor->setObjectName(QString::fromUtf8("spin_IdSensor"));

        horizontalLayout_9->addWidget(spin_IdSensor);

        slid_IdSensor = new QSlider(tab_2);
        slid_IdSensor->setObjectName(QString::fromUtf8("slid_IdSensor"));
        slid_IdSensor->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(slid_IdSensor);

        horizontalLayout_9->setStretch(0, 2);
        horizontalLayout_9->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_9);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        spin_StartMeasure = new QSpinBox(tab_2);
        spin_StartMeasure->setObjectName(QString::fromUtf8("spin_StartMeasure"));
        spin_StartMeasure->setMinimum(-70);
        spin_StartMeasure->setMaximum(700);

        horizontalLayout_6->addWidget(spin_StartMeasure);

        slid_StartMeasure = new QSlider(tab_2);
        slid_StartMeasure->setObjectName(QString::fromUtf8("slid_StartMeasure"));
        slid_StartMeasure->setMinimum(-70);
        slid_StartMeasure->setMaximum(700);
        slid_StartMeasure->setSingleStep(1);
        slid_StartMeasure->setValue(-70);
        slid_StartMeasure->setOrientation(Qt::Horizontal);

        horizontalLayout_6->addWidget(slid_StartMeasure);

        horizontalLayout_6->setStretch(0, 2);
        horizontalLayout_6->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_6);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        spin_EndMeasure = new QSpinBox(tab_2);
        spin_EndMeasure->setObjectName(QString::fromUtf8("spin_EndMeasure"));
        spin_EndMeasure->setMaximum(770);

        horizontalLayout_10->addWidget(spin_EndMeasure);

        slid_EndMeasure = new QSlider(tab_2);
        slid_EndMeasure->setObjectName(QString::fromUtf8("slid_EndMeasure"));
        slid_EndMeasure->setMaximum(770);
        slid_EndMeasure->setOrientation(Qt::Horizontal);

        horizontalLayout_10->addWidget(slid_EndMeasure);

        horizontalLayout_10->setStretch(0, 2);
        horizontalLayout_10->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_10);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        spin_ReceiverGain = new QDoubleSpinBox(tab_2);
        spin_ReceiverGain->setObjectName(QString::fromUtf8("spin_ReceiverGain"));
        spin_ReceiverGain->setMaximum(1.000000000000000);
        spin_ReceiverGain->setSingleStep(0.010000000000000);

        horizontalLayout_7->addWidget(spin_ReceiverGain);

        slid_ReceiverGain = new QSlider(tab_2);
        slid_ReceiverGain->setObjectName(QString::fromUtf8("slid_ReceiverGain"));
        slid_ReceiverGain->setMaximum(100);
        slid_ReceiverGain->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(slid_ReceiverGain);

        horizontalLayout_7->setStretch(0, 2);
        horizontalLayout_7->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_7);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        spin_HWAAS = new QSpinBox(tab_2);
        spin_HWAAS->setObjectName(QString::fromUtf8("spin_HWAAS"));

        horizontalLayout_11->addWidget(spin_HWAAS);

        slid_HWAAS = new QSlider(tab_2);
        slid_HWAAS->setObjectName(QString::fromUtf8("slid_HWAAS"));
        slid_HWAAS->setOrientation(Qt::Horizontal);

        horizontalLayout_11->addWidget(slid_HWAAS);

        horizontalLayout_11->setStretch(0, 2);
        horizontalLayout_11->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_11);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        spin_RunningAverage = new QDoubleSpinBox(tab_2);
        spin_RunningAverage->setObjectName(QString::fromUtf8("spin_RunningAverage"));
        spin_RunningAverage->setMaximum(1.000000000000000);
        spin_RunningAverage->setSingleStep(0.010000000000000);

        horizontalLayout_8->addWidget(spin_RunningAverage);

        slid_RunningAverage = new QSlider(tab_2);
        slid_RunningAverage->setObjectName(QString::fromUtf8("slid_RunningAverage"));
        slid_RunningAverage->setMaximum(100);
        slid_RunningAverage->setOrientation(Qt::Horizontal);

        horizontalLayout_8->addWidget(slid_RunningAverage);

        horizontalLayout_8->setStretch(0, 2);
        horizontalLayout_8->setStretch(1, 5);

        verticalLayout_5->addLayout(horizontalLayout_8);


        horizontalLayout_12->addLayout(verticalLayout_5);


        horizontalLayout_14->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        lab_Noize = new QLabel(tab_2);
        lab_Noize->setObjectName(QString::fromUtf8("lab_Noize"));
        lab_Noize->setFont(font);

        verticalLayout_6->addWidget(lab_Noize);

        lab_DownSampFactor = new QLabel(tab_2);
        lab_DownSampFactor->setObjectName(QString::fromUtf8("lab_DownSampFactor"));
        lab_DownSampFactor->setFont(font);

        verticalLayout_6->addWidget(lab_DownSampFactor);

        lab_MaximizeSignal = new QLabel(tab_2);
        lab_MaximizeSignal->setObjectName(QString::fromUtf8("lab_MaximizeSignal"));
        lab_MaximizeSignal->setFont(font);

        verticalLayout_6->addWidget(lab_MaximizeSignal);

        lab_PowerSaveMode = new QLabel(tab_2);
        lab_PowerSaveMode->setObjectName(QString::fromUtf8("lab_PowerSaveMode"));
        lab_PowerSaveMode->setFont(font);

        verticalLayout_6->addWidget(lab_PowerSaveMode);

        lab_AsynchMeasure = new QLabel(tab_2);
        lab_AsynchMeasure->setObjectName(QString::fromUtf8("lab_AsynchMeasure"));
        lab_AsynchMeasure->setFont(font);

        verticalLayout_6->addWidget(lab_AsynchMeasure);

        lab_RepetitionMode = new QLabel(tab_2);
        lab_RepetitionMode->setObjectName(QString::fromUtf8("lab_RepetitionMode"));
        lab_RepetitionMode->setFont(font);

        verticalLayout_6->addWidget(lab_RepetitionMode);

        lab_TXDisable = new QLabel(tab_2);
        lab_TXDisable->setObjectName(QString::fromUtf8("lab_TXDisable"));
        lab_TXDisable->setFont(font);

        verticalLayout_6->addWidget(lab_TXDisable);

        lab_Profile = new QLabel(tab_2);
        lab_Profile->setObjectName(QString::fromUtf8("lab_Profile"));
        lab_Profile->setFont(font);

        verticalLayout_6->addWidget(lab_Profile);


        horizontalLayout_13->addLayout(verticalLayout_6);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        comb_Noize = new QComboBox(tab_2);
        comb_Noize->setObjectName(QString::fromUtf8("comb_Noize"));

        verticalLayout_7->addWidget(comb_Noize);

        comb_DownSampFactor = new QComboBox(tab_2);
        comb_DownSampFactor->setObjectName(QString::fromUtf8("comb_DownSampFactor"));

        verticalLayout_7->addWidget(comb_DownSampFactor);

        comb_MaximizeSignal = new QComboBox(tab_2);
        comb_MaximizeSignal->setObjectName(QString::fromUtf8("comb_MaximizeSignal"));

        verticalLayout_7->addWidget(comb_MaximizeSignal);

        comb_PowerSaveMode = new QComboBox(tab_2);
        comb_PowerSaveMode->setObjectName(QString::fromUtf8("comb_PowerSaveMode"));

        verticalLayout_7->addWidget(comb_PowerSaveMode);

        comb_AsynchMeasure = new QComboBox(tab_2);
        comb_AsynchMeasure->setObjectName(QString::fromUtf8("comb_AsynchMeasure"));

        verticalLayout_7->addWidget(comb_AsynchMeasure);

        comb_RepetitionMode = new QComboBox(tab_2);
        comb_RepetitionMode->setObjectName(QString::fromUtf8("comb_RepetitionMode"));

        verticalLayout_7->addWidget(comb_RepetitionMode);

        comb_TXDisable = new QComboBox(tab_2);
        comb_TXDisable->setObjectName(QString::fromUtf8("comb_TXDisable"));

        verticalLayout_7->addWidget(comb_TXDisable);

        comb_Profile = new QComboBox(tab_2);
        comb_Profile->setObjectName(QString::fromUtf8("comb_Profile"));

        verticalLayout_7->addWidget(comb_Profile);


        horizontalLayout_13->addLayout(verticalLayout_7);

        horizontalLayout_13->setStretch(0, 2);
        horizontalLayout_13->setStretch(1, 5);

        horizontalLayout_14->addLayout(horizontalLayout_13);


        gridLayout_2->addLayout(horizontalLayout_14, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_4 = new QGridLayout(tab_3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        lab_CountPoint = new QLabel(tab_3);
        lab_CountPoint->setObjectName(QString::fromUtf8("lab_CountPoint"));

        horizontalLayout_15->addWidget(lab_CountPoint);

        spin_CountPoint = new QSpinBox(tab_3);
        spin_CountPoint->setObjectName(QString::fromUtf8("spin_CountPoint"));

        horizontalLayout_15->addWidget(spin_CountPoint);


        gridLayout_4->addLayout(horizontalLayout_15, 0, 0, 1, 1);

        tableWidget = new QTableWidget(tab_3);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        gridLayout_4->addWidget(tableWidget, 1, 0, 1, 1);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        horizontalSpacer_2 = new QSpacerItem(178, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_2);

        button_SendTable = new QPushButton(tab_3);
        button_SendTable->setObjectName(QString::fromUtf8("button_SendTable"));

        horizontalLayout_16->addWidget(button_SendTable);

        button_ReseiveTable = new QPushButton(tab_3);
        button_ReseiveTable->setObjectName(QString::fromUtf8("button_ReseiveTable"));

        horizontalLayout_16->addWidget(button_ReseiveTable);


        gridLayout_4->addLayout(horizontalLayout_16, 2, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(208, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        button_Update = new QPushButton(MWS);
        button_Update->setObjectName(QString::fromUtf8("button_Update"));

        horizontalLayout->addWidget(button_Update);

        button_Accept = new QPushButton(MWS);
        button_Accept->setObjectName(QString::fromUtf8("button_Accept"));

        horizontalLayout->addWidget(button_Accept);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        groupBox = new QGroupBox(MWS);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_5 = new QGridLayout(groupBox);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        lab_Distance = new QLabel(groupBox);
        lab_Distance->setObjectName(QString::fromUtf8("lab_Distance"));

        gridLayout_5->addWidget(lab_Distance, 0, 0, 1, 1);

        lcd_Distance = new QLCDNumber(groupBox);
        lcd_Distance->setObjectName(QString::fromUtf8("lcd_Distance"));

        gridLayout_5->addWidget(lcd_Distance, 1, 0, 1, 1);

        lab_Volume = new QLabel(groupBox);
        lab_Volume->setObjectName(QString::fromUtf8("lab_Volume"));

        gridLayout_5->addWidget(lab_Volume, 2, 0, 1, 1);

        lcd_Volume1 = new QLCDNumber(groupBox);
        lcd_Volume1->setObjectName(QString::fromUtf8("lcd_Volume1"));

        gridLayout_5->addWidget(lcd_Volume1, 3, 0, 1, 1);

        widget_4 = new QCustomPlot(groupBox);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));

        gridLayout_5->addWidget(widget_4, 4, 0, 1, 1);

        gridLayout_5->setRowStretch(0, 1);
        gridLayout_5->setRowStretch(1, 1);
        gridLayout_5->setRowStretch(2, 1);
        gridLayout_5->setRowStretch(3, 1);
        gridLayout_5->setRowStretch(4, 7);

        horizontalLayout_2->addWidget(groupBox);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 3);

        retranslateUi(MWS);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MWS);
    } // setupUi

    void retranslateUi(QWidget *MWS)
    {
        MWS->setWindowTitle(QCoreApplication::translate("MWS", "Form", nullptr));
        lab_AdrModbus->setText(QCoreApplication::translate("MWS", "\320\220\320\264\321\200\320\265\321\201 \320\274\320\276\320\264\320\261\320\260\321\201", nullptr));
        lab_TypeApproximation->setText(QCoreApplication::translate("MWS", "\320\242\320\270\320\277 \320\260\320\277\320\277\321\200\320\276\320\272\321\201\320\270\320\274\320\260\321\206\320\270\320\270", nullptr));
        lab_TypeAverage->setText(QCoreApplication::translate("MWS", "\320\242\320\270\320\277 \321\203\321\201\321\200\320\265\320\264\320\275\320\265\320\275\320\270\321\217", nullptr));
        lab_IntAverage->setText(QCoreApplication::translate("MWS", "\320\230\320\275\321\202\320\265\321\200\320\262\320\260\320\273 \321\203\321\201\321\200\320\265\320\264\320\275\320\265\320\275\320\270\321\217", nullptr));
        lab_Password->setText(QCoreApplication::translate("MWS", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MWS", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \320\264\320\260\321\202\321\207\320\270\320\272\320\260", nullptr));
        lab_IdSensor->setText(QCoreApplication::translate("MWS", "ID \321\201\320\265\320\275\321\201\320\276\321\200\320\260", nullptr));
        lab_StartMeasure->setText(QCoreApplication::translate("MWS", "<html><head/><body><p>\320\235\320\260\321\207\320\260\320\273\320\276 \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\321\217:<br/>(\320\274\320\274)</p></body></html>", nullptr));
        lab_EndMeasure->setText(QCoreApplication::translate("MWS", "<html><head/><body><p>\320\232\320\276\320\275\320\265\321\206 \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\320\271:<br/>(\320\274\320\274)</p></body></html>", nullptr));
        lab_ReceiverGain->setText(QCoreApplication::translate("MWS", "\320\243\321\201\320\270\320\273\320\265\320\275\320\270\320\265 \321\201\320\270\320\263\320\275\320\260\320\273\320\260:", nullptr));
        lab_HWAAS->setText(QCoreApplication::translate("MWS", "HWAAS:", nullptr));
        lab_RunningAverage->setText(QCoreApplication::translate("MWS", "\320\221\320\265\320\263\321\203\321\211\320\265\320\265 \321\201\321\200\320\265\320\264\320\275\320\265\320\265:", nullptr));
        lab_Noize->setText(QCoreApplication::translate("MWS", "\320\250\321\203\320\274:", nullptr));
        lab_DownSampFactor->setText(QCoreApplication::translate("MWS", "\320\224\320\265\320\273\320\270\321\202\320\265\320\273\321\214:", nullptr));
        lab_MaximizeSignal->setText(QCoreApplication::translate("MWS", "\320\234\320\260\320\272\321\201 \321\201\320\270\320\263\320\275\320\260\320\273:", nullptr));
        lab_PowerSaveMode->setText(QCoreApplication::translate("MWS", "\320\240\320\265\320\266\320\270\320\274 \321\215\320\275\320\265\321\200\320\263\320\276\321\201\320\261\320\265\321\200\320\265\320\266\320\265\320\275\320\270\321\217:", nullptr));
        lab_AsynchMeasure->setText(QCoreApplication::translate("MWS", "\320\220\321\201\320\270\320\275\321\205\321\200. \320\270\320\267\320\274\320\265\321\200\320\265\320\275\320\270\320\265:", nullptr));
        lab_RepetitionMode->setText(QCoreApplication::translate("MWS", "\320\240\320\265\320\266\320\270\320\274 \320\277\320\276\320\262\321\202\320\276\321\200\320\265\320\275\320\270\321\217:", nullptr));
        lab_TXDisable->setText(QCoreApplication::translate("MWS", "\320\222\321\213\320\272\320\273\321\216\321\207\320\270\321\202\321\214 TXD:", nullptr));
        lab_Profile->setText(QCoreApplication::translate("MWS", "\320\237\321\200\320\276\321\204\320\270\320\273\321\214:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MWS", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270 \321\201\320\265\320\275\321\201\320\276\321\200\320\260", nullptr));
        lab_CountPoint->setText(QCoreApplication::translate("MWS", "\320\232\320\276\320\273\320\270\321\207\320\265\321\201\321\202\320\262\320\276 \321\202\320\276\321\207\320\265\320\272", nullptr));
        button_SendTable->setText(QCoreApplication::translate("MWS", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", nullptr));
        button_ReseiveTable->setText(QCoreApplication::translate("MWS", "\320\241\321\207\320\270\321\202\320\260\321\202\321\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("MWS", "\320\242\320\260\321\200\320\270\321\200\320\276\320\262\320\276\321\207\320\275\320\260\321\217 \321\202\320\260\320\261\320\273\320\270\321\206\320\260", nullptr));
        button_Update->setText(QCoreApplication::translate("MWS", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", nullptr));
        button_Accept->setText(QCoreApplication::translate("MWS", "\320\237\321\200\320\270\320\274\320\265\320\275\320\270\321\202\321\214", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MWS", "\320\242\320\265\320\272\321\203\321\211\320\270\320\265 \320\277\320\260\321\200\320\274\320\265\321\202\321\200\321\213", nullptr));
        lab_Distance->setText(QCoreApplication::translate("MWS", "\320\240\320\260\321\201\321\201\321\202\320\276\321\217\320\275\320\270\320\265 \320\274\320\274:", nullptr));
        lab_Volume->setText(QCoreApplication::translate("MWS", "\320\236\320\261\321\214\320\265\320\274 \320\273:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MWS: public Ui_MWS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MWS_H
