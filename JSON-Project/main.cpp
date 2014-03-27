#include <QCoreApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

#include "synchronizationpacket.h"

#include "rfidmonitorsettings.h"

bool loadJSON(const QString &fileName, QJsonObject &json)
{
    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    json = loadDoc.object();

    return true;
}

bool saveJSON(const QString &fileName, const QJsonObject &json)
{
    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int option = 2;
    switch (option) {
    case 1:
    {
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
        for(int i = 0; i < 100; i++){
            Data d;
            d.setId(i + 1);
            d.setIdantena(2);
            d.setIdcollectorPoint(1234);
            d.setIdentificationCode(9900114);
            d.setDateTime(QDateTime::currentDateTime());
            dataList.append(d);
        }
        dataSummary.setData(dataList);
        packet.setDataContent(dataSummary);

        QJsonObject obj;
        packet.write(obj);
        saveJSON("sync_package.json",obj);


        RFIDMonitorSettings settings;
        settings.setName("ponto1");
        settings.setId(12);
        settings.setMacAddress("BC:5F:F4:A5:20:19");

        QList<Module> modules;
        for(int i=0; i < 5; i++) {
            Module mod;
            mod.setModuleName(QString("Module %1").arg(i));
            mod.setVersion(1);
            QList<Service> services;
            for(int j=0; j < 5; j++){
                Service serv;
                serv.setServiceType(QString("service_type_%1").arg(j));
                serv.setServiceName(QString("Service %1").arg(j));
                services.append(serv);
            }
            mod.setServices(services);
            modules.append(mod);
        }

        settings.setModules(modules);

        DefaultServices defaultServ;
        defaultServ.setReader("reader.1");
        defaultServ.setPersister("persister.1");
        defaultServ.setExporter("exporter.1");
        defaultServ.setPackager("packager.1");
        defaultServ.setSynchronizer("synchronizer.1");

        settings.setDefaultServices(defaultServ);

        Network net;
        net.setEssid("celtab-wifi");
        net.setPassword("senhaceltab");

        settings.setNetworkConfiguration(net);

        QJsonObject objSettings;
        settings.write(objSettings);
        saveJSON("settings.json", objSettings);
        break;
    }
    case 2:
    {
        QJsonObject syncJson;
        loadJSON("sync_package.json", syncJson);
        SynchronizationPacket sync;
        sync.read(syncJson);

        qDebug() << syncJson.toVariantMap();

        QJsonObject settingsJson;
        loadJSON("settings.json", settingsJson);
        RFIDMonitorSettings settings;
        settings.read(settingsJson);

        qDebug() << settingsJson.toVariantMap();

        break;
    }
    default:
        break;
    }

    return a.exec();
}
