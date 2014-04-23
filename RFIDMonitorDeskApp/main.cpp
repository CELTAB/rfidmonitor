#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QVariant>

#include "settings.h"
#include "gui/mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("RFIDMonitor Desktop");

    //APP TRANSLATION

    QVariant defaultLanguage(Settings::instance()->setting("default-language"));
    /*There is 4 cases:
        defaultLanguage setting does not exist, so will try as auto.
        defaultLanguage setting exists as auto,
        defaultLanguage setting exists and can load translation,
        defaultLanguage setting exists and cannot load translation, so will try as auto.*/

    bool autoLanguageDefined = false;

    if(defaultLanguage.isValid()){ // if the setting exists

        if(defaultLanguage.toString() == "auto") //if the value of setting is defined as auto
            autoLanguageDefined = true; //set to true. Now the app must detect automaticaly the language.

    }else{ //if the setting does not exists
        Settings::instance()->setSetting("default-language", "auto"); //create the setting with auto value.
        autoLanguageDefined = true; //set to true. Now the app must detect automaticaly the language.

    }

    // Translations for Qt
    QTranslator qtTranslator;
    if(autoLanguageDefined){
        qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    }else if( ! qtTranslator.load(":/translations/qt_" + defaultLanguage.toString()) ){
        // if fails to load the language defined on settings,
        // now will try to use the language of the system as the last attempt.
        qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    }
    app.installTranslator(&qtTranslator);

    // Translations for App
    QTranslator appTranslator;
    if(autoLanguageDefined){
        appTranslator.load(":/translations/rfidmonitordeskapp_" + QLocale::system().name());
    }else if( ! appTranslator.load(":/translations/rfidmonitordeskapp_" + defaultLanguage.toString()) ){
        // if fails to load the language defined on settings,
        // now will try to use the language of the system as the last attempt.
        appTranslator.load(":/translations/rfidmonitordeskapp_" + QLocale::system().name());
    }
    app.installTranslator(&appTranslator);


    MainWindow w;
    w.show();

    return app.exec();
}
