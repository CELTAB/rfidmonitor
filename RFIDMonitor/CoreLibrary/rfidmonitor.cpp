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

#include <coremodule.h>
#include <logger.h>

#include "servicemanager.h"
#include "core/service.h"
#include "core/interfaces.h"
#include "applicationsettings.h"
#include "rfidmonitor.h"


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

    bool stop;
    QMutex mutex;

    QString moduleName;
    QString device;
    QList<CoreModule *> moduleList;

    void loadModules()
    {
        QDir pluginsDir(qApp->applicationDirPath());
        pluginsDir.cd("modules");

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));

            Logger::instance()->writeRecord(Logger::info, moduleName, Q_FUNC_INFO, QString("Load module %1").arg(fileName));

            QObject *plugin = loader.instance();

            if(!loader.isLoaded()){
                Logger::instance()->writeRecord(Logger::critical, moduleName, Q_FUNC_INFO, QString("Error to load module %1").arg(fileName));
            }

            if (plugin){
                CoreModule *coreMod = qobject_cast< CoreModule * >(plugin);
                if(coreMod){
                    moduleList.append(coreMod);
                }
            }
        }
        Logger::instance()->writeRecord(Logger::info, moduleName, Q_FUNC_INFO, "All Modules Loaded");
    }

    void loadDefaultServices()
    {
        QMap<ServiceType, QString>::iterator i;
        for (i = defaultServiceNames.begin(); i != defaultServiceNames.end(); ++i){
            switch (i.key()) {
            case ServiceType::KReadingService:
                defaultReading = readingServiceList.value(i.value());
                break;
            case ServiceType::KPersistenceService:
                defaultPersistence = persistenceServiceList.value(i.value());
                break;
            case ServiceType::KCommunicationService:
                defaultCommunication = communicationServiceList.value(i.value());
                break;
            case ServiceType::KExportService:
                defaultExport = exportServiceList.value(i.value());
                break;
            case ServiceType::KPackagerService:
                defaultPackager = packagerServiceList.value(i.value());
                break;
            case ServiceType::KSynchronizeService:
                defaultSynchronization = synchronizationServiceList.value(i.value());
                break;
            default:
                break;
            }
        }
    }

    void addService(Service *serv)
    {
        switch (serv->type()) {
        case ServiceType::KReadingService:
            readingServiceList.insert(serv->serviceName(), qobject_cast<ReadingInterface *>(serv));
            break;
        case ServiceType::KPersistenceService:
            persistenceServiceList.insert(serv->serviceName(), qobject_cast<PersistenceInterface *>(serv));
            break;
        case ServiceType::KCommunicationService:
            communicationServiceList.insert(serv->serviceName(), qobject_cast<CommunicationInterface *>(serv));
            break;
        case ServiceType::KExportService:
            exportServiceList.insert(serv->serviceName(), qobject_cast<ExportInterface *>(serv));
            break;
        case ServiceType::KPackagerService:
            packagerServiceList.insert(serv->serviceName(), qobject_cast<PackagerInterface *>(serv));
            break;
        case ServiceType::KSynchronizeService:
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
        case ServiceType::KReadingService:
            foreach (Service *ri, readingServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KPersistenceService:
            foreach (Service *ri, persistenceServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KCommunicationService:
            foreach (Service *ri, communicationServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KExportService:
            foreach (Service *ri, exportServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KPackagerService:
            foreach (Service *ri, packagerServiceList) {
                list.append(ri);
            }
            break;
        case ServiceType::KSynchronizeService:
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
        case ServiceType::KReadingService:
            return defaultReading;
            break;
        case ServiceType::KPersistenceService:
            return defaultPersistence;
            break;
        case ServiceType::KCommunicationService:
            return defaultCommunication;
            break;
        case ServiceType::KExportService:
            return defaultExport;
            break;
        case ServiceType::KPackagerService:
            return defaultPackager;
            break;
        case ServiceType::KSynchronizeService:
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
    Logger::instance()->writeRecord(Logger::info, "Main", Q_FUNC_INFO, "System started");
    d_ptr->moduleName = "Main";
    d_ptr->persistenceThread = new QThread(this);
    d_ptr->syncronizationThread = new QThread(this);
}


RFIDMonitor::~RFIDMonitor()
{
    delete d_ptr;
}


void RFIDMonitor::start(const QCoreApplication &app)
{
    // TEMP
    if(app.arguments().contains("-d") && app.arguments().size() > (app.arguments().indexOf("-d") + 1)){
        d_ptr->device = app.arguments().at(app.arguments().indexOf("-d") + 1);
    }


    d_ptr->loadModules();
    foreach (CoreModule *mod, d_ptr->moduleList) {
        mod->init();
        ApplicationSettings::instance()->installModule(mod);
        foreach (Service *serv, mod->services()) {
            //            d_ptr->services.insert(serv->serviceName(), serv);
            d_ptr->addService(serv);
        }
    }
    // TEMP
    d_ptr->loadDefaultServices();

    // Loads default services information
    ApplicationSettings::instance()->loadDefaultServices();

    // Get Default services instance
    // ReadingService
    QString readingServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KReadingService);
    // PersintenceService
    QString persistenceServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KPersistenceService);
    // CommunicationService
    QString commServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KCommunicationService);
    // ExportService
    QString exportServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KExportService);
    // PackagerService
    QString packagerServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KPackagerService);
    // SynchronizationService
    QString synchronizationServiceName = ApplicationSettings::instance()->defaultServices().value(ServiceType::KSynchronizeService);

    // Loads all Services available
    ReadingInterface *readingService = d_ptr->defaultReading;
    PersistenceInterface *persistenceService = d_ptr->defaultPersistence;
    CommunicationInterface *communicationService = d_ptr->defaultCommunication;
    ExportInterface *exportService = d_ptr->defaultExport;
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


    // Connect the events to listeners
    // When there is new data, the data is sended to the persistence thread
    //    connect(readingService, SIGNAL(dataRaceived(Rfiddata*)), persistenceService, SLOT(insert(Rfiddata*)));
    // The it has to wake up the synchronization thread
//    connect(readingService, SIGNAL(dataRaceived(Rfiddata*)), synchronizationService, SLOT(newData(Rfiddata *)));



    // Messages from the outside must be evaluated in RFIDMonitor
    connect(communicationService, SIGNAL(messageReceived(QByteArray)), SLOT(newMessage(QByteArray)));


    // Start threads
    d_ptr->persistenceThread->start();
    d_ptr->syncronizationThread->start();
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
    case ServiceType::KReadingService:
        return d_ptr->defaultReading;
        break;
    case ServiceType::KPersistenceService:
        return d_ptr->defaultPersistence;
        break;
    case ServiceType::KCommunicationService:
        return d_ptr->defaultCommunication;
        break;
    case ServiceType::KExportService:
        return d_ptr->defaultExport;
        break;
    case ServiceType::KPackagerService:
        return d_ptr->defaultPackager;
        break;
    case ServiceType::KSynchronizeService:
        return d_ptr->defaultSynchronization;
        break;
    default:
        break;
    }
    return 0;
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

void RFIDMonitor::newMessage(QByteArray message)
{
    qDebug() << QString(message);

    if(message == "ExitSystem"){
        qApp->exit(0);
    }else if(message == "RestartSystem"){
        qApp->exit(1);
    }
}

