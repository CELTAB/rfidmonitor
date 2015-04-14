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

#include <QJsonDocument>
#include <QJsonArray>
#include <QProcess>
#include <QRegularExpression>

#include <core/interfaces.h>
#include <logger.h>
#include <json/nodejsmessage.h>
#include <json/synchronizationpacket.h>
#include <rfidmonitor.h>

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
//    m_tempFile.setFileName(QCoreApplication::applicationDirPath() + "/TempExport.fish");

    m_fileName = QCoreApplication::applicationDirPath() + "/TempExport.fish";
}

ExportLocalData::~ExportLocalData()
{
}

void ExportLocalData::startExport()
{
    // Timer to export data to temporary file
    m_exportTime = new QTimer(this);
    m_exportTime->setInterval(exportTime);
    // When the timeout signal is emitted the export action slot is called (exportAction when do not receive parameters uses a default value and then export data to temp file)
    QObject::connect(m_exportTime, SIGNAL(timeout()), this, SLOT(exportAction()));
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
            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Exporting temp file to %1").arg(device));

            // name of file to save on device (absolute path)
            QString destinationPath(device + "/export_" + QDateTime::currentDateTime().toString().replace(" ", "_").replace(":","") + ".fish");

            // If the temp file doesn't exist, has nothing to be exported
            if(QFile::exists(m_fileName)){
                if(QFile::copy(m_fileName, destinationPath)){
                    if(!QFile::remove(m_fileName)){
                        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("ERROR to remove temp file from disk"));
                        throw std::exception();
                    }
                } else {
                    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("ERROR to copy temp file to device"));
                    throw std::exception();
                }
            } else {
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("There's nothing to be exported"));
                returnValue = true;
            }
        }catch(std::exception &e){
            Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Erro: %1").arg(e.what()));
            returnValue = false;
        }
    } else {
        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("EXPORT ERROR: Can\'t export to external device"));
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
    if(path == "temp"){
        exportToTempFile();
    } else {
        if(exportToDevice(path)){
            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Exportation finished"));
        }
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
    // if a server is connected than has no need to export into a temporary file. Return false.
    if(!RFIDMonitor::instance()->isconnected()){
        static PackagerInterface *packager = 0;
        if(!packager) {
            packager = qobject_cast<PackagerInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPackager));
        }

        if(packager){

            QFile tempFile;
            tempFile.setFileName(m_fileName);

            // try to open a file to read the records to be exported. Return false if the file cannot be opened for some reason
            if (!tempFile.open(QIODevice::ReadOnly)){
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("File [%1] does not exist yet.").arg(m_fileName));
            }

            QMap<QString, QByteArray> allData = packager->getAll();
            QMap<QString, QByteArray>::iterator i;

            // if have no packets to be exported, turns off the red led (after 1 seconds) and than return.
            if(allData.size() <= 0){
                // Turn off the red LED after 1 second
                QTimer timer;
                timer.start(1000);
                while(timer.remainingTime() > 0)
                    ;
                // turn off red led
                m_blinkLed->blinkRedLed(0);
                return true;
            }
            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Exporting %1 Packets to %2").arg(allData.size()).arg(m_fileName));

            QByteArray saveData = tempFile.readAll();
            tempFile.close();

            QJsonArray loadDoc(QJsonDocument::fromJson(saveData).array());

            // turn on red led
            m_blinkLed->blinkRedLed(1);

            // try to open a file to write the records to be exported. Return false if the file cannot be opened for some reason
            if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Error to write into file %1").arg(m_fileName));
                return false;
            }

            if(allData.size() > 0){
                for(i = allData.begin(); i != allData.end(); ++i){
                    loadDoc.append(QJsonValue(QJsonDocument::fromJson(i.value()).object()));
                }
                tempFile.write(QJsonDocument(loadDoc).toJson());
                tempFile.flush();

                // close the file
                tempFile.close();

                // Turn off the red LED after 1 second
                QTimer timer;
                timer.start(1000);
                while(timer.remainingTime() > 0)
                    ;
                // turn off red led
                m_blinkLed->blinkRedLed(0);
            }
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::debug, "synchronizer", Q_FUNC_INFO, QString("Packager is not working!"));
            return false;
        }
    }else{
        return false;
    }
    return true;
}
