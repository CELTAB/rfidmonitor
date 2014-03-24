#ifndef READERINTERACTORWIDGET_H
#define READERINTERACTORWIDGET_H

#include <QWidget>

#include <communication/serialcommunication.h>
#include <communication/networkcommunication.h>
#include <settings.h>

namespace Ui {
class ReaderInteractorWidget;
}

class ReaderInteractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReaderInteractorWidget(Settings::ConnectionType type, QWidget *parent = 0);
    ~ReaderInteractorWidget();

private:
    Ui::ReaderInteractorWidget *ui;
    SerialCommunication *m_serialCommunication;
    NetworkCommunication *m_networkCommunication;
    Settings::ConnectionType m_connectionType;

    void sendCommand(const QString command);

public slots:
    void newAnswerFromSerialComm(QString answer);
    void btSendCommandClicked();
};

#endif // READERINTERACTORWIDGET_H
