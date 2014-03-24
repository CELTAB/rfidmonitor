#ifndef SERIALCONNCONFIGWIDGET_H
#define SERIALCONNCONFIGWIDGET_H

#include <QWidget>

#include "communication/serialcommunication.h"

namespace Ui {
class SerialConnConfigWidget;
}

class SerialConnConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialConnConfigWidget(QWidget *parent = 0);
    ~SerialConnConfigWidget();


private:
    Ui::SerialConnConfigWidget *ui;
    SerialCommunication *m_serialCommunication;
    void refreshDeviceList();
    void connectToDevice();
    void disconnectFromDevice();
    void populateFields();
    void searchLogFile();
    void clearOutput();

public slots:
    void btRefreshDeviceClicked();
    void btConnectToDeviceClicked();
    void btDisconnectFromDeviceClicked();
    void btSearchLogFileClicked();
    void btClearOutput();
    void leCommandLineSent();

signals:
    void serialCommunicationReady();
};

#endif // SERIALCONNCONFIGWIDGET_H
