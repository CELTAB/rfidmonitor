#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QIODevice>
#include <QSerialPort>
#include <QFile>
#include <QStringList>

/**
 * @brief The SerialCommunication class offers the way to communicate with a device by serial.
 */
class SerialCommunication : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief The CommandType enum is used to define how the format of the byte array.
     */
    enum CommandType {
        KASCII = 0, /** enum KASCII value says the byte array must be writed as a normal ASCII string.*/
        KNumber /** enum value KNumber says the byte array must be parsed to number (int) before writting.*/
    };

    /**
     * @brief instance implents the singleton pattern of this class.
     * @return the SerialCommunication unique instance.
     */
    static SerialCommunication* instance();

    /**
     * @brief availablePorts return the list of all available serial ports in the computer.
     * @return QStringList with available serial ports path.
     */
    QStringList availablePorts();

    /**
     * @brief connectToDevice try to stablish connection to a serial device.
     * @param device is the Device path.
     * @param openMode is the open mode (only read, only write or read/write).
     * @param baudRate is the Baud Rate.
     * @param dataBits is the Data Bits.
     * @param stopBits is the Stop Bits.
     * @param parity is the Parity.
     * @return true if connected or false if not connected.
     */
    bool connectToDevice(const QString &device,
                         const QIODevice::OpenModeFlag &openMode,
                         const QSerialPort::BaudRate &baudRate,
                         const QSerialPort::DataBits &dataBits,
                         const QSerialPort::StopBits &stopBits,
                         const QSerialPort::Parity &parity);

    /**
     * @brief disconnectFromDevice if the connection with the device is open, close it.
     */
    void disconnectFromDevice();

    /**
     * @brief sendCommand prepares a QString command with the CommandType, and write it in the serial device.
     * @param command is the command in string format.
     * @param type defines the format of the command (ASCII or number).
     * @return return true if sucessfuly writed or false if could not write.
     */
    bool sendCommand(const QString &command, const SerialCommunication::CommandType &type);

public slots:
    /**
     * @brief handleError is a slot connected with the QSerialPort::error signal.
     *
     * This function prints all detected errors in the connection, but do not close it.
     *
     * @param error the error type received from QSerialPort signal.
     */
    void handleError(const QSerialPort::SerialPortError error);

    /**
     * @brief dataRead is a slot called when are new bytes in the device buffer.
     */
    void dataRead();

signals:
    /**
     * @brief newAnswer is a signal that emits a complete answer arrived from the connected device.
     * @param answer is a string from the byte array.
     */
    void newAnswer(QString answer);

private:
    explicit SerialCommunication(QObject *parent = 0);

    /**
     * @brief m_serialPort is a unique working instance for serial communication.
     */
    QSerialPort *m_serialPort;

    /**
     * @brief writeAnswer is the function called when exists a new complete answer (complete line) from the serial device.
     * @param text is the complete answer.
     */
    void writeAnswer(QString text);
};

#endif // SERIALCOMMUNICATION_H
