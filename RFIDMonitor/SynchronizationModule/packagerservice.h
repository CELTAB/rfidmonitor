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

#ifndef PACKAGERSERVICE_H
#define PACKAGERSERVICE_H

#include <core/interfaces.h>

class PackagerService : public PackagerInterface
{
    Q_OBJECT
public:
    explicit PackagerService(QObject *parent = 0);

    QString serviceName() const;
    void init();
    ServiceType type();

    QMap<QString, QByteArray> getAll();

};

#endif // PACKAGERSERVICE_H
