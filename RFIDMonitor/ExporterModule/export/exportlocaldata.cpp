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

#include <QProcess>
#include <QRegularExpression>

#include <logger.h>
#include "object/rfiddata.h"

#include "exportlocaldata.h"
#include "blinkled.h"

ExportLocalData::ExportLocalData(QObject *parent) :
    QObject(parent),
    m_blinkLed(0)
{
    m_module = "ExportModule";
    m_blinkLed = new BlinkLed(this);

    // Object to manipulate file
    m_tempFile.setFileName(QCoreApplication::applicationDirPath() + "/TempExport.fish");
}

void ExportLocalData::startExport()
{
    // Timer to export data to temporary file
    m_exportTime = new QTimer(this);
    m_exportTime->setInterval(exportTime);
    // When the timeout signal is emitted the export action slot is called (exportAction when do not receive parameters uses a default value and then export data to temp file)
    QObject::connect(m_exportTime, SIGNAL(timeout()), this, SLOT(exportAction()));//exportToTempFile()));
    m_exportTime->start();
}

// Export temporary file to an external device.
bool ExportLocalData::exportToDevice(QString device)
{
    // turns off leds red and green
    m_blinkLed->blinkGreenLed(0);
    m_blinkLed->blinkRedLed(0);

    // turn on red led
    m_blinkLed->blinkRedLed(1);

    bool returnValue = true;

    QTimer timer;
    timer.start(5000);

    // if device path is empty there's no device capable to recive the file
    if(!device.isEmpty()){
        try{
            // export data to external device
            Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("Exporting temp file to device"));

            // name of file to save on device (absolute path)
            QString destinationPath(device + "/export_" + QDateTime::currentDateTime().toString().replace(" ", "_").replace(":","") + ".fish");

            // If the temp file doesn't exist, has nothing to be exported
            if(m_tempFile.exists()){
                if(QFile::copy(m_tempFile.fileName(), destinationPath)){
                    if(!QFile::remove(m_tempFile.fileName())){
                        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("ERROR to remove temp file from disk"));
                        throw std::exception();
                    }
                } else {
                    Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("ERROR to copy temp file to device"));
                    throw std::exception();
                }
            } else {
                Logger::instance()->writeRecord(Logger::info, m_module, Q_FUNC_INFO, QString("There's nothing to be exported"));
                returnValue = true;
            }
        }catch(std::exception &e){
            Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(e.what()));
            returnValue = false;
        }
    } else {
        Logger::instance()->writeRecord(Logger::critical, m_module, Q_FUNC_INFO, QString("EXPORT ERROR: Can\'t export to external device"));
        returnValue = false;
    }

    // wait five(5) seconds before turn off the red led
    while(timer.remainingTime() > 0)
        ;
    // turns off the red led
    m_blinkLed->blinkRedLed(0);
    // turns on the green led
    m_blinkLed->blinkGreenLed(1);

    // return true only if the data was successfully exported
    return returnValue;
}

void ExportLocalData::exportAction(QString path)
{
    QMutexLocker locker(&m_mutex);

    if(path.compare("temp")){
        exportToDevice(path);
    } else {
        exportToTempFile();
    }
}

// SLOT
void ExportLocalData::turnOffLed()
{
    m_blinkLed->blinkGreenLed(0);
}

// search for data with non-sync status and export these datas into a temp file. If the file doesn't exist create a file
bool ExportLocalData::exportToTempFile()
{
    // functions pointer
    std::function< bool(const QList<Rfiddata *> &) > updateObject;
    std::function< QList<Rfiddata *> (const QString &, QVariant) > getDataToExport;

    Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("Exporting to temporary file"));
    Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("Search not synced data"));

    /*!
     * \brief list of data with non-synced status
     *
     * the getDataExport function will search in table rfiddata all data with non-synced value and return a list of datas
     */
    QList< Rfiddata * > list = getDataToExport("sync", Rfiddata::KNotSynced);

    // Verify how many records has to export. If none return true.
    if(list.length() > 0)
    {
        // try to open a file to append the records to be exported. Return false if the file cannot be opened for some reason
        if (!m_tempFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Error to open %1").arg(m_tempFile.fileName()));
            return false;
        }

        Logger::instance()->writeRecord(Logger::info, m_module, Q_FUNC_INFO, QString("Exporting %1 records to %2").arg(list.length()).arg(m_tempFile.fileName()));

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

        // turn on red led
        m_blinkLed->blinkRedLed(1);

        // Creat a stream to write data into file
        QTextStream out(&m_tempFile);

        foreach (Rfiddata *var, list) {
            out << QString("%1,%2,%3,%4, \"%5\"\n")
                   .arg(var->idpontocoleta().toString())
                   .arg(var->idantena().toString())
                   .arg(var->applicationcode().toString())
                   .arg(var->identificationcode().toString())
                   .arg(var->datetime().toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            // write each data from list in file
            out.flush();
            var->setSync(Rfiddata::KSynced); // change status to synced
        }
        Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("%1 records exported").arg(list.length()));

        // close the file
        m_tempFile.close();

        try{
            // update database
            Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, QString("Update data base to synced status"));
            updateObject(list);
        }catch(std::exception &e){
            Logger::instance()->writeRecord(Logger::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(e.what()));
            return false;
        }

        // Turn off the red LED after 1 second
        QTimer timer;
        timer.start(1000);
        while(timer.remainingTime() > 0)
            ;
        // turn off red led
        m_blinkLed->blinkRedLed(0);

    } else {
        Logger::instance()->writeRecord(Logger::debug, m_module, Q_FUNC_INFO, "No records to be exported");
    }

    // If all made successfully return true.
    return true;
}
