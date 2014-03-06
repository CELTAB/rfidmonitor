#ifndef SYNCHRONIZATIONINTERFACE_H
#define SYNCHRONIZATIONINTERFACE_H

#include "service.h"

class SynchronizationInterface : public Service
{
    Q_OBJECT
public:
    explicit SynchronizationInterface(QObject *parent = 0);

signals:
    void sendData(QString data);
public slots:
    virtual void onDataReceived(QString data) = 0;

};

#endif // SYNCHRONIZATIONINTERFACE_H
