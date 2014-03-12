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

#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QDebug>

#include <functional>
#include <boost/any.hpp>


enum class ServiceType {
    KReadingService = 0x1,
    KPersistenceService,
    KExportService,
    KSynchronizeService,
    KCommunicationService,
    KPackagerService
};

//namespace ServiceType{
//    const QString KReadingService = "ReadingService";
//    const QString KPersistenceService = "PersistenceService";
//    const QString KExportService = "ReadingService";
//    const QString KSynchronizeService = "ReadingService";
//}

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = 0);

    virtual QString serviceName() const = 0;
    virtual void init() = 0;

    virtual ServiceType type() = 0;



    /*!
     * \brief register_function is in charge of receiving a function address and a name, and save the function in a map by using the name as the key.
     * \param key is the value that is used to get a function from the map.
     * \param function is the service from some module.
     */
    void registerFunction(const QString &key, const boost::any &function)
    {
        m_serviceMap.insert(key, function);
    }

    /*!
     * \brief get_function is responsible for returning a std::function by using the name of the service as a key.
     *
     * The function is a variadic template from C++11, it receives as a first template argument the return value of the stored function.
     * The second template parameter is variable, it can be 0 (zero)  or more parameters, the represents the arguments of the function.
     * In it's implementation, it takes the ReturnValue and Args to cast a prototype of the function from a boost::any object, throws boost::bad_any_cast.
     *
     * \param key is the name of the service that the function will try to return.
     * \return creates a std::function of the following signature std::function< ReturnValue(Args...) >
     */
    template<typename ReturnValue, typename... Args >
    std::function< ReturnValue(Args ...) > getFunction(const QString &key) throw(boost::bad_any_cast)
    {
      const boost::any &func = m_serviceMap.value(key);
      std::function< ReturnValue(Args ...) > function = boost::any_cast< std::function< ReturnValue(Args ...) > >(func);
      return function;
    }

private:
    QMap < QString, boost::any > m_serviceMap;
};

#endif // SERVICE_H
