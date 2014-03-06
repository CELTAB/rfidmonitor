#ifndef MTAPPLICATION_H
#define MTAPPLICATION_H

#include <QObject>
#include <QMap>

class Service;

namespace Constants{
    const QString Reading("READING");
    const QString Synchronizing("SYNCHRONIZATION");
    const QString Exporting("EXPORTING");
    const QString Packaging("PACKAGING");
}

class MTApplication : public QObject
{
    Q_OBJECT
public:
    static MTApplication * instance();
    ~MTApplication();

    void init();

    void registerService(QString key, Service *serv);
    Service * service(QString key);

private:
    explicit MTApplication(QObject *parent = 0);
    QThread *m_exportThread;
    QThread *m_synchronizeThread;

    QMap<QString, Service *> m_services;
};

#endif // MTAPPLICATION_H
