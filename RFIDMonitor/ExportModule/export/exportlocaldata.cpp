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
#include <QDebug>
#include <QList>
#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QMutexLocker>
#include <QDir>
#include <QTimer>
#include <functional>

#include <servicemanager.h>
#include <logger.h>
#include "object/rfiddata.h"

#include "exportlocaldata.h"
#include "blinkled.h"

ExportLocalData::ExportLocalData(QObject *parent) :
    QObject(parent),
    m_blickLed(0)
{
    m_module = "ExportModule";
    m_blickLed = new BlinkLed(this);
}

bool ExportLocalData::exportToDevice(QString device)
{
    // functions pointer
    std::function< bool(const QList<Rfiddata *> &) > updateObject;
    std::function< QList<Rfiddata *> (const QString &, QVariant) > getDataToExport;

    try{
        // reference to persistence service functions
        getDataToExport = ServiceManager::instance()->get_function< QList<Rfiddata *>, const QString &, QVariant >("persistence.select_data");
        updateObject = ServiceManager::instance()->get_function< bool, const QList<Rfiddata *> & >("persistence.update_object_list");
    }catch(boost::bad_any_cast &ex){
        Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(ex.what()));
        return false;
    }catch(std::exception &e){
       Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(e.what()));
        return false;
    }

    /*!
     * \brief list of data with non-synced status
     *
     * the getDataExport function will search in table rfiddata all data with non-synced value and return a list of datas
     */
    QList< Rfiddata * > list = getDataToExport("sync", Rfiddata::KNotSynced);

    // RAII para eliminar automaticamente os registros do "list" alocados no heap cuando a funcao sai do scope
    // E criado um shared pointer que guarda o endereco de um std::function< void() > f1 que e chamado para eliminar os registros da list
    // A funcao std::function< void ( std::function<void()> * ) > f2 e o destructor da f1, a mesma executa a funcao f1 e depois chama o destructor da mesma ja que esta esta alocada no heap.
    /*!
     * \brief raii remove the data allocated on heap after leave the scope
     *
     * The shared pointer keeps the address of std::function< void() > f1 function that will be called to delete the list of data
     * std::function< void ( std::function<void()> * ) > f2 function is f1 destructor function and is called once is that allocated on heap
     */
    QSharedPointer< std::function<void()> > raii;
    raii = QSharedPointer< std::function< void() > >( new std::function< void() >( [&list] () -> void { qDeleteAll(list); } ),
                                                      std::function< void ( std::function<void()> * ) > ([] ( std::function<void()> *ptr) { (*ptr)(); delete ptr; }));

    // turns off leds red and green
    m_blickLed->blinkGreenLed(0);
    m_blickLed->blinkRedLed(0);

    QTimer timer;
    timer.start(5000);
    // turn on red led
    m_blickLed->blinkRedLed(1);

    bool returnValue = true;
    try{
        // export data
        exportCSVData(device, list);
        exportLegacyCSVData(device, list);
        // update database
        updateObject(list);
    }catch(std::exception &e){
        Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(e.what()));
//        qDebug() << e.what();
        returnValue = false;
    }
    // wait five(5) seconds before turn off the red led
    while(timer.remainingTime() > 0)
        ;
    // turns off the red led
    m_blickLed->blinkRedLed(0);
    // turns on the green led
    m_blickLed->blinkGreenLed(1);

    // return true only if the data was successfully exported
    return returnValue;
}

bool ExportLocalData::exportAllNonSyncedRegisters()
{
    // lock this section of code so that only one thread can access it at a time.
    QMutexLocker locker(&m_mutex);

    // wait five(5) seconds before write data into device.
    QTimer timer;
    timer.start(5000);
    while(timer.remainingTime() > 0){

    }

    /*
     * The directory path where the device is mounted is different on each platform.
     * So, it is verified if the directory "/media/'USER'/" exists. in negative case, is used the "/media/" directory, without 'USER'.
     */
    // Desktop Ubuntu
    QString dirDesk(QString("/media/") + getenv("USER"));
    // Raspberry Pi
    QString dirPi("/media");

    QDir directory(dirDesk);
    if(!directory.exists()){
        directory.setPath(dirPi);
    }

    // after create the directory path for current platform, verify if exists
    if(directory.exists()){
        //for each subdirectory, verify if is valid and call the exportToDevice function
        foreach (QFileInfo path, directory.entryList()){
            if(!path.baseName().isEmpty()){
                QString device(QString(directory.path() + "/" + path.fileName()));
                Logger::instance()->writeRecord(Logger::info, m_module, Q_FUNC_INFO, QString("Using device: %1").arg(device));
                // call the exportToDevice function with a valid path of device to write data
                if(exportToDevice(device)){
                    system("sync");
                    return true;
                }
            }
        }
        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("EXPORT ERROR: No media found to export on: %1").arg(directory.absolutePath()));
    }else{
        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("EXPORT ERROR: Mount point not found"));
    }
    return false;
}

void ExportLocalData::exportCSVData(const QString &device, const QList<Rfiddata *> &list) throw(std::exception)
{
    // name of file to save on device (absolute path)
    QString path(device + "/export_" + QDateTime::currentDateTime().toString().replace(" ", "_").replace(":","") + ".fish");

    QFile exportFile(path);
    if(exportFile.open(QFile::ReadWrite)) { // open file with read and write permission
        QTextStream stream;
        stream.setDevice(&exportFile);
        stream << QString("idpontocoleta,idantena,applicationcode,identificationcode,datetime\n");
        stream.flush();
        // idpontocoleta,idantena,applicationcode,identificationcode,datetime
        foreach (Rfiddata *var, list) {
            stream << QString("%1,%2,%3,%4, \"%5\"\n")
                      .arg(var->idpontocoleta().toString())
                      .arg(var->idantena().toString())
                      .arg(var->applicationcode().toString())
                      .arg(var->identificationcode().toString())
                      .arg(var->datetime().toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            // write each data from list in file
            stream.flush();
            var->setSync(Rfiddata::KSynced); // change status to synced
        }
        exportFile.close(); // close file
    }else{
        throw std::exception();
    }
}

void ExportLocalData::exportLegacyCSVData(const QString &device, const QList<Rfiddata *> &list) throw(std::exception)
{
    // name of file to save on device (absolute path)
    QString path(device + "/export_legacy_" + QDateTime::currentDateTime().toString().replace(" ", "_").replace(":","") + ".fish");
    QFile exportFile(path);
    if(exportFile.open(QFile::ReadWrite)) {
        QTextStream stream;
        stream.setDevice(&exportFile);

        foreach (Rfiddata *var, list) {
            // idpontocoleta,idantena,applicationcode,identificationcode,datetime
            stream << QString("%1,\"%2\",\"%3\"\n")
                      .arg(var->idantena().toString())
                      .arg(var->identificationcode().toString())
                      .arg(var->datetime().toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            stream.flush();
            var->setSync(Rfiddata::KSynced);
        }
        exportFile.close();
    }else{
        throw std::exception();
    }
}

void ExportLocalData::turnOffLed()
{
    m_blickLed->blinkGreenLed(0);
}

ExportLocalData *ExportLocalData::instance()
{
    // if already exist a instance of this class, returns. otherwise get a new instance
    static ExportLocalData *singleton = 0;
    if(!singleton){
        singleton = new ExportLocalData(qApp);
    }
    return singleton;
}
