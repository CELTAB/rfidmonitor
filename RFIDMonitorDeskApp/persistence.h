/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the RFIDMonitor project
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

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QSqlDatabase>

class Persistence : public QObject
{
    Q_OBJECT
public:
    static Persistence *instance();
    bool persistConfigurationSent(const QString &mac, const QString &ip, const QString &collectPointName, const QByteArray &json);
private:
    explicit Persistence(QObject *parent = 0);
    QSqlDatabase *systemConnection();
    QSqlDatabase m_systemConnection;

signals:

public slots:

};

#endif // PERSISTENCE_H
