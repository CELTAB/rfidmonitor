#ifndef READERINTERACTORWIDGET_H
#define READERINTERACTORWIDGET_H

#include <QWidget>
#include <QFile>

#include <communication/serialcommunication.h>
#include <communication/networkcommunication.h>
#include <settings.h>

#include "rictlmb2b30widget.h"

namespace Ui {
class ReaderInteractorWidget;
}

/**
 * @brief The ReaderInteractorWidget class provides the way to interact with a serial device,
 * using a Terminal Window or a especific window for a device.
 */
class ReaderInteractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReaderInteractorWidget(const Settings::ConnectionType type, QWidget *parent = 0);
    ~ReaderInteractorWidget();

    /**
     * @brief closeConnection closes the existing connection (serial or network).
     */
    void closeConnection();

private:
    Ui::ReaderInteractorWidget *ui;

    /**
     * @brief m_connectionType holds the connection type used in this interaction with the reader.
     */
    Settings::ConnectionType m_connectionType;

    /**
     * @brief m_logFile holds the log file used to write the answers comming from the device and
     * the commands sent.
     */
    QFile *m_logFile;

    /**
     * @brief m_useLogFile says if the log file must be used or not.
     */
    bool m_useLogFile;

    /**
     * @brief m_mb2b30 holds the instance of the custom interactor widget for the RI-CTL-MB2B-30 reader.
     */
    RICTLMB2B30Widget *m_mb2b30;

    /**
     * @brief sendCommand sends a command to through the connection.
     *
     * This function analyses the m_connectionType and choose to send by serial or network.
     *
     * @param command is the command in string format.
     */
    void sendCommand(const QString &command);

    /**
     * @brief writeToOutput writes in the answers from the connection to QTextEdit and in the log file if defined.
     * @param text is the answer from the device or the command sent.
     */
    void writeToOutput(const QString &text);

    /**
     * @brief lockForms disable all interation methods in this window.
     */
    void lockForms();

public slots:

    /**
     * @brief newAnswerFromSerialComm is a slot that receive answers from Serial connection.
     * @param answer is the answer in string format.
     */
    void newAnswerFromSerialComm(const QString answer);

    /**
     * @brief newAnswerFromNetworkComm is a slot that receive answers from Network connection.
     * @param answer is the answer in string format.
     */
    void newAnswerFromNetworkComm(const QString answer);

    /**
     * @brief btSendCommandClicked is a slot called by button Send, and then calls the sendCommand() function.
     */
    void btSendCommandClicked();

    /**
     * @brief leCommandReturnPressed is a slot called by return in the command line,
     * and then calls the sendCommand() function.
     */
    void leCommandReturnPressed();

    /**
     * @brief btClearOutputClicked is a slot to clean the output QTextEdit.
     */
    void btClearOutputClicked();

    /**
     * @brief btLogToClicked is a slot that opens a QFileDialog that user can use
     * to find the log file for the write the outputs.
     */
    void btLogToClicked();

    /**
     * @brief btStartPauseReadingClicked is a slot that feed the output QTextEdit with answers
     * from the connection.
     *
     * This function only connect and disconnect the slots newAnswerFromSerialComm and newAnswerFromNetworkComm.
     * Even paused, the connection continues sending the answers, but are not shown.
     *
     * @param checked true if must start, and false if must pause.
     */
    void btStartPauseReadingClicked(const bool checked);

    /**
     * @brief connectionFinished is a slot called when the connection is lost.
     *
     * This will block the window to the user dont try to send new commands.
     */
    void connectionFinished();
};

#endif // READERINTERACTORWIDGET_H
