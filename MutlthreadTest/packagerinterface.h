#ifndef PACKAGERINTERFACE_H
#define PACKAGERINTERFACE_H

#include "service.h"

class PackagerInterface : public Service
{
    Q_OBJECT
public:
    explicit PackagerInterface(QObject *parent = 0);

    virtual QString getAll() = 0;

signals:

public slots:

};

#endif // PACKAGERINTERFACE_H
