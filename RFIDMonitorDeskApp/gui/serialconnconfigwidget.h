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

    /**
     * @brief refreshDeviceList offers the way to refresh the list of available connected devices.
     */
    void refreshDeviceList();

    /**
     * @brief connectToDevice offers the way to connect to the device, defined with the connection
     * parameters by the user in the window.
     */
    void connectToDevice();

    /**
     * @brief populateFields load the serial communication options in the window for the user.
     */
    void populateFields();

public slots:
    /**
     * @brief btRefreshDeviceClicked is a slot that calls the refreshDeviceList when the user ask for it
     * in the window button.
     */
    void btRefreshDeviceClicked();

    /**
     * @brief btConnectToDeviceClicked is a slot that calls connectToDevice when the user ask for it
     * in the window button.
     */
    void btConnectToDeviceClicked();

signals:
    /**
     * @brief serialCommunicationReady is a signal emmited when the connection is ready.
     */
    void serialCommunicationReady();
};

#endif // SERIALCONNCONFIGWIDGET_H
