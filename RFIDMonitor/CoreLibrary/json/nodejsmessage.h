#ifndef NODEJSMESSAGE_H
#define NODEJSMESSAGE_H

#include <QJsonObject>
#include <QDateTime>

#include "jsonrwinterface.h"
//#include <CoreLibrary/json/jsonrwinterface.h>

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

    QJsonObject jsonData() const;
    void setJsonData(const QJsonObject &jsonData);

    QString jsonDateTime() const;
    void setJsonDateTime(const QString &dateTime);

private:
    QString m_type;
    QJsonObject m_jsonData;
    QString m_dateTime;
//    QString m_jsonData;
};

}

#endif // NODEJSMESSAGE_H
