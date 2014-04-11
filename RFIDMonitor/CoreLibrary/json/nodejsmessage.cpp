#include <QJsonObject>
#include <QDebug>

#include "nodejsmessage.h"

namespace json {

NodeJSMessage::NodeJSMessage()
{
}

void NodeJSMessage::read(const QJsonObject &json)
{
    m_type = json["type"].toString();
    m_jsonData = json["data"].toObject();
    m_dateTime = json["datetime"].toString();
}

void NodeJSMessage::write(QJsonObject &json) const
{

}
QString NodeJSMessage::type() const
{
    return m_type;
}

void NodeJSMessage::setType(const QString &type)
{
    m_type = type;
}
QJsonObject NodeJSMessage::jsonData() const
{
    return m_jsonData;
}

void NodeJSMessage::setJsonData(const QJsonObject &jsonData)
{
    m_jsonData = jsonData;
}

QString NodeJSMessage::jsonDateTime() const
{
    return m_dateTime;
}

void NodeJSMessage::setJsonDateTime(const QString &dateTime)
{
    m_dateTime = dateTime;
}
}

