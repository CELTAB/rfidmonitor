#ifndef RASPCONFIGURATIONJSON_H
#define RASPCONFIGURATIONJSON_H

#include <QObject>
#include <QString>
#include <QMap>

class RaspConfigurationJson : public QObject
{
    Q_OBJECT
public:
    explicit RaspConfigurationJson(QObject *parent = 0);
    QString getId() const;
    void setId(const QString &value);

    QString getIp() const;
    void setIp(const QString &value);

    QString getName() const;
    void setName(const QString &value);

    QMap<QString, QString> getDefaultServices() const;
    void setDefaultServices(const QMap<QString, QString> &value);
    void insertDefaultService(const QString &serviceType, const QString &serviceName);

    QByteArray generateJson();
    bool loadJson(QByteArray jsonFile);

private:
    QString id;
    QString ip;
    QString name;
    QMap<QString, QString> defaultServices;


signals:

public slots:

};

#endif // RASPCONFIGURATIONJSON_H
