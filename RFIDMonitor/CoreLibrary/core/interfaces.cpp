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

#include "interfaces.h"


ReadingInterface::ReadingInterface(QObject *parent) :
    Service(parent)
{
}

ExportInterface::ExportInterface(QObject *parent) :
    Service(parent)
{
}


PackagerInterface::PackagerInterface(QObject *parent) :
    Service(parent)
{
}

SynchronizationInterface::SynchronizationInterface(QObject *parent) :
    Service(parent)
{
}


CommunicationInterface::CommunicationInterface(QObject *parent) :
    Service(parent)
{

}


PersistenceInterface::PersistenceInterface(QObject *parent) :
    Service(parent)
{

}
