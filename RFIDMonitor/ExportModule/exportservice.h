#ifndef EXPORTSERVICE_H
#define EXPORTSERVICE_H

#include "devicethread.h"
#include <core/interfaces.h>

class ExportService : public ExportInterface
{
    Q_OBJECT

    // ExportInterface interface
public:
    ExportService(QObject *parent = 0);
    ~ExportService();

    // Service interface
    void startUSBExport();
    void stopUSBExport();
    QString serviceName() const;
    void init();
    ServiceType type();

private:
    QThread *m_daemonThread;
    QThread *m_exportThread;
    ExportLocalData *m_exporter;
    DeviceThread *m_device;

};

#endif // EXPORTSERVICE_H
