#include <QApplication>
#include <QListView>
#include <QTranslator>
#include <QDebug>

#include "settings.h"
#include "gui/mainwindow.h"
#include "gui/devicemodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RFIDMonitor Desktop Application");

    Settings::instance();

    // Translations for Qt
    QTranslator qtTranslator;
    qDebug() << "INSTALL QT TRANSLATIONS: " << qtTranslator.load("translations/qt/qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);

    // Translations for App
    QTranslator appTranslator;
    qDebug() << "INSTALL QT TRANSLATIONS: "<< appTranslator.load("translations/app/rfidmonitordeskapp_pt_BR" + QLocale::system().name());
    app.installTranslator(&appTranslator);


    MainWindow w;
    w.show();

    return app.exec();
}
