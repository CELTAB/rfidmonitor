#ifndef NETWORKCONNCONFIGWIDGET_H
#define NETWORKCONNCONFIGWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include <QStandardItemModel>
#include <QModelIndex>

#include "communication/networkcommunication.h"
#include "settings.h"
#include "devicemodel.h"

namespace Ui {
class NetworkConnConfigWidget;
}

class NetworkConnConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkConnConfigWidget(QWidget *parent = 0);
    ~NetworkConnConfigWidget();

    /**
     * @brief isReaderManipulatorSelected return if the reader interaction
     * was selected or not.
     *
     * @return true if reader Manipulator is selected, and false otherwise.
     */
    bool isReaderManipulatorSelected();

private:
    Ui::NetworkConnConfigWidget *ui;

    /**
     * @brief m_raspFoundModel is the model used in the QListView, showing the
     * connected rasps.
     */
    DeviceModel *m_raspFoundModel;

    /**
     * @brief resetForm back the window state to the start.
     */
    void resetForm();

public slots:

    /**
     * @brief btRaspSearchClicked called when the button to search rasps is clicked.
     */
    void btRaspSearchClicked();

    /**
     * @brief btStopSearchingRaspClicked called when the button to stop searching
     * rasps is clicked.
     */
    void btStopSearchingRaspClicked();

    /**
     * @brief newRaspFound called when the NetworkCommunication says there is a new
     * rasp successfully connected.
     *
     * This function takes the data of the new device, and add it to the device model.
     *
     * @param raspInfo contains a map of info about the new device.
     */
    void newRaspFound(const QVariantMap raspInfo);

    /**
     * @brief raspDisconnected called when NetworkCommunication says a rasp must be
     * removed from the model.
     *
     * @param ipAddress contains the IP to identify the device to be removed.
     */
    void raspDisconnected(const QString ipAddress);

    /**
     * @brief btConnectToRaspClicked called when the button to connect to rasp is clicked.
     *
     * This set the main current connection, and close all other connections.
     */
    void btConnectToRaspClicked();

    /**
     * @brief listViewClicked called when a device is clicked in the list view.
     * @param index represent which device was clicked.
     */
    void listViewClicked(const QModelIndex index);

    /**
     * @brief connectionEstablished called when NetworkCommunication says the new current
     * main connection is successfully defined.
     */
    void connectionEstablished();

signals:
    /**
     * @brief networkCommunicationReady emmit to say a new current main connection is
     * ready.
     *
     * @param interactionType says which interation type to use: Reader or RFIDMonitor.
     */
    void networkCommunicationReady(Settings::InteractionType interactionType);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // NETWORKCONNCONFIGWIDGET_H
