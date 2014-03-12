#include "exportservice.h"
#include "logger.h"
//static QThread *m_exportThread = 0;
//static QThread *m_daemonThread = 0;
//static ExportLocalData *m_exporter = 0;

ExportService::ExportService(QObject *parent) :
    ExportInterface(parent)
{

}

ExportService::~ExportService()
{
    stopUSBExport();
}

void ExportService::init()
{

}

QString ExportService::serviceName() const
{
    return "export.service";
}

ServiceType ExportService::type()
{
    return ServiceType::KExportService;
}

void ExportService::startUSBExport()
{

    /*
     * Creates the object of the classes and then move these objects to execute as thread.
     * Make the connection between threads. Starts both threads.
     */
    m_exportThread = new QThread();
    m_exporter = new ExportLocalData();

    m_exporter->moveToThread(m_exportThread);
    QObject::connect(m_exportThread, SIGNAL(started()), m_exporter, SLOT(startExport()));

    m_daemonThread = new QThread();

    DeviceThread::instance()->moveToThread(m_daemonThread);
    QObject::connect(m_daemonThread, SIGNAL(started()), DeviceThread::instance(), SLOT(startListening()));
    QObject::connect(DeviceThread::instance(), SIGNAL(exportToDevice(QString)), m_exporter, SLOT(exportAction(QString)));
    QObject::connect(DeviceThread::instance(), SIGNAL(turnLedOff()), m_exporter, SLOT(turnOffLed()));

    m_exportThread->start();
    m_daemonThread->start();

}

void ExportService::stopUSBExport()
{
    /* First stop the thread. For second delete the object running on the thread.
     * And then delete the thread.
     */
    m_exportThread->exit(0);
    m_exporter->deleteLater();
    m_exportThread->deleteLater();

    m_daemonThread->exit(0);
    m_device->deleteLater();
    m_daemonThread->deleteLater();
}
