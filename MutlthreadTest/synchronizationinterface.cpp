#include "synchronizationinterface.h"

SynchronizationInterface::SynchronizationInterface(QObject *parent) :
    Service(parent)
{
}

//void SynchronizationInterface::onDataReceived(QString data)
//{
//    qDebug() << Q_FUNC_INFO << data;
//    emit sendData(data);
//}
