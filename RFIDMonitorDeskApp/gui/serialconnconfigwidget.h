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
    void populateFields();

public slots:
    void btRefreshDeviceClicked();
    void btConnectToDeviceClicked();

signals:
    void serialCommunicationReady();
};

#endif // SERIALCONNCONFIGWIDGET_H
