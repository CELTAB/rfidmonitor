#ifndef READINGINTERFACE_H
#define READINGINTERFACE_H

#include "service.h"

class ReadingInterface : public Service
{
    Q_OBJECT
public:
    explicit ReadingInterface(QObject *parent = 0);

signals:
    void dataRaceived(QString);
public slots:
    virtual void stop() = 0;
};

#endif // READINGINTERFACE_H
