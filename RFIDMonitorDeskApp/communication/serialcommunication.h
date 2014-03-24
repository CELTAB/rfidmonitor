#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QIODevice>
#include <QSerialPort>
#include <QFile>
#include <QStringList>

class SerialCommunication : public QObject
{
    Q_OBJECT
public:
    static SerialCommunication* instance();
    ~SerialCommunication();
    QStringList availablePorts();
    bool connectToDevice(QString device);
    bool disconnectFromDevice();
    bool sendCommand(const QString &command);

    qint32 baudRate() const;
    void setBaudRate(const qint32 &baudRate);
    qint32 dataBits() const;
    void setDataBits(const qint32 &dataBits);
    qint32 stopBits() const;
    void setStopBits(const qint32 &stopBits);
    qint32 partity() const;
    void setPartity(const qint32 &partity);
    qint32 openMode() const;
    void setOpenMode(const qint32 &openMode);
    QString logFile() const;
    void setLogFile(const QString &logFile, const qint32 &openMode = QIODevice::Append);

public slots:
    void handleError(QSerialPort::SerialPortError error);
    void dataRead();

signals:
    void newAnswer(QString answer);

private:
    explicit SerialCommunication(QObject *parent = 0);
    QSerialPort *m_serialPort;
    QFile *m_logFile;
    qint32 m_logFileMode;
    qint32 m_baudRate;
    qint32 m_dataBits;
    qint32 m_stopBits;
    qint32 m_partity;
    qint32 m_openMode;

    void writeLog(QString text);
};

#endif // SERIALCOMMUNICATION_H
