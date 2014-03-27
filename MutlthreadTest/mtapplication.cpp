#include <QCoreApplication>
#include <QThread>

#include <functional>

#include "mtapplication.h"

#include "readinginterface.h"
#include "synchronizationinterface.h"
#include "packagerinterface.h"

MTApplication *MTApplication::instance()
{
    static MTApplication *singleton = 0;
    if(!singleton){
        singleton = new MTApplication(qApp);
    }
    return singleton;
}

MTApplication::~MTApplication()
{
    foreach (Service *serv, m_services.values()) {
        serv->deleteLater();
    }
}


void MTApplication::init()
{
    m_exportThread = new QThread(this);
    m_synchronizeThread = new QThread(this);

    ReadingInterface *reading = qobject_cast<ReadingInterface *>(service(Constants::Reading));
    SynchronizationInterface *synchronizing = qobject_cast<SynchronizationInterface *>(service(Constants::Synchronizing));
    PackagerInterface *packaging = qobject_cast<PackagerInterface *>(service(Constants::Packaging));

    synchronizing->moveToThread(m_synchronizeThread);
    packaging->moveToThread(m_synchronizeThread);


    std::function<void(QString)> dataReceived = std::bind(&SynchronizationInterface::onDataReceived, synchronizing, std::placeholders::_1);
    reading->registerFunction("sync.datareceived", dataReceived);

    m_exportThread->start();
    m_synchronizeThread->start();

    reading->init();

}

void MTApplication::registerService(QString key, Service *serv)
{
    m_services.insert(key, serv);
}

Service *MTApplication::service(QString key)
{
    if(m_services.contains(key)){
        return m_services.value(key);
    }else
        return 0;
}

MTApplication::MTApplication(QObject *parent) :
    QObject(parent)
{
}
