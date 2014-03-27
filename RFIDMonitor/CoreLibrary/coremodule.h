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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>
#include <QMap>

class Service;

/*!
 * \brief The CoreModule class is the interface that every module of the system has to implement to make it available to the QPlubinLoader.
 */
class CoreModule : public QObject
{
    Q_OBJECT
public:
    explicit CoreModule(QObject *parent = 0);

    /*!
     * \brief init is responsible fo any initialization of the module, eg. create data and register services.
     */
    virtual void init() = 0;

    /*!
     * \brief name
     * \return
     */
    virtual QString name() = 0;

    /*!
     * \brief version
     * \return
     */
    virtual quint32 version() = 0;

    /*!
     * \brief services
     * \return
     */
    QList<Service *> services();

    /*!
     * \brief service
     * \param name
     * \return
     */
    Service * service(QString name);

    /*!
     * \brief addService
     * \param serv
     */
    void addService(QString name, Service *serv);

private:
    QMap<QString, Service *> m_services;
};

#endif // PLUGININTERFACE_H
