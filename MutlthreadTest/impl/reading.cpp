#include <QTimer>
#include <QThread>

#include "reading.h"

Reading::Reading(QObject *parent) :
    ReadingInterface(parent)
{
}

void Reading::stop()
{
    qDebug() << Q_FUNC_INFO << "Stop reading now!";
}

void Reading::readData()
{
   qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
   static int count = 0;
   emit dataRaceived(QString("Data Number %1").arg(count));
   count++;
   QTimer::singleShot(5000, this, SLOT(readData()));
}

QString Reading::serviceName() const
{
    return "Reading_V1";
}

void Reading::init()
{
    QTimer::singleShot(5000, this, SLOT(readData()));
}
