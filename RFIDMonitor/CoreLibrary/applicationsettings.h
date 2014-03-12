/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Luis Valdes <luisvaldes88@gmail.com>
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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include <QMap>

class CoreModule;
class Service;
enum class ServiceType;

class ApplicationSettings : public QObject
{
    Q_OBJECT
public:

    static ApplicationSettings * instance();
    /*!
     * \brief installModule
     * \param module
     */
    void installModule(CoreModule *module);

    /*!
     * \brief loadDefaultServices
     */
    void loadDefaultServices();

    /*!
     * \brief servicesLoaded
     * \return
     */
    bool servicesLoaded();

    /*!
     * \brief m_defaultServices
     */
    const QMap<ServiceType, QString> & defaultServices() const;

private:
    explicit ApplicationSettings(QObject *parent = 0);

    bool m_servicesLoaded;
    QMap<ServiceType, QString> m_defaultServices;
};

#endif // APPLICATIONSETTINGS_H
