#include <QApplication>
#include <QTranslator>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    appTranslator.load("rfidmonitordesktop_" + QLocale::system().name());
//    appTranslator.load("rfidmonitordesktop_pt-br");

    a.installTranslator(&appTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
