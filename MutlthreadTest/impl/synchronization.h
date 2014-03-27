#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "../synchronizationinterface.h"

class Synchronization : public SynchronizationInterface
{
    Q_OBJECT
public:
    explicit Synchronization(QObject *parent = 0);

    // SynchronizationInterface interface
public slots:
    void onDataReceived(QString data);

    // Service interface
public:
    QString serviceName() const;
    void init();
};

#endif // SYNCHRONIZATION_H
