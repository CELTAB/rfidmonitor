#ifndef JSONRWINTERFACE_H
#define JSONRWINTERFACE_H

#include <QObject>
#include <QVariant>

class QJsonObject;

class JsonRWInterface
{
public:
    virtual ~JsonRWInterface(){}
    virtual void read(const QJsonObject &json) = 0;
    virtual void write(QJsonObject &json) const = 0;
};

#endif // JSONRWINTERFACE_H
