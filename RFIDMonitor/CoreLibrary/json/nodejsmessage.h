#ifndef NODEJSMESSAGE_H
#define NODEJSMESSAGE_H

#include <QDateTime>

#include "jsonrwinterface.h"

namespace json {

class NodeJSMessage : public JsonRWInterface
{
public:
    NodeJSMessage();

    // JsonRWInterface interface
public:
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString type() const;
    void setType(const QString &type);

    QString jsonData() const;
    void setJsonData(const QString &jsonData);

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dateTime);

private:
    QString m_type;
    QString m_jsonData;
    QDateTime m_dateTime;
};

}

#endif // NODEJSMESSAGE_H
