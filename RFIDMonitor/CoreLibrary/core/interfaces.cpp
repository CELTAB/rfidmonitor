#include "interfaces.h"


ReadingInterface::ReadingInterface(QObject *parent) :
    Service(parent)
{
}

ExportInterface::ExportInterface(QObject *parent) :
    Service(parent)
{
}


PackagerInterface::PackagerInterface(QObject *parent) :
    Service(parent)
{
}

SynchronizationInterface::SynchronizationInterface(QObject *parent) :
    Service(parent)
{
}


CommunicationInterface::CommunicationInterface(QObject *parent) :
    Service(parent)
{

}


PersistenceInterface::PersistenceInterface(QObject *parent) :
    Service(parent)
{

}
