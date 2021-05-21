QT  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


QT += serialbus widgets
requires(qtConfig(combobox))

qtConfig(modbus-serialport): QT += serialport

QT += widgets printsupport
QT += network

TRANSLATIONS += QtLanguage_en_US.ts

include(QtXlsxWriter/src/xlsx/qtxlsx.pri)

SOURCES += \
    bootform.cpp \
    dev_base.cpp \
    devicelibs.cpp \
    differenseanglvolt.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    mouseenter.cpp \
    mws.cpp \
    popup.cpp \
    qcustomplot.cpp

HEADERS += \
    bootform.h \
    dev_base.h \
    devicelibs.h \
    differenseanglvolt.h \
    login.h \
    mainwindow.h \
    mouseenter.h \
    mws.h \
    popup.h \
    qcustomplot.h \
    struct_lib_dif.h \
    structs_lib.h \
    structs_main.h \
    structs_ui.h \
    ui_mws.h

FORMS += \
    bootform.ui \
    differenseanglvolt.ui \
    login.ui \
    mainwindow.ui \
    mws.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    res.qrc

RC_FILE += myrc.rc
