#ifndef SYSTEMEVENTS_H
#define SYSTEMEVENTS_H

#include <QObject>

class SystemEvents : public QObject
{
    Q_OBJECT
public:
    static SystemEvents *instance();
    enum GeneralEvent {KLosingData=0, KRunningSmooth, KSoftProblem, KGeneralUnknowState};
    enum ExportEvent {KExportingNow=0, KExportingDone, KExportUnknowState};

private:
    SystemEvents(QObject *parent = 0);

signals:
    void General(SystemEvents::GeneralEvent);
    void Exporting(SystemEvents::ExportEvent);
};
#endif // SYSTEMEVENTS_H
