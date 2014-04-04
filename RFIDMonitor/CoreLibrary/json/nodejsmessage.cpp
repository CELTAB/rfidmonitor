#include<QJsonDocument>
#include <QJsonObject>

#include "nodejsmessage.h"

namespace json {

NodeJSMessage::NodeJSMessage()
{
}

void NodeJSMessage::read(const QJsonObject &json)
{
    m_type = json["type"].toString();
    m_dateTime = QDateTime::fromString(json["datetime"].toString(), Qt::ISODate);
    m_jsonData = QString(QJsonDocument(json["data"].toObject()).toJson());
}

void NodeJSMessage::write(QJsonObject &json) const
{
    json["type"] = m_type;
    json["datetime"] = m_dateTime.toString(Qt::ISODate);

    QJsonObject jsonData = QJsonDocument::fromJson(m_jsonData.toLatin1()).object();
    json["data"] = jsonData;
}

QString NodeJSMessage::type() const
{
    return m_type;
}

void NodeJSMessage::setType(const QString &type)
{
    m_type = type;
}
QString NodeJSMessage::jsonData() const
{
    return m_jsonData;
}

void NodeJSMessage::setJsonData(const QString &jsonData)
{        
    m_jsonData = jsonData;
}
QDateTime NodeJSMessage::dateTime() const
{
    return m_dateTime;
}

void NodeJSMessage::setDateTime(const QDateTime &dateTime)
{
    m_dateTime = dateTime;
}




}

