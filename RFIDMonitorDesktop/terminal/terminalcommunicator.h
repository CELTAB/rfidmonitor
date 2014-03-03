#ifndef TERMINALCOMMUNICATOR_H
#define TERMINALCOMMUNICATOR_H

#include <QObject>
#include <QSerialPort>
#include <QTextEdit>
#include <QIODevice>
#include <QFile>
#include <QTextStream>

class TerminalCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit TerminalCommunicator(QObject *parent = 0);
    QStringList availablePorts();
    void setTextEdit(QTextEdit *textEdit);
    bool connectToDevice(QString device);
    bool disconnectFromDevice();
    bool sendCommand(QString command);
    ~TerminalCommunicator();

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

private:
    QSerialPort *m_serialPort;
    QTextEdit *m_textEdit;
    QFile *m_logFile;
    qint32 m_logFileMode;
    qint32 m_baudRate;
    qint32 m_dataBits;
    qint32 m_stopBits;
    qint32 m_partity;
    qint32 m_openMode;


    void writeToTextEdit(QString text);

signals:

public slots:
    void handleError(QSerialPort::SerialPortError error);
    void dataRead();
};

#endif // TERMINALCOMMUNICATOR_H
