/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the FishMonitoring project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QCoreApplication>
#include <QPluginLoader>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <coremodule.h>
#include <logger.h>

#include "core/service.h"
#include "core/interfaces.h"
#include "applicationsettings.h"
#include "rfidmonitor.h"
#include "json/rfidmonitorsettings.h"
#include "json/nodejsmessage.h"


struct RFIDMonitorPrivate
{
    RFIDMonitorPrivate() :
        defaultReading(0),
        defaultPersistence(0),
        defaultCommunication(0),
        defaultExport(0),
        defaultPackager(0),
        defaultSynchronization(0),
        stop(false)
    {

    }

    QMap<QString, ReadingInterface *> readingServiceList;
    QMap<QString, PersistenceInterface *> persistenceServiceList;
    QMap<QString, CommunicationInterface *> communicationServiceList;
    QMap<QString, ExportInterface *> exportServiceList;
    QMap<QString, PackagerInterface *> packagerServiceList;
    QMap<QString, SynchronizationInterface *> synchronizationServiceList;

    ReadingInterface *defaultReading;
    PersistenceInterface *defaultPersistence;
    CommunicationInterface *defaultCommunication;
    ExportInterface *defaultExport;
    PackagerInterface *defaultPackager;
    SynchronizationInterface *defaultSynchronization;

    QMap<ServiceType, QString> defaultServiceNames;

    QThread *persistenceThread;
    QThread *syncronizationThread;
    QThread *exporterThread;

    int collectorId = 0;
    QString collectorName = "";

    bool stop;
    bool connected;
    QMutex mutex;

    QString moduleName;
    QString device;
    QList<CoreModule *> moduleList;

    // RFIDMonitor Settings
    json::RFIDMonitorSettings systemSettings;

    bool readSettings()
    {
        QFile loadFile(QCoreApplication::applicationDirPath() + "/rfidmonitor.json");

        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");
            return false;
        }

        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        systemSettings.read(loadDoc.object());

        device = systemSettings.device();
        collectorId = systemSettings.id();
        collectorName = systemSettings.name();
        loadFile.close();
        return true;
    }

    bool writeSettings()
    {
        QFile saveFile(QCoreApplication::applicationDirPath() + "/rfidmonitor.json");

        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return false;
        }

        QJsonObject obj;
        systemSettings.write(obj);
        QJsonDocument saveDoc(obj);
        saveFile.write(saveDoc.toJson());
        return true;
    }

    void installModule(CoreModule *mod)
    {
        json::Module sysMod;
        sysMod.setModuleName(mod->name());
        sysMod.setVersion(mod->version());
        QList<json::Service> services;
        foreach (Service *s, mod->services()) {
            json::Service serv;
            serv.setServiceName(s->serviceName());
            serv.setServiceType(int(s->type()));
            services.append(serv);
        }
        sysMod.setServices(services);
        QList<json::Module> modules;
        modules.append(sysMod);
        foreach (json::Module module, systemSettings.modules()) {
            if(!(sysMod == module)){
                modules.append(module);
            }
        }
        systemSettings.setModules(modules);
        writeSettings();
    }

    void loadModules()
    {
        QDir pluginsDir(qApp->applicationDirPath());
        // Verify is the modules directory exists; If doesn't close the appication. Without load modules the application can't work
        if(!pluginsDir.cd("modules")){
            Logger::instance()->writeRecord(Logger::severity_level::fatal, moduleName, Q_FUNC_INFO, "Can't find Modules Directory");
            return;
        }

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));

            Logger::instance()->writeRecord(Logger::severity_level::info, moduleName, Q_FUNC_INFO, QString("Load module %1").arg(fileName));

            QObject *plugin = loader.instance();

            if(!loader.isLoaded()){
                Logger::instance()->writeRecord(Logger::severity_level::critical, moduleName, Q_FUNC_INFO, QString("Error to load module %1: %2").arg(fileName).arg(loader.errorString()));
            }

            if (plugin){
                CoreModule *coreMod = qobject_cast< CoreModule * >(plugin);
                if(coreMod){
                    moduleList.append(coreMod);
                }
            }
        }
        Logger::instance()->writeRecord(Logger::severity_level::info, moduleName, Q_FUNC_INFO, "All Modules Loaded");

        foreach (CoreModule *mod, moduleList) {
            mod->init();
            if(systemSettings.modules().contains(json::Module(mod->name()))){
                json::Module sysMod = systemSettings.modules().at(systemSettings.modules().indexOf(json::Module(mod->name())));
                if(sysMod.version() < mod->version()){
                    installModule(mod);
                }
            }else{
                installModule(mod);
            }
            foreach (Service *serv, mod->services()) {
                addService(serv);
            }
        }
    }

    void loadDefaultServices()
    {
        defaultReading = readingServiceList.value(systemSettings.defaultServices().reader());
        if(!defaultReading){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Reder Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Reder Module OK");
        }

        defaultPersistence = persistenceServiceList.value(systemSettings.defaultServices().persister());
        if(!defaultPersistence){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Persistence Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Persistence Module OK");
        }

        defaultCommunication = communicationServiceList.value(systemSettings.defaultServices().communicator());
        if(!defaultCommunication){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Communication Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Communication Module OK");
        }

        defaultExport = exportServiceList.value(systemSettings.defaultServices().exporter());
        if(!defaultExport){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Exporter Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Exporter Module OK");
        }

        defaultPackager = packagerServiceList.value(systemSettings.defaultServices().packager());
        if(!defaultPackager){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Packager Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Packager Module OK");
        }

        defaultSynchronization = synchronizationServiceList.value(systemSettings.defaultServices().synchronizer());
        if(!defaultSynchronization){
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Missing Default Syncronization Module. Exiting now");
            exit(1);
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Default Syncronization Module OK");
        }

        Logger::instance()->writeRecord(Logger::severity_level::info, "RFIDMonitor", Q_FUNC_INFO, "Modules Verification Finished");
    }

    void addService(Service *serv)
    {
        switch (serv->type()) {
        case ServiceType::KReader:
            readingServiceList.insert(serv->serviceName(), qobject_cast<ReadingInterface *>(serv));
            break;
        case ServiceType::KPersister:
            persistenceServiceList.insert(serv->serviceName(), qobject_cast<PersistenceInterface *>(serv));
            break;
        case ServiceType::KCommunicator:
            communicationServiceList.insert(serv->serviceName(), qobject_cast<CommunicationInterface *>(serv));
            break;
        case ServiceType::KExporter:
            exportServiceList.insert(serv->serviceName(), qobject_cast<ExportInterface *>(serv));
            break;
        case ServiceType::KPackager:
            packagerServiceList.insert(serv->serviceName(), qobject_cast<PackagerInterface *>(serv));
            break;
        case ServiceType::KSynchronizer:
            synchronizationServiceList.insert(serv->serviceName(), qobject_cast<SynchronizationInterface *>(serv));
            break;
        default:
            break;
        }
    }

    QList<Service *> services(ServiceType type)
    {
        QList<Service *> list;
        switch (type) {
        case ServiceType::KReader:
            foreach (Service *ri, readingServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KPersister:
            foreach (Service *ri, persistenceServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KCommunicator:
            foreach (Service *ri, communicationServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KExporter:
            foreach (Service *ri, exportServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KPackager:
            foreach (Service *ri, packagerServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KSynchronizer:
            foreach (Service *ri, synchronizationServiceList) {
                list.append(ri);
            }
            break;
        default:
            break;
        }
        return list;
    }

    Service * defaultService(ServiceType type)
    {
        switch (type) {
        case ServiceType::KReader:
            return defaultReading;
            break;
        case ServiceType::KPersister:
            return defaultPersistence;
            break;
        case ServiceType::KCommunicator:
            return defaultCommunication;
            break;
        case ServiceType::KExporter:
            return defaultExport;
            break;
        case ServiceType::KPackager:
            return defaultPackager;
            break;
        case ServiceType::KSynchronizer:
            return defaultSynchronization;
            break;
        default:
            break;
        }
        return 0;
    }
};

RFIDMonitor *RFIDMonitor::instance()
{
    static RFIDMonitor *singleton = 0;
    if(!singleton){
        singleton = new RFIDMonitor(qApp);
    }
    return singleton;
}

RFIDMonitor::RFIDMonitor(QObject *parent) :
    QObject(parent),
    d_ptr(new RFIDMonitorPrivate)
{
    Logger::instance()->writeRecord(Logger::severity_level::info, "Main", Q_FUNC_INFO, "System started");
    d_ptr->moduleName = "Main";
    d_ptr->persistenceThread = new QThread(this);
    d_ptr->syncronizationThread = new QThread(this);
    d_ptr->exporterThread = new QThread(this);
}


RFIDMonitor::~RFIDMonitor()
{
    delete d_ptr;
}

bool RFIDMonitor::isconnected(){
    return d_ptr->connected;
}

void RFIDMonitor::start(const QCoreApplication &app)
{
    d_ptr->connected = false;
    d_ptr->readSettings();
    d_ptr->loadModules();
    d_ptr->loadDefaultServices();

    // Loads all Services available
    ReadingInterface *readingService = d_ptr->defaultReading;
    PersistenceInterface *persistenceService = d_ptr->defaultPersistence;
    CommunicationInterface *communicationService = d_ptr->defaultCommunication;
    ExportInterface *exportService = d_ptr->defaultExport;
    //    (void)exportService;
    PackagerInterface *packagerService = d_ptr->defaultPackager;
    SynchronizationInterface *synchronizationService = d_ptr->defaultSynchronization;

    readingService->setParent(this);

    // Move Services to their respective threads
    persistenceService->setParent(0);
    persistenceService->moveToThread(d_ptr->persistenceThread);
    connect(d_ptr->persistenceThread, SIGNAL(destroyed()), persistenceService, SLOT(deleteLater()));

    packagerService->setParent(0);
    packagerService->moveToThread(d_ptr->syncronizationThread);
    connect(d_ptr->persistenceThread, SIGNAL(destroyed()), packagerService, SLOT(deleteLater()));

    synchronizationService->setParent(0);
    synchronizationService->moveToThread(d_ptr->syncronizationThread);
    connect(d_ptr->persistenceThread, SIGNAL(destroyed()), synchronizationService, SLOT(deleteLater()));

    exportService->setParent(0);
    exportService->moveToThread(d_ptr->exporterThread);
    connect(d_ptr->exporterThread, SIGNAL(started()), exportService, SLOT(startUSBExport()));
    connect(d_ptr->exporterThread, SIGNAL(destroyed()), exportService, SLOT(deleteLater()));

    // Messages from the outside must be evaluated in RFIDMonitor
    connect(communicationService, SIGNAL(messageReceived(QByteArray)), SLOT(newMessage(QByteArray)));

    // Start threads
    d_ptr->persistenceThread->start();
    d_ptr->syncronizationThread->start();
    d_ptr->exporterThread->start();
    readingService->start();
}

const QList<CoreModule *> &RFIDMonitor::moduleList() const
{
    return d_ptr->moduleList;
}

QList<Service *> RFIDMonitor::services(ServiceType type)
{
    return d_ptr->services(type);
}

Service *RFIDMonitor::defaultService(ServiceType type)
{
    switch (type) {
    case ServiceType::KReader:
        return d_ptr->defaultReading;
        break;
    case ServiceType::KPersister:
        return d_ptr->defaultPersistence;
        break;
    case ServiceType::KCommunicator:
        return d_ptr->defaultCommunication;
        break;
    case ServiceType::KExporter:
        return d_ptr->defaultExport;
        break;
    case ServiceType::KPackager:
        return d_ptr->defaultPackager;
        break;
    case ServiceType::KSynchronizer:
        return d_ptr->defaultSynchronization;
        break;
    default:
        break;
    }
    return 0;
}

QVariant RFIDMonitor::getProperty(const QString &propName)
{
    return QVariant();
}

void RFIDMonitor::setDefaultService(ServiceType type, QString name)
{
    d_ptr->defaultServiceNames.insert(type, name);
}

bool RFIDMonitor::isRunning()
{
    QMutexLocker locker(&d_ptr->mutex);
    return d_ptr->stop;

}

QString RFIDMonitor::device()
{
    return d_ptr->device;
}

void RFIDMonitor::stop()
{
    d_ptr->stop = true;
}

int RFIDMonitor::idCollector(){
    return d_ptr->collectorId;
}

QString RFIDMonitor::collectorName()
{
    return d_ptr->collectorName;
}

void RFIDMonitor::newMessage(QByteArray message)
{

    QJsonObject obj = QJsonDocument::fromJson(message).object();
    json::NodeJSMessage nodeJSMessage;
    nodeJSMessage.read(obj);

    if(nodeJSMessage.type() == "SYNC"){

        Logger::instance()->writeRecord(Logger::severity_level::debug, "Main", Q_FUNC_INFO, "Server connected");
        d_ptr->connected = true;
        // The daemon is now connected with server, send the not-synced data
        d_ptr->defaultSynchronization->readyRead();
    }else if(nodeJSMessage.type() == "STOP"){

        // Stop all services and quit system. Used to restart application, but first must to close properly
        d_ptr->defaultExport->stopUSBExport();
        d_ptr->defaultReading->stop();

        Logger::instance()->writeRecord(Logger::severity_level::debug, "Main", Q_FUNC_INFO, "Stoping services");

        QJsonDocument json;
        QJsonObject dObj;
        dObj.insert("type", QString("STOPPED"));
        dObj.insert("data", QJsonValue());
        dObj.insert("datetime", QString(QDateTime::currentDateTime().toString(Qt::ISODate)));
        json.setObject(dObj);

        d_ptr->defaultCommunication->sendMessage(json.toJson());
        QCoreApplication::exit(0);

    }else if(nodeJSMessage.type() == "SLEEP"){
        d_ptr->connected = false;
        Logger::instance()->writeRecord(Logger::severity_level::debug, "Main", Q_FUNC_INFO, "Server Disconnected");

    }else if(nodeJSMessage.type() == "FULL-READ"){

        d_ptr->defaultReading->fullRead(nodeJSMessage.jsonData().value("full").toBool());

    }else if(nodeJSMessage.type() == "READER-COMMAND"){

        QString command = nodeJSMessage.jsonData().value("command").toString();
        d_ptr->defaultReading->write(command);

    }else if(nodeJSMessage.type() == "ACK-DATA"){
        QJsonArray hashArray = nodeJSMessage.jsonData()["md5diggest"].toArray();
        QList<QString> hashList;
        for(int i = 0 ; i<hashArray.size(); i++){
            hashList.append(hashArray[i].toString());
            Logger::instance()->writeRecord(Logger::severity_level::debug, "Main", Q_FUNC_INFO, hashArray[i].toString());
        }
        //        Logger::instance()->writeRecord(Logger::severity_level::debug, "Main", Q_FUNC_INFO, "Packager Update");
        d_ptr->defaultPackager->update(hashList);
    }else{
        //UNKNOWN MESSAGE
        QJsonDocument json;
        QJsonObject unknownObj;
        QJsonObject dObj;

        unknownObj.insert("unknownmessage", QJsonValue(QJsonDocument::fromJson(message).object()));
        unknownObj.insert("errorinfo", QString("Unknown message received"));

        dObj.insert("type", QString("ACK-UNKNOWN"));
        dObj.insert("data", QJsonValue(unknownObj));
        dObj.insert("datetime", QString(QDateTime::currentDateTime().toString(Qt::ISODate)));
        json.setObject(dObj);

        d_ptr->defaultCommunication->sendMessage(json.toJson());
    }
}
