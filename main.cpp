#include "mainwindow.h"

#include <QScreen>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    const QRect availableGeometry = w.screen()->availableGeometry();
    w.resize(availableGeometry.width() / 1.5, availableGeometry.height() / 1.5);
    w.show();
    return a.exec();
}
