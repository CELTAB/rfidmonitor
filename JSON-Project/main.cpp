#include <QCoreApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "synchronizationpacket.h"

bool loadJSON()
{
    QFile loadFile(QStringLiteral("save.json"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
//    read(loadDoc.object());

    return true;
}

bool saveJSON(const QJsonObject &gameObject)
{
    QFile saveFile(QStringLiteral("save.json"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SynchronizationPacket packet;
    packet.setId(1);
    packet.setName("pontocoleta 1");
    packet.setMacAddress("BC:5F:F4:A5:20:19");
    packet.setIpAddress("192.168.1.15");

    DataSummary dataSummary;
    dataSummary.setIdBegin(1);
    dataSummary.setIdEnd(100);
    dataSummary.setMd5diggest("asdf1234asdf1234");

    QList<Data> dataList;
    for(int i = 0; i < 50; i++){
        Data d;
        d.setId(i + 1);
        d.setIdantena(2);
        d.setIdcollectorPoint(1234);
        d.setIdentificationCode(9900114433);
        d.setDateTime(QDateTime::currentDateTime());
        dataList.append(d);
    }
    dataSummary.setData(dataList);
    packet.setDataContent(dataSummary);

    QJsonObject obj;
    packet.write(obj);
    saveJSON(obj);

    int option = 1;
    switch (option) {
    case 1:

        break;
    case 2:

        break;
    default:
        break;
    }

    return a.exec();
}
