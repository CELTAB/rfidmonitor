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
#include <future>
#include <functional>

#include "reader_rfm008b.h"

#include <logger.h>
#include <object/rfiddata.h>
#include <rfidmonitor.h>


#include <json/nodejsmessage.h>
#include <json/synchronizationpacket.h>


//QFile file("rfidmonitor.txt");
QFile fileCaptured("rfidmonitor_captured.txt");

Reader_RFM008B::Reader_RFM008B(QObject *parent) :
    ReadingInterface(parent),
    m_serial(0)
{
    m_module = "ReadingModule";
    m_serial = new QSerialPort(this);

    allLines = false;

    //    if (file.open(QFile::WriteOnly)){
    //        m_outReceived.setDevice(&file);
    //    }
    if (fileCaptured.open(QFile::WriteOnly)){
        m_outCaptured.setDevice(&fileCaptured);
    }
}

Reader_RFM008B::~Reader_RFM008B()
{
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

QString Reader_RFM008B::serviceName() const
{
    return "reading.service";
}

void Reader_RFM008B::init()
{

}

ServiceType Reader_RFM008B::type()
{
    return ServiceType::KReader;
}

void Reader_RFM008B::fullRead(bool fr)
{
    allLines = fr;
}

void Reader_RFM008B::write(QString command)
{
    if(m_serial->isOpen()){
        m_serial->write(command.toLocal8Bit());
    }
}

void Reader_RFM008B::readData()
{
    if(m_serial->canReadLine()){
        if(!allLines){

            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("Reading Data..."));

            QByteArray buffer = m_serial->readAll();
            QRegExp regex;
            regex.setPattern("(L(\\d{2})?W)\\s([0-9a-fA-F]{4})\\s([0-9a-fA-F]{16})");

            QString data(buffer);
            data.remove(QRegExp("[\\n\\t\\r]"));

            //        if(m_outReceived.device()){
            //            m_outReceived << data;
            //            m_outReceived.flush();
            //        }


            int pos = regex.indexIn(data);
            if(pos != -1){
                // The first string in the list is the entire matched string.
                // Each subsequent list element contains a string that matched a
                // (capturing) subexpression of the regexp.
                QStringList matches = regex.capturedTexts();

                // crear RFIDData
                QRegularExpression regexCode;
                regexCode.setPattern("([0-9a-fA-F]{4})(\\s)([0-9a-fA-F]{16})");
                QRegularExpressionMatch match = regexCode.match(matches.at(0));

                if(m_outCaptured.device()){
                    m_outCaptured << matches.at(0);
                    m_outCaptured.flush();
                }

                if(match.hasMatch()) {
                    Rfiddata *data = new Rfiddata(this);

                    int idPontoColeta = 1;
                    data->setIdpontocoleta(idPontoColeta);

                    int idAntena = 1;
                    data->setIdantena(idAntena);

                    qlonglong applicationcode = match.captured(1).toLongLong();
                    qlonglong identificationcode = match.captured(3).toLongLong();

                    /*
                 * Filter by time. If more than one transponder was read in a time interval only one of them will be persisted.
                 * A QMap is used to verify if each new data that had arrived was already read in this interval.
                 */
                    if(!m_map.contains(identificationcode)){

                        QTimer *timer = new QTimer;
                        timer->setSingleShot(true);
                        timer->setInterval(1000);
                        connect(timer, &QTimer::timeout,
                                [=, this]()
                        {
                            this->m_map.remove(identificationcode);
                            timer->deleteLater();
                        });
                        m_map.insert(identificationcode, timer);
                        timer->start();

                        data->setApplicationcode(applicationcode);
                        data->setIdentificationcode(identificationcode);
                        data->setDatetime(QDateTime::currentDateTime());
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
                }
            }
        }else{

            Logger::instance()->writeRecord(Logger::severity_level::debug, m_module, Q_FUNC_INFO, QString("FULL READ..."));

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
            dataObj.insert("reader", QString("A1"));

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
    }
}

void Reader_RFM008B::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        Logger::instance()->writeRecord(Logger::severity_level::error, m_module, Q_FUNC_INFO, QString("Error: %1").arg(m_serial->errorString()));
    }
}

void Reader_RFM008B::start()
{
    QString device = RFIDMonitor::instance()->device();
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

void Reader_RFM008B::stop()
{
    disconnect(m_serial, SIGNAL(readyRead()),this, SLOT(readData()));
    disconnect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}
