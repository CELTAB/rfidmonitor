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

#include <QTimer>

#include <servicemanager.h>
#include <logger.h>

#include "mainservice.h"

const QString MainService::KDeviceParam = "device";

MainService::MainService(QObject *parent) :
    QObject(parent)
{
    m_module = "MainService";
}

void MainService::callMainServices(const QMap<QString, QString> &services)
{
    try {
        // reading.start_reading
        std::function< void(const QString &)> readingService = ServiceManager::instance()->get_function< void, const QString &>("reading.start_reading");
        m_device = services.value(MainService::KDeviceParam);
        readingService(m_device);
    }catch(...) {
        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, "start reading service not found");
        QTimer::singleShot(6000, this, SLOT(retryStartReading()));
    }

    try {
        // export.start_daemon
        std::function< void()> exportDaemonService = ServiceManager::instance()->get_function< void >("export.start_daemon");
        exportDaemonService();
    }catch(...) {
        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, "export service not found");
    }
}

void MainService::retryStartReading()
{
    try {
        // reading.start_reading
        std::function< void(const QString &)> readingService = ServiceManager::instance()->get_function< void, const QString &>("reading.start_reading");
        readingService(m_device);
    }catch(...){
        Logger::instance()->writeRecord(Logger::fatal, m_module, Q_FUNC_INFO, "start reading service not found");
        QTimer::singleShot(6000, this, SLOT(retryStartReading()));
    }
}
