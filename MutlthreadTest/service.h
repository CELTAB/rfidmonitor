#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QDebug>

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = 0);

    virtual QString serviceName() const = 0;
    virtual void init() = 0;
};

#endif // SERVICE_H
