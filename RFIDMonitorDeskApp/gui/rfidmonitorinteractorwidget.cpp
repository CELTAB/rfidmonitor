#include <QStandardItem>
#include <QStandardItemModel>

#include "rfidmonitorinteractorwidget.h"
#include "ui_rfidmonitorinteractorwidget.h"
#include "communication/networkcommunication.h"

RFIDMonitorInteractorWidget::RFIDMonitorInteractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RFIDMonitorInteractorWidget)
{
    ui->setupUi(this);

    //TESTE

    QStandardItemModel *standardModel = new QStandardItemModel ;
    QStandardItem *rootNode = standardModel->invisibleRootItem();

    //defining a couple of items
    QStandardItem *americaItem = new QStandardItem("America");
    QStandardItem *mexicoItem =  new QStandardItem("Canada");
    QStandardItem *usaItem =     new QStandardItem("USA");
    QStandardItem *bostonItem =  new QStandardItem("Boston");
    QStandardItem *europeItem =  new QStandardItem("Europe");
    QStandardItem *italyItem =   new QStandardItem("Italy");
    QStandardItem *romeItem =    new QStandardItem("Rome");
    QStandardItem *veronaItem =  new QStandardItem("Verona");

    //building up the hierarchy
    rootNode->appendRow(americaItem);
    rootNode->appendRow(europeItem);
    americaItem->appendRow(mexicoItem);
    americaItem->appendRow(usaItem);
    usaItem->appendRow(bostonItem);
    europeItem->appendRow(italyItem);
    italyItem->appendRow(romeItem);
    italyItem->appendRow(veronaItem);

    //register the model
    ui->treeViewAvailableModules->setModel(standardModel);
    ui->treeViewAvailableModules->expandAll();

    //FIM-TESTE

    connect(NetworkCommunication::instance(), SIGNAL(currentConfigFromRasp(QByteArray)),
            this, SLOT(newConfigFromRaspArrived(QByteArray)));

    connect(NetworkCommunication::instance(), SIGNAL(newRFIDMontiorAnswer(QString)),
            this, SLOT(newAnswerFromRaspArrived(QString)));

    connect(NetworkCommunication::instance(), SIGNAL(connectionFailed()),
            this, SLOT(raspDisconnected()));
    connect(ui->btSendToRasp, SIGNAL(clicked()),this, SLOT(btSendToRaspClicked()));


    NetworkCommunication::instance()->triggerToGetCurrentConfigFromRasp();
}

RFIDMonitorInteractorWidget::~RFIDMonitorInteractorWidget()
{
    delete ui;
}

void RFIDMonitorInteractorWidget::loadConfigurationFromJson(const QByteArray &byteArray)
{
    clearForm();
    //fills the interface with json data.
}

void RFIDMonitorInteractorWidget::clearForm()
{
    //clear all fields
}

void RFIDMonitorInteractorWidget::disableAllForm()
{
    //lock the form waiting for some answer or procedure ends.
}

void RFIDMonitorInteractorWidget::sendCurrentConfiguration()
{
    //mount a json from the current configuration on form and send to rasp.
    //NetworkCommunication::instance()->sendNewConfigToRasp(json);
    //persistLocalyCurrentConfig(mac, json);
}

void RFIDMonitorInteractorWidget::persistLocalyCurrentConfig(const QString &mac, const QByteArray &json)
{
    //persist on sqlite the current configuration sent to have a history of it.
}

void RFIDMonitorInteractorWidget::raspDisconnected()
{
    disableAllForm();
    //tell to the user rasp has disconnected.
}

void RFIDMonitorInteractorWidget::newConfigFromRaspArrived(QByteArray json)
{
    loadConfigurationFromJson(json);
}

void RFIDMonitorInteractorWidget::newAnswerFromRaspArrived(QString answer)
{
    //show somewhere the answer from rasp. Maybe QmessageBox or a status label
}

void RFIDMonitorInteractorWidget::btSendToRaspClicked()
{

}
