#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QSqlDatabase>

class Persistence : public QObject
{
    Q_OBJECT
public:
    static Persistence *instance();
    bool persistConfigurationSent(const QString &mac, const QString &ip, const QString &collectPointName, const QByteArray &json);
private:
    explicit Persistence(QObject *parent = 0);
    QSqlDatabase *systemConnection();
    QSqlDatabase m_systemConnection;

signals:

public slots:

};

#endif // PERSISTENCE_H
