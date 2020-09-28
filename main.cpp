#include "mainwindow.h"

#include <QScreen>
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    const QRect availableGeometry = w.screen()->availableGeometry();
//    w.resize(availableGeometry.width() / 1.5, availableGeometry.height() / 1.5);
    static const char* const FILE_NAME = "settings.bin";
    QFile file( FILE_NAME );
    QDataStream stream( &file );
    QByteArray state;
    QByteArray window;
    file.open( QIODevice::ReadOnly );
    if (file.isOpen())
    {
        stream >> state;
        stream >> window;
        file.close();
        w.restoreGeometry(window);
        w.restoreState(state);
    }
    w.show();
    return a.exec();
}
