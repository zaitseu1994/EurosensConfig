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
    QAction *searh;
    QAction *saved;
    QAction *action;
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
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1039, 540);
        add = new QAction(MainWindow);
        add->setObjectName(QString::fromUtf8("add"));
        view = new QAction(MainWindow);
        view->setObjectName(QString::fromUtf8("view"));
        searh = new QAction(MainWindow);
        searh->setObjectName(QString::fromUtf8("searh"));
        saved = new QAction(MainWindow);
        saved->setObjectName(QString::fromUtf8("saved"));
        action = new QAction(MainWindow);
        action->setObjectName(QString::fromUtf8("action"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        splitter_3 = new QSplitter(centralwidget);
        splitter_3->setObjectName(QString::fromUtf8("splitter_3"));
        splitter_3->setOrientation(Qt::Horizontal);
        treeWidget = new QTreeWidget(splitter_3);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setMinimumSize(QSize(100, 100));
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
        menubar->setGeometry(QRect(0, 0, 1039, 20));
        libs = new QMenu(menubar);
        libs->setObjectName(QString::fromUtf8("libs"));
        libs->setMouseTracking(true);
        about = new QMenu(menubar);
        about->setObjectName(QString::fromUtf8("about"));
        devices = new QMenu(menubar);
        devices->setObjectName(QString::fromUtf8("devices"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setMinimumSize(QSize(30, 30));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(devices->menuAction());
        menubar->addAction(libs->menuAction());
        menubar->addAction(about->menuAction());
        libs->addAction(add);
        libs->addAction(view);
        devices->addAction(searh);
        devices->addAction(saved);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        add->setText(QCoreApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", nullptr));
        view->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\201\320\274\320\276\321\202\321\200\320\265\321\202\321\214", nullptr));
        searh->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262", nullptr));
        saved->setText(QCoreApplication::translate("MainWindow", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", nullptr));
        action->setText(QCoreApplication::translate("MainWindow", "\320\236\321\201\321\202\320\260\320\275\320\276\320\262\320\270\321\202\321\214 \320\277\320\276\320\270\321\201\320\272", nullptr));
        libs->setTitle(QCoreApplication::translate("MainWindow", "\320\221\320\270\320\261\320\273\320\270\320\276\321\202\320\265\320\272\320\270", nullptr));
        about->setTitle(QCoreApplication::translate("MainWindow", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265", nullptr));
        devices->setTitle(QCoreApplication::translate("MainWindow", "\320\243\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
