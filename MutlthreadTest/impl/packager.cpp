#include <QThread>

#include "packager.h"

Packager::Packager(QObject *parent) :
    PackagerInterface(parent)
{
}

QString Packager::getAll()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    return "All information packages";
}

QString Packager::serviceName() const
{
    return "PackagerV1";
}

void Packager::init()
{

}
