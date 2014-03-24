#include <QSerialPortInfo>
#include <QCoreApplication>
#include <QTextStream>
#include <QTime>

#include "serialcommunication.h"

SerialCommunication *SerialCommunication::instance()
{
    static SerialCommunication *singleton = 0;
    if(!singleton)
        singleton = new SerialCommunication(qApp);
    return singleton;
}

SerialCommunication::SerialCommunication(QObject *parent) :
    QObject(parent)
{
    m_serialPort = new QSerialPort(this);
    m_logFile = new QFile(this);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::dataRead);
}

QStringList SerialCommunication::availablePorts()
{
    QStringList deviceList;
    foreach (QSerialPortInfo spi, QSerialPortInfo::availablePorts()) {
        deviceList.append(spi.portName());
    }
    return deviceList;
}


SerialCommunication::~SerialCommunication()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
    }

    if(m_logFile->isOpen()){
        m_logFile->close();
    }
}
qint32 SerialCommunication::baudRate() const
{
    return m_baudRate;
}

void SerialCommunication::setBaudRate(const qint32 &baudRate)
{
    m_baudRate = baudRate;
}
qint32 SerialCommunication::dataBits() const
{
    return m_dataBits;
}

void SerialCommunication::setDataBits(const qint32 &dataBits)
{
    m_dataBits = dataBits;
}
qint32 SerialCommunication::stopBits() const
{
    return m_stopBits;
}

void SerialCommunication::setStopBits(const qint32 &stopBits)
{
    m_stopBits = stopBits;
}
qint32 SerialCommunication::partity() const
{
    return m_partity;
}

void SerialCommunication::setPartity(const qint32 &partity)
{
    m_partity = partity;
}
qint32 SerialCommunication::openMode() const
{
    return m_openMode;
}

void SerialCommunication::setOpenMode(const qint32 &openMode)
{
    m_openMode = openMode;
}
QString SerialCommunication::logFile() const
{
    return m_logFile->fileName();
}

void SerialCommunication::setLogFile(const QString &logFile, const qint32 &openMode)
{
    if(m_logFile->isOpen())
        m_logFile->close();

    if( ! logFile.isEmpty()){
        m_logFile->setFileName(logFile);
        if(m_logFile->open((QIODevice::OpenModeFlag)openMode) && m_logFile->isWritable()){
            writeLog(tr("Log file ok."));
        }else{
            if(m_logFile->isOpen())
                m_logFile->close();

            writeLog(tr("Failed to use the log file."));
        }
    }
}

void SerialCommunication::writeLog(QString text)
{
        QString time("[" + QTime::currentTime().toString() + "] ");

        emit newAnswer(time + text);
        if(m_logFile->isOpen()){
            QTextStream logStream(m_logFile);
            logStream << time << text << QString("\r");
            logStream.flush();
        }

}

bool SerialCommunication::connectToDevice(QString device)
{
    if( ! m_serialPort->isOpen() ) {
        QSerialPortInfo spi(device);
        m_serialPort->setPort(spi);
        if(spi.isValid()){
            if(m_serialPort->open((QIODevice::OpenModeFlag)m_openMode)){
                m_serialPort->setBaudRate((QSerialPort::BaudRate)m_baudRate);
                m_serialPort->setDataBits((QSerialPort::DataBits)m_dataBits);
                m_serialPort->setStopBits((QSerialPort::StopBits)m_stopBits);
                m_serialPort->setParity((QSerialPort::Parity)m_partity);
                writeLog(tr("Successfuly connected to device."));
                return true;
            }else{
                writeLog(tr("Cannot connect to device."));
                return false;
            }
        }else{
            writeLog(QString(tr("Device [ %1 ] not valid.")).arg(spi.systemLocation()));
            return false;
        }
    }else{
        writeLog(tr("The device is already open."));
        return false;
    }
}

bool SerialCommunication::disconnectFromDevice()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
        writeLog(tr("Disconnected from device."));
        return true;
    }else{
        writeLog(tr("INTERNAL ERROR: Can't disconnect from device because it is not connected."));
        return false;
    }

}

bool SerialCommunication::sendCommand(const QString &command)
{
    if(m_serialPort->isWritable()){
        writeLog(QString(tr("Command sent: '%1'")).arg(command));
        if (m_serialPort->write(command.toLocal8Bit()) == -1){
            writeLog(tr("Error occorred writing to device."));
            return false;
        }
        return true;
    }else{
        writeLog(tr("Cannot send command to device. Device is not writable."));
        return false;
    }
}

void SerialCommunication::handleError(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError){
        writeLog(QString(tr("Serial Port Error: %1")).arg(m_serialPort->errorString()));
    }
}

void SerialCommunication::dataRead()
{
    if(m_serialPort->canReadLine())
        writeLog(QString(m_serialPort->readAll()));
//        writeLog(QString(tr("Device answer: ")) + QString(m_serialPort->readAll()));
}
