#include <QCoreApplication>

#include "systemevents.h"

SystemEvents::SystemEvents(QObject *parent) :
    QObject(parent)
{

}

SystemEvents *SystemEvents::instance()
{
    static SystemEvents *singleton=0;
    if(!singleton){
        singleton = new SystemEvents(qApp);
    }
    return singleton;
}
