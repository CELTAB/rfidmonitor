#include <QThread>

#include "../mtapplication.h"
#include "../packagerinterface.h"

#include "synchronization.h"

Synchronization::Synchronization(QObject *parent) :
    SynchronizationInterface(parent)
{
}

void Synchronization::onDataReceived(QString data)
{
//    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    qDebug() << data.arg(reinterpret_cast<qulonglong>(QThread::currentThreadId()), 0 , 16);

    PackagerInterface *packager = qobject_cast<PackagerInterface *>(MTApplication::instance()->service(Constants::Packaging));

    QString allData = packager->getAll();

    qDebug() << "All Data: " << allData;
}

QString Synchronization::serviceName() const
{
    return "SynchronizationV1";
}

void Synchronization::init()
{

}
