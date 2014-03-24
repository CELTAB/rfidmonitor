#include "rfidmonitorinteractorwidget.h"
#include "ui_rfidmonitorinteractorwidget.h"
#include "communication/networkcommunication.h"

RFIDMonitorInteractorWidget::RFIDMonitorInteractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RFIDMonitorInteractorWidget)
{
    ui->setupUi(this);

    connect(NetworkCommunication::instance(), SIGNAL(currentConfigFromRasp(QByteArray)),
            this, SLOT(newConfigFromRaspArrived()));

    connect(NetworkCommunication::instance(), SIGNAL(newRFIDMontiorAnswer(QString)),
            this, SLOT(newAnswerFromRaspArrived(QString)));

    connect(NetworkCommunication::instance(), SIGNAL(raspDisconnected()),
            this, SLOT(raspDisconnected()));


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
