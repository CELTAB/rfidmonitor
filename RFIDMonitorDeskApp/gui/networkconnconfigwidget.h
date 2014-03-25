#ifndef NETWORKCONNCONFIGWIDGET_H
#define NETWORKCONNCONFIGWIDGET_H

#include <QWidget>
#include <QVariantMap>
#include <QStandardItemModel>
#include <QModelIndex>

#include "communication/networkcommunication.h"
#include "settings.h"

namespace Ui {
class NetworkConnConfigWidget;
}

class NetworkConnConfigWidget : public QWidget
{
    Q_OBJECT

public:
    enum HandleRaspType {KTimeout = 0, KAvaiable};
    explicit NetworkConnConfigWidget(QWidget *parent = 0);
    ~NetworkConnConfigWidget();
    bool isReaderInteractorSelected();

private:
    Ui::NetworkConnConfigWidget *ui;
    NetworkCommunication *m_networkCommunication;
    QStandardItemModel *m_raspFoundModel;
    QModelIndex m_selectedDevice;


public slots:
    void btRaspSearchClicked();
    void btStopRaspSearchClicked();
    void newRaspFound(QVariantMap raspInfo);
    void removeOlderItem();
    void btConnectToRaspClicked();
    void listViewClicked(QModelIndex index);
    void connectionFailed();

signals:
    void networkCommunicationReady(Settings::InteractionType interactionType);
};

#endif // NETWORKCONNCONFIGWIDGET_H
