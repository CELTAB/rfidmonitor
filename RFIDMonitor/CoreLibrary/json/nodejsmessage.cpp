#include <QJsonObject>

#include "nodejsmessage.h"

namespace json {

NodeJSMessage::NodeJSMessage()
{
}

void NodeJSMessage::read(const QJsonObject &json)
{
    m_type = json["type"].toString();
    m_jsonData = json["data"].toString();
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
QString NodeJSMessage::jsonData() const
{
    return m_jsonData;
}

void NodeJSMessage::setJsonData(const QString &jsonData)
{
    m_jsonData = jsonData;
}



}

