#include <QTimer>
#include <QStandardItem>
#include <QMapIterator>
#include <QIcon>

#include "networkconnconfigwidget.h"
#include "ui_networkconnconfigwidget.h"

NetworkConnConfigWidget::NetworkConnConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkConnConfigWidget)
{
    ui->setupUi(this);
    ui->btStopRaspSearch->setEnabled(false);
    ui->btRaspSearch->setIcon(QIcon(":/icons/icon-search"));
    ui->btStopRaspSearch->setIcon(QIcon(":/icons/icon-cancel"));
    ui->btConnectToRasp->setIcon(QIcon(":/icons/icon-connect"));

    m_raspFoundModel = new DeviceModel;
    ui->lvDevicesFound->setModel(m_raspFoundModel);

    connect(ui->btRaspSearch, SIGNAL(clicked()), this, SLOT(btRaspSearchClicked()));
    connect(ui->btStopRaspSearch, SIGNAL(clicked()), this, SLOT(btStopSearchingRaspClicked()));
    connect(ui->lvDevicesFound, SIGNAL(clicked(QModelIndex)), this, SLOT(listViewClicked(QModelIndex)));
    connect(ui->btConnectToRasp, SIGNAL(clicked()), this, SLOT(btConnectToRaspClicked()));
    connect(NetworkCommunication::instance(), SIGNAL(raspDisconnected(QString)),this,SLOT(raspDisconnected(QString)));
    connect(NetworkCommunication::instance(), SIGNAL(newRaspFound(QVariantMap)), this, SLOT(newRaspFound(QVariantMap)));
    connect(NetworkCommunication::instance(), SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));
}

NetworkConnConfigWidget::~NetworkConnConfigWidget()
{
    m_raspFoundModel->deleteLater();
    delete ui;
}

bool NetworkConnConfigWidget::isReaderManipulatorSelected()
{
    return ui->rbReaderInteraction->isChecked();
}

void NetworkConnConfigWidget::resetForm()
{
    ui->btRaspSearch->setEnabled(true);
    ui->btStopRaspSearch->setEnabled(false);
    m_raspFoundModel->clear();
    ui->leDeviceChosen->clear();
}

void NetworkConnConfigWidget::btRaspSearchClicked()
{
    ui->btRaspSearch->setEnabled(false);
    ui->btStopRaspSearch->setEnabled(true);
    NetworkCommunication::instance()->startBroadcast();
}

void NetworkConnConfigWidget::btStopSearchingRaspClicked()
{   
    NetworkCommunication::instance()->stopBroadcast();
    resetForm();
}

void NetworkConnConfigWidget::newRaspFound(const QVariantMap raspInfo)
{
    m_raspFoundModel->addDevice(
                raspInfo.value("name").toString(),
                raspInfo.value("macaddress").toString(),
                raspInfo.value("ipaddress").toString()
                );
}

void NetworkConnConfigWidget::raspDisconnected(const QString ipAddress)
{
    m_raspFoundModel->removeDevice(ipAddress);

    if(ui->leDeviceChosen->text().contains(ipAddress))
        ui->leDeviceChosen->clear();
}

void NetworkConnConfigWidget::btConnectToRaspClicked()
{
    if( ! ui->leDeviceChosen->text().isEmpty()){
        QModelIndex index = ui->lvDevicesFound->currentIndex();
        if(index.isValid()){
            NetworkCommunication::instance()->connectToRasp(m_raspFoundModel->deviceIPAddress(index).toString());
        }
    }
}

void NetworkConnConfigWidget::listViewClicked(const QModelIndex index)
{
    ui->leDeviceChosen->setText(index.data(Qt::DisplayRole).toString());
}

void NetworkConnConfigWidget::connectionEstablished()
{
    resetForm();
}

void NetworkConnConfigWidget::resizeEvent(QResizeEvent *event)
{
    //Analize both buttons and set the size from the bigger to
    // the smaller.
    if(ui->btRaspSearch->width() > ui->btStopRaspSearch->width())
        ui->btStopRaspSearch->setMinimumWidth(ui->btRaspSearch->width());
    else
        ui->btRaspSearch->setMinimumWidth(ui->btStopRaspSearch->width());


    QWidget::resizeEvent(event);
}
