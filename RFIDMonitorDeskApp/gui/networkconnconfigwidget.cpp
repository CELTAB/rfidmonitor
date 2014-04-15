#include <QTimer>
#include <QStandardItem>
#include <QMapIterator>

#include "networkconnconfigwidget.h"
#include "ui_networkconnconfigwidget.h"

NetworkConnConfigWidget::NetworkConnConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkConnConfigWidget)
{
    ui->setupUi(this);
    ui->btStopRaspSearch->setEnabled(false);

    m_raspFoundModel = new DeviceModel;
    ui->lvDevicesFound->setModel(m_raspFoundModel);

    connect(ui->btRaspSearch, SIGNAL(clicked()), this, SLOT(btRaspSearchClicked()));
    connect(ui->btStopRaspSearch, SIGNAL(clicked()), this, SLOT(stopSearchingRasp()));
    connect(ui->lvDevicesFound, SIGNAL(clicked(QModelIndex)), this, SLOT(listViewClicked(QModelIndex)));
    connect(ui->btConnectToRasp, SIGNAL(clicked()), this, SLOT(btConnectToRaspClicked()));
    connect(m_raspFoundModel, SIGNAL(deviceRemoved(QString)),this,SLOT(newRaspTimeout(QString)));
    connect(NetworkCommunication::instance(), SIGNAL(newRaspFound(QVariantMap)), this, SLOT(newRaspFound(QVariantMap)));
    connect(NetworkCommunication::instance(), SIGNAL(connectionFailed()), this, SLOT(connectionFailed()));


}

NetworkConnConfigWidget::~NetworkConnConfigWidget()
{
    m_raspFoundModel->deleteLater();
    delete ui;
}

bool NetworkConnConfigWidget::isReaderInteractorSelected()
{
    return ui->rbReaderInteraction->isChecked();
}

void NetworkConnConfigWidget::btRaspSearchClicked()
{
    ui->btRaspSearch->setEnabled(false);
    ui->btStopRaspSearch->setEnabled(true);
    NetworkCommunication::instance()->startListeningBroadcast();
}

void NetworkConnConfigWidget::stopSearchingRasp()
{   
    ui->btRaspSearch->setEnabled(true);
    ui->btStopRaspSearch->setEnabled(false);
    NetworkCommunication::instance()->stopListeningBroadcast();
    m_raspFoundModel->clear();
    ui->leDeviceChosen->clear();
}

void NetworkConnConfigWidget::newRaspFound(const QVariantMap raspInfo)
{

    m_raspFoundModel->addDevice(raspInfo.value("raspmac").toString(), raspInfo.value("raspaddress").toString(), raspInfo.value("daemonport").toInt());
}

void NetworkConnConfigWidget::newRaspTimeout(const QString deviceDisplayRole)
{
    if(ui->leDeviceChosen->text().compare(deviceDisplayRole) == 0)
        ui->leDeviceChosen->clear();
}

void NetworkConnConfigWidget::btConnectToRaspClicked()
{
    if( ! ui->leDeviceChosen->text().isEmpty()){
        QModelIndex index = ui->lvDevicesFound->currentIndex();
        if(index.isValid()){
            NetworkCommunication::instance()->connectToRasp(m_raspFoundModel->deviceIPAddress(index).toString(),
                                                            m_raspFoundModel->deviceDaemonPort(index).toInt());
        }
    }
}

void NetworkConnConfigWidget::listViewClicked(const QModelIndex index)
{

    ui->leDeviceChosen->setText(index.data(Qt::DisplayRole).toString());
}

void NetworkConnConfigWidget::connectionFailed()
{
    //fazer
}
