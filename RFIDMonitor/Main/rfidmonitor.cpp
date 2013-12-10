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

#include <servicemanager.h>
#include <coremodule.h>
#include <logger.h>

#include "rfidmonitor.h"

RFIDMonitor::RFIDMonitor(QObject *parent) :
    QObject(parent)
{
    Logger::instance()->writeRecord(Logger::info, "Main", Q_FUNC_INFO, "System started");
    m_module = "Main";
}

void RFIDMonitor::start(const QCoreApplication &app)
{
    if(app.arguments().contains("--debug"))
        Logger::instance()->startDebugMode();

    if(app.arguments().contains("-d") &&
            app.arguments().size() > (app.arguments().indexOf("-d") + 1)){

        QString device = app.arguments().at(app.arguments().indexOf("-d") + 1);

        loadModules();
        Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, "Hello Debug");

        try {
            /*!
             * \brief mainServiceFunc loads the function that invokes the main service
             */
            std::function< void(const QString &) > mainServiceFunc = ServiceManager::instance()->get_function< void, const QString &>("main.call_services");
            mainServiceFunc(device);

        }catch(std::exception &ex){
            Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, "Main service not found");
            QTimer::singleShot(100, &app, SLOT(quit()));
        }

    }else{
        std::function< QString() > func = [&app]() -> QString
        {
                QString list;
                for(int i= 1; i < app.arguments().size(); i++){
            list += app.arguments().at(i);
        }
        return list;
    };
    Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, QString("Invalid arguments: %1").arg(func()));
    QTimer::singleShot(300, &app, SLOT(quit()));
}
}

const QList<CoreModule *> &RFIDMonitor::moduleList() const
{
    return m_moduleList;
}

void RFIDMonitor::loadModules()
{
    QDir pluginsDir(qApp->applicationDirPath());

    // To be used in production on Pi with "/home/pi/FishMonitoring " folder.
    pluginsDir.cd("modules");

    //To be used in development on Desktop with "/home/<user>/projectBuildPath" folder, that make possible direct run app by QtCreator.
    //pluginsDir.cd("../RFIDMonitor/modules");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)){
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));

        Logger::instance()->writeRecord(Logger::info, m_module, Q_FUNC_INFO, QString("Load module %1").arg(fileName));

        QObject *plugin = loader.instance();

        if(!loader.isLoaded()){
            Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("Error to load module %1").arg(fileName));
        }

        if (plugin){
            CoreModule *coreMod = qobject_cast< CoreModule * >(plugin);
            if(coreMod){
                m_moduleList.append(coreMod);
                coreMod->init();
            }
        }
    }
    Logger::instance()->writeRecord(Logger::info, m_module, Q_FUNC_INFO, "All Modules Loaded");
}
