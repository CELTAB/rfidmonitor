#ifndef READING_H
#define READING_H

#include "../readinginterface.h"

class Reading : public ReadingInterface
{
    Q_OBJECT
public:
    explicit Reading(QObject *parent = 0);

    // ReadingInterface interface
public slots:
    void stop();
    void readData();

    // Service interface
public:
    QString serviceName() const;
    void init();
};

#endif // READING_H
