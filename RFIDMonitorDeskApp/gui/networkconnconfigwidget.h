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
    enum HandleRaspType {KTimeout = 0, KAvaiable};
    explicit NetworkConnConfigWidget(QWidget *parent = 0);
    ~NetworkConnConfigWidget();
    bool isReaderInteractorSelected();

private:
    Ui::NetworkConnConfigWidget *ui;
    DeviceModel *m_raspFoundModel;

public slots:
    void btRaspSearchClicked();
    void stopSearchingRasp();
    void newRaspFound(const QVariantMap raspInfo);
    void newRaspTimeout(const QString deviceDisplayRole);
    void btConnectToRaspClicked();
    void listViewClicked(const QModelIndex index);
    void connectionFailed();

signals:
    void networkCommunicationReady(Settings::InteractionType interactionType);
};

#endif // NETWORKCONNCONFIGWIDGET_H
