#ifndef RICTLMB2B30WIDGET_H
#define RICTLMB2B30WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QRegExpValidator>

#include "settings.h"

namespace Ui {
class RICTLMB2B30Widget;
}

/**
 * @brief The RICTLMB2B30Widget class is the custom class to interact with the RI-CTL_MB2B-30 Reader.
 */
class RICTLMB2B30Widget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief RICTLMB2B30Widget is the constructor of the RICTLMB2B30Widget class, that must receive
     * two parameters.
     * @param connType Say to the class wich connection it must use (serial or network).
     * @param parent is parent of the class object to control the lifetime.
     */
    explicit RICTLMB2B30Widget(Settings::ConnectionType connType, QWidget *parent = 0);
    ~RICTLMB2B30Widget();

private:
    Ui::RICTLMB2B30Widget *ui;

    /**
     * @brief m_waitingForAnswer says if the reading process must check out for especific mapped answers.
     */
    bool m_waitingForAnswer;

    /**
     * @brief m_waitingForCheck says if the reading process must check if the transponder really have
     * the new identification code.
     */
    bool m_waitingForCheck;

    /**
     * @brief m_identification holds the identification code to be writed in the transponder.
     */
    QString m_identification;

    /**
     * @brief m_connectionType holds the connection type (KSerial or KNetwork).
     */
    Settings::ConnectionType m_connectionType;

    /**
     * @brief sendCommand send a string command to device using the defined connection.
     * @param command is the string command.
     */
    void sendCommand(const QString &command);

    /**
     * @brief m_timeout handle the timeout time waiting for a answer or a check for the
     * writing new identification code process.
     */
    QTimer m_timeout;

    /**
     * @brief m_hexaValidator holds the regular expression to validate a hexadecimal identification
     * code.
     */
    QRegExpValidator *m_hexaValidator;

    /**
     * @brief m_deciValidator holds the regular expression to validate a decimal identification
     * code.
     */
    QRegExpValidator *m_deciValidator;

    /**
     * @brief lockForms disable all interation methods in this window.
     */
    void lockForms();

public slots:

    /**
     * @brief leIdentificationChanged is the lost called every time the text in the QLineEdit changes.
     *
     * This function parse the identification code every time it is called to the labelIdentificationStatus,
     * if the decimal type is selected. Else simple replicate it, formated as 16 digits in the label.
     *
     * @param newText contains the new text from the field.
     */
    void leIdentificationChanged(QString newText);

    /**
     * @brief btWriteClicked do the process to write new identification in the transponder.
     */
    void btWriteClicked();

    /**
     * @brief newAnswerFromSerialComm is a slot that receives new answers from the device.
     * @param answer is the answer in string format.
     */
    void newAnswerFromSerialComm(QString answer);

    /**
     * @brief timeout is a slot that call procedures after a timeout occurs.
     */
    void timeout();

    /**
     * @brief rbDecimalClicked is a slot called when the user change the identification code type.
     *
     * When this slot is called it validate the value of the identification against the regular expression
     * and call the leIdentificationChanged in the end.
     */
    void rbDecimalClicked();

    /**
     * @brief rbHexadecimalClicked is a slot called when the user change the identification code type.
     *
     * When this slot is called it validate the value of the identification against the regular expression
     * and call the leIdentificationChanged in the end.
     */
    void rbHexadecimalClicked();

    /**
     * @brief incrementIdentification is a slot called when the user wants to increment the value
     * of the identification code.
     *
     * This function gets the code, increment it, set it back to the QLineEdit and consequently
     * leIdentificationChanged is called.
     *
     */
    void incrementIdentification();

    /**
     * @brief connectionFinished is a slot called when the connection is lost.
     *
     * This will block the window to the user dont try to send new commands.
     */
    void communicationFinished();
};

#endif // RICTLMB2B30WIDGET_H
