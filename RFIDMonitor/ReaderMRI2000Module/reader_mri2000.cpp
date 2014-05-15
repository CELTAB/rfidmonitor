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

#include <iostream>
#include <ctime>

#include "reader_mri2000.h"

#include <logger.h>
#include <rfidmonitor.h>
#include <object/rfiddata.h>

#include <json/nodejsmessage.h>
#include <json/synchronizationpacket.h>

Reader_MRI2000::Reader_MRI2000(QObject *parent) :
    ReadingInterface(parent),
    m_serial(0)
{
    m_module = "ReadingModule_MRI2000";
    m_serial = new QSerialPort(this);

    allLines = false;
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

    if(m_serial->isOpen()){
        //        convert QString to number
        bool parseOK;
        int parsedValue = command.toInt(&parseOK, 16);
        if(parseOK){
            m_serial->write(reinterpret_cast<char*>(&parsedValue), sizeof(int));
        } else {
            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Could not write command %1 into device").arg(command));
        }
    }
}

void Reader_MRI2000::start()
{
    QString device = RFIDMonitor::instance()->device();

    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Using device: %1").arg(device));

    QSerialPortInfo info(device);
    m_serial->setPort(info);
    if(!m_serial->open(QIODevice::ReadWrite)) {

        Logger::instance()->writeRecord(Logger::severity_level::fatal, m_module, Q_FUNC_INFO, QString("Could not open device %1 - Error %2").arg(device).arg(m_serial->errorString()));
        // create class invalid_device exception on core Module
        QTimer::singleShot(1000, this, SLOT(start()));
    }else{
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setParity(QSerialPort::NoParity);
        connect(m_serial, SIGNAL(readyRead()), SLOT(readData()));
        connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
    }
}

void Reader_MRI2000::stop()
{
    disconnect(m_serial, SIGNAL(readyRead()),this, SLOT(readData()));
    disconnect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}

void Reader_MRI2000::readData()
{
    Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Data Available..."));

    if(m_serial->bytesAvailable() > 0){
        if(!allLines){

            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Reading Data..."));

            QByteArray buffer = m_serial->readAll();

            QString hardData(buffer);
            hardData.remove(QRegExp("[\\n\\t\\r]"));

            QRegularExpression regexCode;
            regexCode.setPattern("TAG[0-9a-fA-F]W\\s[0-9a-fA-F]{3}\\s[0-9a-fA-F]{16}");
            QRegularExpressionMatch match = regexCode.match(hardData);

            if(match.hasMatch()) {

                //Here the matched string must be like "TAG5W 001 0000000295901506"
                Rfiddata *data = new Rfiddata(this);

                //Temporary define a static id to the collect point.
                int idPontoColeta = 1;
                data->setIdpontocoleta(idPontoColeta);

                //The character 3 from string is the number of antenna: TAG[5]W...
                data->setIdantena(match.captured(0).at(3).digitValue());

                //Try to convert the code of 16 characters, from hexa to decimal.
                bool hexaConvertion;
                QString hexaCode(match.captured(0).right(16));
                QString deciCode(QString::number(hexaCode.toLongLong(&hexaConvertion, 16)));

                if(!hexaConvertion){
                    //Problem converting from hexadecimal.
                    Logger::instance()->writeRecord(Logger::severity_level::fatal, m_module, Q_FUNC_INFO, QString("Could not convert RFID code from hexadecimal. Hexa code: "+hexaCode));
                    return;
                }

                //From the full code, the leftmost 4 are the application core.
                data->setApplicationcode(deciCode.left(4));
                //From the full code, removing the application code, there is the identification code
                data->setIdentificationcode(deciCode.remove(deciCode.left(4)));
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
                    Logger::instance()->writeRecord(Logger::severity_level::fatal, m_module, Q_FUNC_INFO, e.what());
                }
            }
        }else{

            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("FULL READ..."));

            QByteArray buffer = m_serial->readLine();
            QString data(buffer);

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
        }
    } else {
        Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("No bytes Available..."));
    }
}

void Reader_MRI2000::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError)
    {
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Error: %1").arg(m_serial->errorString()));
    }
}
