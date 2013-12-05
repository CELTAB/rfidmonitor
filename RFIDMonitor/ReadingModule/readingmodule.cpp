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

#include <servicemanager.h>

#include "readingmodule.h"
#include "datareader.h"

static DataReader *reader = 0;

void start_reading(const QString &device)
{
    if(!reader){
        reader = new DataReader();
    }
    reader->startReading(device);
}

ReadingModule::ReadingModule(QObject *parent) :
    CoreModule(parent)
{
}

ReadingModule::~ReadingModule()
{
    if(reader){
        reader->deleteLater();
    }
}

void ReadingModule::init()
{
    ServiceManager::instance()->register_function("reading.start_reading", std::function< void(const QString &) >(start_reading));
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ReadingModule, CoreModule)
#endif // QT_VERSION < 0x050000
