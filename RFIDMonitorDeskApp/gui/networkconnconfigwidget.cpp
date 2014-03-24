#include <QTimer>
#include <QStandardItem>

#include "networkconnconfigwidget.h"
#include "ui_networkconnconfigwidget.h"

NetworkConnConfigWidget::NetworkConnConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkConnConfigWidget)
{
    ui->setupUi(this);
    connect(ui->btRaspSearch, SIGNAL(clicked()), this, SLOT(btRaspSearchClicked()));
    connect(ui->btStopRaspSearch, SIGNAL(clicked()), this, SLOT(btStopRaspSearchClicked()));
    connect(ui->lvDevicesFound, SIGNAL(clicked(QModelIndex)), this, SLOT(listViewClicked(QModelIndex)));
    connect(ui->btConnectToRasp, SIGNAL(clicked()), this, SLOT(btConnectToRaspClicked()));

    connect(NetworkCommunication::instance(), SIGNAL(newRaspFound(QVariantMap)), this, SLOT(newRaspFound(QVariantMap)));

    ui->btStopRaspSearch->setEnabled(false);

    m_raspFoundModel = new QStandardItemModel(this);
    ui->lvDevicesFound->setModel(m_raspFoundModel);
    m_selectedDevice = QModelIndex();

}

NetworkConnConfigWidget::~NetworkConnConfigWidget()
{
    delete ui;
}

void NetworkConnConfigWidget::btRaspSearchClicked()
{
    ui->btRaspSearch->setEnabled(false);
    ui->btStopRaspSearch->setEnabled(true);
    NetworkCommunication::instance()->startListeningBroadcast();
}

void NetworkConnConfigWidget::btStopRaspSearchClicked()
{   
    ui->btRaspSearch->setEnabled(true);
    ui->btStopRaspSearch->setEnabled(false);
    NetworkCommunication::instance()->stopListeningBroadcast();
}

void NetworkConnConfigWidget::newRaspFound(QVariantMap raspInfo)
{
    QStandardItem *item = new QStandardItem;
    item->setEditable(false);
    QString viewInfo("MAC: %1 , IP: %2");
    viewInfo = viewInfo.arg(raspInfo.value("raspmac").toString()).arg(raspInfo.value("raspaddress").toString());
    QList<QStandardItem *> list(m_raspFoundModel->findItems(viewInfo));
    if(list.isEmpty()){
        item->setData(QVariant(viewInfo), Qt::DisplayRole);
        item->setData(raspInfo, Qt::UserRole);
        m_raspFoundModel->appendRow(item);
        QTimer::singleShot(10000, this, SLOT(removeOlderItem()));

    }
}

void NetworkConnConfigWidget::removeOlderItem()
{
    m_raspFoundModel->removeRow(0);
}

void NetworkConnConfigWidget::btConnectToRaspClicked()
{
    if(m_selectedDevice.isValid()){
        QVariantMap raspInfo= m_selectedDevice.data(Qt::UserRole).toMap();
        if(NetworkCommunication::instance()->connectToRasp(raspInfo.value("raspaddress").toString(),
                                                           raspInfo.value("daemonport").toInt())){
            NetworkCommunication::instance()->stopListeningBroadcast();

            if(ui->rbReaderInteraction->isChecked())
                emit networkCommunicationReady(Settings::KReader);
            else if(ui->rbRFIDMonitorInteraction->isChecked())
                emit networkCommunicationReady(Settings::KRFIDMonitor);

        }else{
            //failed to connect to rasp
        }
    }

}

void NetworkConnConfigWidget::listViewClicked(QModelIndex index)
{
    m_selectedDevice = index;
    ui->leDeviceChosen->setText(index.data(Qt::DisplayRole).toString());
}
