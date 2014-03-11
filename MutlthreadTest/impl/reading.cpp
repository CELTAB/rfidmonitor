#include <QTimer>
#include <QThread>

#include <future>

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

   try {
       std::function<void (QString)> dataReceived = getFunction<void, QString>("sync.datareceived");
       qulonglong thread = reinterpret_cast<quintptr>(QThread::currentThreadId());
       std::async(std::launch::async, dataReceived, QString("%1 - From Thread %2 - On Thread %3 Using - Using \"std::async\"").arg(count).arg(thread, 0, 16));
       count++;
   } catch (boost::bad_any_cast &ex) {
       qDebug() << Q_FUNC_INFO << "boost::bad_any_cast - " << ex.what();
   } catch (std::exception &e) {
       qDebug() << Q_FUNC_INFO << "std::exception - " << e.what();
   }
   QTimer::singleShot(1000, this, SLOT(readData()));
}

QString Reading::serviceName() const
{
    return "Reading_V1";
}

void Reading::init()
{
    QTimer::singleShot(100, this, SLOT(readData()));
}
