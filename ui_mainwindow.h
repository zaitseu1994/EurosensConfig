/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *add;
    QAction *view;
    QAction *actionSearh;
    QAction *actionSaved;
    QAction *actionCancel;
    QAction *actionConfigure;
    QAction *actionQuit;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QSplitter *splitter_3;
    QTreeWidget *treeWidget;
    QSplitter *splitter_2;
    QSplitter *splitter;
    QMdiArea *mdiArea;
    QListWidget *listInfo;
    QTextBrowser *textBrowser;
    QMenuBar *menubar;
    QMenu *libs;
    QMenu *about;
    QMenu *devices;
    QMenu *Settings;
    QMenu *menu;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(853, 482);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/PNG/images/Logo3.PNG"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        add = new QAction(MainWindow);
        add->setObjectName(QString::fromUtf8("add"));
        view = new QAction(MainWindow);
        view->setObjectName(QString::fromUtf8("view"));
        actionSearh = new QAction(MainWindow);
        actionSearh->setObjectName(QString::fromUtf8("actionSearh"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/PNG/images/search.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSearh->setIcon(icon1);
        actionSaved = new QAction(MainWindow);
        actionSaved->setObjectName(QString::fromUtf8("actionSaved"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/PNG/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaved->setIcon(icon2);
        actionCancel = new QAction(MainWindow);
        actionCancel->setObjectName(QString::fromUtf8("actionCancel"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/PNG/images/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCancel->setIcon(icon3);
        actionConfigure = new QAction(MainWindow);
        actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/PNG/images/settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionConfigure->setIcon(icon4);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/PNG/images/application-exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon5);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter_3 = new QSplitter(centralwidget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        splitter_3->setHandleWidth(1);
        treeWidget = new QTreeWidget(splitter_3);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setMinimumSize(QSize(150, 100));
        treeWidget->setFrameShape(QFrame::NoFrame);
        treeWidget->setLineWidth(0);
        treeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        treeWidget->setTextElideMode(Qt::ElideRight);
        splitter_3->addWidget(treeWidget);
        treeWidget->header()->setCascadingSectionResizes(true);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        mdiArea = new QMdiArea(splitter);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setAutoFillBackground(false);
        mdiArea->setStyleSheet(QString::fromUtf8(""));
        mdiArea->setFrameShape(QFrame::NoFrame);
        mdiArea->setFrameShadow(QFrame::Plain);
        mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        QBrush brush(QColor(170, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        mdiArea->setBackground(brush);
        mdiArea->setViewMode(QMdiArea::SubWindowView);
        mdiArea->setDocumentMode(true);
        mdiArea->setTabShape(QTabWidget::Rounded);
        splitter->addWidget(mdiArea);
        listInfo = new QListWidget(splitter);
        listInfo->setObjectName(QString::fromUtf8("listInfo"));
        listInfo->setFrameShape(QFrame::NoFrame);
        listInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        listInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        splitter->addWidget(listInfo);
        splitter_2->addWidget(splitter);
        textBrowser = new QTextBrowser(splitter_2);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setAutoFillBackground(false);
        textBrowser->setFrameShape(QFrame::NoFrame);
        textBrowser->setFrameShadow(QFrame::Plain);
        textBrowser->setMidLineWidth(0);
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        splitter_2->addWidget(textBrowser);
        splitter_3->addWidget(splitter_2);

        gridLayout->addWidget(splitter_3, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 853, 20));
        libs = new QMenu(menubar);
        libs->setObjectName(QString::fromUtf8("libs"));
        libs->setMouseTracking(true);
        about = new QMenu(menubar);
        about->setObjectName(QString::fromUtf8("about"));
        devices = new QMenu(menubar);
        devices->setObjectName(QString::fromUtf8("devices"));
        Settings = new QMenu(menubar);
        Settings->setObjectName(QString::fromUtf8("Settings"));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMinimumSize(QSize(25, 25));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(devices->menuAction());
        menubar->addAction(libs->menuAction());
        menubar->addAction(Settings->menuAction());
        menubar->addAction(about->menuAction());
        libs->addAction(add);
        libs->addAction(view);
        devices->addAction(actionSearh);
        devices->addAction(actionSaved);
        menu->addAction(actionQuit);
        toolBar->addAction(actionSearh);
        toolBar->addAction(actionSaved);
        toolBar->addAction(actionConfigure);
        toolBar->addAction(actionQuit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "EuroSense", nullptr));
        add->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", nullptr));
        view->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\201\320\274\320\276\321\202\321\200\320\265\321\202\321\214", nullptr));
        actionSearh->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262", nullptr));
        actionSaved->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        actionCancel->setText(QCoreApplication::translate("MainWindow", "\320\236\321\201\321\202\320\260\320\275\320\276\320\262\320\270\321\202\321\214 \320\277\320\276\320\270\321\201\320\272", nullptr));
        actionConfigure->setText(QCoreApplication::translate("MainWindow", "&Configure", nullptr));
#if QT_CONFIG(tooltip)
        actionConfigure->setToolTip(QCoreApplication::translate("MainWindow", "Configure serial port", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionConfigure->setShortcut(QCoreApplication::translate("MainWindow", "Alt+C", nullptr));
#endif // QT_CONFIG(shortcut)
        actionQuit->setText(QCoreApplication::translate("MainWindow", "\320\222\321\213\321\205\320\276\320\264", nullptr));
#if QT_CONFIG(shortcut)
        actionQuit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        libs->setTitle(QCoreApplication::translate("MainWindow", "\320\221\320\270\320\261\320\273\320\270\320\276\321\202\320\265\320\272\320\270", nullptr));
        about->setTitle(QCoreApplication::translate("MainWindow", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", nullptr));
        devices->setTitle(QCoreApplication::translate("MainWindow", "\320\243\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        Settings->setTitle(QCoreApplication::translate("MainWindow", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\320\234\320\265\320\275\321\216", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
