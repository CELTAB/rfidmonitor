#ifndef PACKAGER_H
#define PACKAGER_H

#include "../packagerinterface.h"

class Packager : public PackagerInterface
{
    Q_OBJECT
public:
    explicit Packager(QObject *parent = 0);

signals:

public slots:


    // PackagerInterface interface
public:
    QString getAll();

    // Service interface
public:
    QString serviceName() const;
    void init();
};

#endif // PACKAGER_H
