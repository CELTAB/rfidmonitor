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

#include <QSerialPortInfo>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QTime>
#include <QJsonObject>

#include <iostream>
#include <ctime>

#include "reader_mri2000.h"

#include <logger.h>
#include <rfidmonitor.h>
#include <object/rfiddata.h>
#include <systemevents.h>

#include <json/nodejsmessage.h>
#include <json/synchronizationpacket.h>

QFile fileCaptured(QCoreApplication::applicationDirPath() + "/mri2000_captured.txt");

Reader_MRI2000::Reader_MRI2000(QObject *parent) :
    ReadingInterface(parent),
    m_serial(0)
{
    m_module = "ReadingModule_MRI2000";
    m_serial = new QSerialPort(this);
    m_lastGeneralState = SystemEvents::KGeneralUnknowState;

    connect(m_serial, SIGNAL(readyRead()), SLOT(readData()));
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));

    connect(SystemEvents::instance(), SIGNAL(General(SystemEvents::GeneralEvent)), this, SLOT(SysGeneralEvent(SystemEvents::GeneralEvent)));
    connect(SystemEvents::instance(), SIGNAL(Exporting(SystemEvents::ExportEvent)), this, SLOT(SysExportEvent(SystemEvents::ExportEvent)));

    allLines = false;
    idCollector = 0;

    if (fileCaptured.open(QFile::WriteOnly)){
        m_outCaptured.setDevice(&fileCaptured);
        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("mri2000_captured opened"));
    }else{
        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("mri2000_captured not open"));
    }

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("%1 Started").arg(m_module));
}

Reader_MRI2000::~Reader_MRI2000()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

QString Reader_MRI2000::serviceName() const
{
    return "MRI2000Reading.service";
}

void Reader_MRI2000::init()
{

}

ServiceType Reader_MRI2000::type()
{
    return ServiceType::KReader;
}

void Reader_MRI2000::fullRead(bool fr)
{
    allLines = fr;
}

void Reader_MRI2000::write(QString command)
{
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Try to write command %1 into device").arg(command));

    try {
        if(m_serial->isOpen()){
            //        convert QString to number
            bool parseOK;
            int parsedValue = command.toInt(&parseOK, 16);
            if(parseOK){
                if( (m_serial->write(reinterpret_cast<char*>(&parsedValue), sizeof(int))) == -1 ){
                    Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Error while writing"));
                }
            } else {
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Could not write command %1 into device").arg(command));
            }
        }else{
            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Serial device is not open"));
        }
    } catch (...) {
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Unknown error while trying to write."));
    }


}

void Reader_MRI2000::readData()
{
    /* MRID2000 Multireader pattern

       TAG2W 001 0000000002A474C9
       TAG2W 002 0000000002A474B3
       TAG2W 001 0000000002A474C9
     */
    if(m_serial->canReadLine()){
        if(!allLines){

            QString hardData(m_serial->readLine());
            hardData.remove(QRegExp("[\\n\\t\\r]"));

            QRegularExpression regexCode;
            regexCode.setPattern("TAG[0-9a-fA-F]W\\s[0-9a-fA-F]{3}\\s[0-9a-fA-F]{16}");
            QRegularExpressionMatch match = regexCode.match(hardData);          

            if(match.hasMatch()) {

                //Here the matched string must be like "TAG5W 001 0000000295901506"
                Rfiddata *data = new Rfiddata(this);

                if(m_outCaptured.device()){
                    m_outCaptured << match.captured(0) << "\n";
                    m_outCaptured.flush();
                }

                // Id collector from configuration file
//                data->setIdpontocoleta(idCollector);

                //The character 3 from string is the number of antenna: TAG[5]W...
//                data->setIdantena(match.captured(0).at(3).digitValue());
                QJsonObject extraData;
                extraData.insert("idAntena", QJsonValue(match.captured(0).at(3).digitValue()));
                data->setExtraData(extraData);

                //Try to convert the code of 16 characters, from hexa to decimal.
                bool hexaConvertion;
                QString hexaCode(match.captured(0).right(16));
                QString deciCode(QString::number(hexaCode.toLongLong(&hexaConvertion, 16)));

                if(!hexaConvertion){
                    //Problem converting from hexadecimal.
                    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Could not convert RFID code from hexadecimal. Hexa code: %1").arg(hexaCode));
                    return;
                }

//                qlonglong applicationcode = deciCode.left(4).toLongLong();
//                qlonglong applicationcode = 0;
//                qlonglong identificationcode = deciCode.remove(deciCode.left(4)).toLongLong();
                qlonglong identificationcode = deciCode.toLongLong();

                /* -- NOT USED BECAUSE THE MRI2000 MULTIREADER ALREADY APPLIES A FILTER.
                    * Filter by time. If more than one transponder was read in a time interval only one of them will be persisted.
                    * A QMap is used to verify if each new data that had arrived was already read in this interval.
                    */
                //                if(!m_map.contains(identificationcode)){

                //                    QTimer *timer = new QTimer;
                //                    timer->setSingleShot(true);
                //                    timer->setInterval(1000);
                //                    connect(timer, &QTimer::timeout,
                //                            [=, this]()
                //                    {
                //                        this->m_map.remove(identificationcode);
                //                        timer->deleteLater();
                //                    });
                //                    m_map.insert(identificationcode, timer);
                //                    timer->start();

                //From the full code, the leftmost 4 are the application core.
//                data->setApplicationcode(applicationcode);
                //From the full code, removing the application code, there is the identification code
//                data->setIdentificationcode(identificationcode);
                data->setRfidcode(identificationcode);
                //Take the current date and set on the object
                data->setDatetime(QDateTime::currentDateTime());
                //Set the object as NotSynced
                data->setSync(Rfiddata::KNotSynced);

                QList<Rfiddata*> list;
                list.append(data);

                try {
                    PersistenceInterface *persister = qobject_cast<PersistenceInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KPersister));
                    Q_ASSERT(persister);

                    SynchronizationInterface *synchronizer = qobject_cast<SynchronizationInterface*>(RFIDMonitor::instance()->defaultService(ServiceType::KSynchronizer));
                    Q_ASSERT(synchronizer);

#ifdef CPP_11_ASYNC
                    /*C++11 std::async Version*/
                    std::function<void(const QList<Rfiddata *>&)> persistence = std::bind(&PersistenceInterface::insertObjectList, persister, std::placeholders::_1);
                    std::async(std::launch::async, persistence, list);
                    std::function<void()> synchronize = std::bind(&SynchronizationInterface::readyRead, synchronizer);
                    std::async(std::launch::async, synchronize);
#else
                    /*Qt Concurrent Version*/
                    QtConcurrent::run(persister, &PersistenceInterface::insertObjectList, list);
                    QtConcurrent::run(synchronizer, &SynchronizationInterface::readyRead);
#endif
                } catch (std::exception &e) {
                    emit SystemEvents::instance()->General(SystemEvents::KLosingData);
                    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("QtConcurrent ERROR"));
                }

//                                } // END OF if(!m_map.contains(identificationcode)){

            } else {
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Temperature: %1").arg(hardData));
            }
        }else{

//            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("FULL READ..."));

            QByteArray buffer = m_serial->readLine();
            QString data(buffer);
            data.remove(QRegExp("[\\n\\t\\r]"));

            // Remove lines only with LI. Not used anywhere.
            if(data == "LI")
                return;

            json::NodeJSMessage answer;
            answer.setType("READER-RESPONSE");

            QJsonObject dataObj;
            dataObj.insert("sender", QString("app"));
            dataObj.insert("response", data);
            dataObj.insert("reader", QString(""));

            answer.setDateTime(QDateTime::currentDateTime());
            answer.setJsonData(dataObj);
            QJsonObject jsonAnswer;
            answer.write(jsonAnswer);

            try {
                static CommunicationInterface *communitacion = 0;

                communitacion = qobject_cast<CommunicationInterface *>(RFIDMonitor::instance()->defaultService(ServiceType::KCommunicator));
#ifdef CPP_11_ASYNC
                /*C++11 std::async Version*/
                std::function<void (QByteArray)> sendMessage = std::bind(&CommunicationInterface::sendMessage, communitacion, std::placeholders::_1);
                std::async(std::launch::async, sendMessage, QJsonDocument(jsonAnswer).toJson());
#else
                /*Qt Concurrent Version*/
                QtConcurrent::run(communitacion, &CommunicationInterface::sendMessage, QJsonDocument(jsonAnswer).toJson());
#endif
            } catch (std::exception &e) {
                emit SystemEvents::instance()->General(SystemEvents::KLosingData);
                Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("QtConcurrent ERROR"));
            }
        }
    }
}

void Reader_MRI2000::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Error: %1").arg(m_serial->errorString()));
    }
}

void Reader_MRI2000::start()
{

    QString device = RFIDMonitor::instance()->device();
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 Using device: %1").arg(device));

    idCollector = RFIDMonitor::instance()->idCollector();

    QSerialPortInfo info(device);
    m_serial->setPort(info);
    if(!m_serial->open(QIODevice::ReadWrite)) {

        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Could not open device %1 - Error %2").arg(device).arg(m_serial->errorString()));
        emit SystemEvents::instance()->General(SystemEvents::KLosingData);
        // create class invalid_device exception on core Module
        QTimer::singleShot(30000, this, SLOT(start()));
    }else{
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setParity(QSerialPort::NoParity);

        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("HERE>>>>>>>>>> Emitting RunningSmooth Signal"));
        emit SystemEvents::instance()->General(SystemEvents::KRunningSmooth);

        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("HERE>>>>>>>>>> Signal RunningSmooth Emitted"));
    }
}

void Reader_MRI2000::stop()
{
    emit SystemEvents::instance()->General(SystemEvents::KLosingData);
    disconnect(m_serial, SIGNAL(readyRead()),this, SLOT(readData()));
    disconnect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}

/*
 *
 * POSSIBLE SYSTEM EVENTS FROM CORELIBRARY - SYSTEMEVENTS.H:
 * LosingData : (Error) not possible to persist and read rfid (MRI2000 DEFAULT STATE).
 * SoftProblem : (Error)any problem that not lose data.
 * RunningSmooth : Everything is ok.
 * ExportingNow: system started to export to local device
 * ExportingDone: system finished local export to localdevice.
 *
 * LED States with BLINK:
 * Red On | Green Off = LosingData;
 * Red Blink | Green On = SoftProblem;
 * Red Off | Green On = RunningSmooth OR ExportingDone;
 * Red Off | Green Blink = ExportingNow;
 *
 * LED States without BLINK:
 * G on R on - ExportingNow
 * G on R off - RunningSmooth | ExportingDone
 * G off R on - SoftProblem
 * G off R off - LosingData (MRI2000 DEFAULT STATE);
 *
 * RELAY MAP:
 * green relay 1
 * red relay 2
 *
 * RELAY COMMANDS:
 * activate relay 1 0x1D
 * deactivate relay 1 0x0D
 * activate relay 2 0x1E
 * deactivate relay 2 0x0E
*/

void Reader_MRI2000::writeLosingData()
{
    //G off R off - LosingData (default)

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeLosingData"));

    //green off
    write("0x0D");

    //red off
    write("0x0E");

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeLosingData DONE"));
}

void Reader_MRI2000::writeRunningSmooth()
{
    //G on R off - RunningSmooth | ExportingDone

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeRunningSmooth"));

    //green on
    write("0x1D");

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeRunningSmooth DELAY"));
    delay(1);

    //red off
    write("0x0E");
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeRunningSmooth DONE"));
}

void Reader_MRI2000::writeSoftProblem()
{
    //G off R on - SoftProblem

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeSoftProblem"));

    //green off
    write("0x0D");

    //red on
    write("0x1E");
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeSoftProblem DONE"));
}

void Reader_MRI2000::writeExportingNow()
{
    //G on R on - ExportingNow

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeExportingNow"));

    //green on
    write("0x1D");

    delay(1);
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeExportingNow DELAY"));

    //red on
    write("0x1E");
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("MRI2000 writeExportingNow DEBUG"));
}

void Reader_MRI2000::SysGeneralEvent(SystemEvents::GeneralEvent event)
{ 
    m_lastGeneralState = event;

    switch(event){
    case SystemEvents::KLosingData:
        writeLosingData();
        break;
    case SystemEvents::KSoftProblem:
        writeSoftProblem();
        break;
    case SystemEvents::KRunningSmooth:
        writeRunningSmooth();
        break;
    default:
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Undefined or KExportUnknowState state."));
    }
}

void Reader_MRI2000::SysExportEvent(SystemEvents::ExportEvent event)
{
    switch(event){
    case SystemEvents::KExportingNow:
        writeExportingNow();
        //wait 5 seconds
        delay(5);
        break;
    case SystemEvents::KExportingDone:
        writeRunningSmooth();
        //wait 2 sec
        delay(2);
        //print last state;
        SysGeneralEvent(m_lastGeneralState);
        break;
    default:
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Undefined or KExportUnknowState state."));
    }
}

void Reader_MRI2000::delay(int seconds)
{
    QTime dieTime= QTime::currentTime().addSecs(seconds);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
