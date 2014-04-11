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

class ReaderInteractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReaderInteractorWidget(const Settings::ConnectionType type, QWidget *parent = 0);
    ~ReaderInteractorWidget();
    void closeConnection();

private:
    Ui::ReaderInteractorWidget *ui;
    SerialCommunication *m_serialCommunication;
    NetworkCommunication *m_networkCommunication;
    Settings::ConnectionType m_connectionType;
    QFile *m_logFile;
    bool m_useLogFile;
    RICTLMB2B30Widget *m_mb2b30;

    void sendCommand(const QString &command);
    void writeToOutput(const QString &text);

public slots:
    void newAnswerFromSerialComm(const QString answer);
    void newAnswerFromNetworkComm(const QString answer);
    void btSendCommandClicked();
    void leCommandReturnPressed();
    void btClearOutputClicked();
    void btLogToClicked();
    void btStartPauseReadingClicked(const bool checked);
};

#endif // READERINTERACTORWIDGET_H
