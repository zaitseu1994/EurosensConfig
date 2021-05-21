#include "mainwindow.h"

#include <QScreen>
#include <QApplication>
#include <QFile>


int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

//если кто то увидит этот *овнокод не ругайтесь впустую, ругайтесь на zaitseu1994@gmail.com

//    QTranslator qtLanguageTranslator;
//    qtLanguageTranslator.load(QString("QtLanguage_") + QString("en_US"));
//    qApp->installTranslator(&qtLanguageTranslator);

    MainWindow w;
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

