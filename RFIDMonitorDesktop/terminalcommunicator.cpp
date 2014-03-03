#include <QSerialPortInfo>
#include <QStringList>
#include <QDebug>
#include <QTime>

#include "terminalcommunicator.h"

TerminalCommunicator::TerminalCommunicator(QObject *parent) :
    QObject(parent)
{
    m_serialPort = new QSerialPort(this);
    m_textEdit = 0;
    m_logFile = new QFile(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &TerminalCommunicator::dataRead);
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}

QStringList TerminalCommunicator::availablePorts()
{
    QStringList deviceList;
    foreach (QSerialPortInfo spi, QSerialPortInfo::availablePorts()) {
        deviceList.append(spi.portName());
    }
    return deviceList;
}

void TerminalCommunicator::setTextEdit(QTextEdit *textEdit)
{
    m_textEdit = textEdit;
}

TerminalCommunicator::~TerminalCommunicator()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
    }
    m_serialPort->deleteLater();

    if(m_logFile->isOpen()){
        m_logFile->close();
    }
    m_logFile->deleteLater();
}
qint32 TerminalCommunicator::baudRate() const
{
    return m_baudRate;
}

void TerminalCommunicator::setBaudRate(const qint32 &baudRate)
{
    m_baudRate = baudRate;
}
qint32 TerminalCommunicator::dataBits() const
{
    return m_dataBits;
}

void TerminalCommunicator::setDataBits(const qint32 &dataBits)
{
    m_dataBits = dataBits;
}
qint32 TerminalCommunicator::stopBits() const
{
    return m_stopBits;
}

void TerminalCommunicator::setStopBits(const qint32 &stopBits)
{
    m_stopBits = stopBits;
}
qint32 TerminalCommunicator::partity() const
{
    return m_partity;
}

void TerminalCommunicator::setPartity(const qint32 &partity)
{
    m_partity = partity;
}
qint32 TerminalCommunicator::openMode() const
{
    return m_openMode;
}

void TerminalCommunicator::setOpenMode(const qint32 &openMode)
{
    m_openMode = openMode;
}
QString TerminalCommunicator::logFile() const
{
    return m_logFile->fileName();
}

void TerminalCommunicator::setLogFile(const QString &logFile, const qint32 &openMode)
{
    if(m_logFile->isOpen())
        m_logFile->close();

    if( ! logFile.isEmpty()){
        m_logFile->setFileName(logFile);
        if(m_logFile->open((QIODevice::OpenModeFlag)openMode) && m_logFile->isWritable()){
            writeToTextEdit(tr("Log file ok."));
        }else{
            if(m_logFile->isOpen())
                m_logFile->close();

            writeToTextEdit(tr("Failed to use the log file."));
        }
    }
}

void TerminalCommunicator::writeToTextEdit(QString text)
{
    if(m_textEdit){
        QString time("[" + QTime::currentTime().toString() + "] ");

        m_textEdit->append(time + text);
        if(m_logFile->isOpen()){
            QTextStream logStream(m_logFile);
            logStream << time << text << QString("\r");
            logStream.flush();
        }
    }else{
        qDebug() << QString(tr("INTERNAL ERROR: no QTextEdit from view defined on TerminalCommunicator class."));
    }

}

bool TerminalCommunicator::connectToDevice(QString device)
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
                writeToTextEdit(tr("Successfuly connected to device."));
                return true;
            }else{
                writeToTextEdit(tr("Cannot connect to device."));
                return false;
            }
        }else{
            writeToTextEdit(QString(tr("Device [ %1 ] not valid.")).arg(spi.systemLocation()));
            return false;
        }
    }else{
        writeToTextEdit(tr("The device is already open."));
        return false;
    }
}

bool TerminalCommunicator::disconnectFromDevice()
{
    if(m_serialPort->isOpen()){
        m_serialPort->close();
        writeToTextEdit(tr("Disconnected from device."));
        return true;
    }else{
        writeToTextEdit(tr("INTERNAL ERROR: Can't disconnect from device because it is not connected."));
        return false;
    }

}

bool TerminalCommunicator::sendCommand(QString command)
{
    if(m_serialPort->isWritable()){
        writeToTextEdit(QString(tr("Command sent: '%1'")).arg(command));
        if (m_serialPort->write(command.toLocal8Bit()) == -1){
            writeToTextEdit(tr("Error occorred writing to device."));
            return false;
        }
        return true;
    }else{
        writeToTextEdit(tr("Cannot send command to device. Device is not writable."));
        return false;
    }
}


void TerminalCommunicator::handleError(QSerialPort::SerialPortError error)
{
    if(error != QSerialPort::NoError){
        writeToTextEdit(QString(tr("Serial Port Error: %1")).arg(m_serialPort->errorString()));
    }
}

void TerminalCommunicator::dataRead()
{
    if(m_serialPort->canReadLine())
        writeToTextEdit(QString(tr("Device answer: ")) + QString(m_serialPort->readAll()));
}
