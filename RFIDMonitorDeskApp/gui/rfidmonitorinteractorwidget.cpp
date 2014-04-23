#include <QStandardItem>
#include <QStandardItemModel>
#include <QDebug>
#include <QJsonArray>

#include "rfidmonitorinteractorwidget.h"
#include "ui_rfidmonitorinteractorwidget.h"
#include "communication/networkcommunication.h"
#include "systemmessageswidget.h"

RFIDMonitorInteractorWidget::RFIDMonitorInteractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RFIDMonitorInteractorWidget),
    m_raspSettings(0)
{
    ui->setupUi(this);
    m_treeViewModel = new QStandardItemModel(this);
    ui->treeViewAvailableModules->setModel(m_treeViewModel);
    ui->treeViewAvailableModules->header()->hide();
    //read-only
    ui->treeViewAvailableModules->setEditTriggers(QAbstractItemView::NoEditTriggers);

    changeFormState(KEmpty);
    ui->leMac->setReadOnly(true);

    connect(NetworkCommunication::instance(), SIGNAL(connectionFailed()),
            this, SLOT(raspDisconnected()));
    connect(NetworkCommunication::instance(), SIGNAL(currentConfigFromRasp(QJsonObject)),
            this, SLOT(newConfigFromRaspArrived(QJsonObject)));
    connect(NetworkCommunication::instance(), SIGNAL(currentConfigStatusFromRasp(QJsonObject)),
            this, SLOT(newConfigStatusFromRasp(QJsonObject)));
    connect(ui->btRetrieveFromRasp, SIGNAL(clicked()),
            this, SLOT(btRetrieveConfigFromRaspClicked()));
    connect(ui->btSendToRasp, SIGNAL(clicked()),
            this, SLOT(btSendToRaspClicked()));
    connect(ui->btEdit, SIGNAL(clicked()),
            this, SLOT(btEditClicked()));
    connect(ui->btCancel, SIGNAL(clicked()),
            this, SLOT(btCancelClicked()));
}

RFIDMonitorInteractorWidget::~RFIDMonitorInteractorWidget()
{
    delete ui;

    if(m_raspSettings)
        delete m_raspSettings;
}

void RFIDMonitorInteractorWidget::closeConnection()
{
    changeFormState(KBlocked);
    NetworkCommunication::instance()->closeTCPConnection();
}

void RFIDMonitorInteractorWidget::loadConfigurationFromJson(const QJsonObject &obj)
{
    if(m_raspSettings){
        delete m_raspSettings;
        m_raspSettings = 0;
    }

    m_raspSettings = new json::RFIDMonitorSettings;
    m_raspSettings->read(obj);

    clearForm();

    //fills the interface with json data.

    //GENERAL
    ui->leName->setText(obj.value("name").toString());
    ui->leID->setText(QString::number(obj.value("id").toInt()));
    ui->leDevice->setText(obj.value("device").toString());

    //MODULES


    QStandardItem *rootNode = m_treeViewModel->invisibleRootItem();

    QJsonArray modulesArray = obj.value("modules").toArray();
    for(int i = 0; i < modulesArray.size(); i++){
        QJsonObject moduleObj = modulesArray.at(i).toObject();

        //MODULE
        QStandardItem *moduleNameItem = new QStandardItem(tr("Module: %1").arg(moduleObj.value("modulename").toString()));
        rootNode->appendRow(moduleNameItem);

        //MODULE > VERSION
        QStandardItem *moduleVersionItem = new QStandardItem(tr("Version: %1").arg(QString::number(moduleObj.value("version").toDouble())));
        moduleNameItem->appendRow(moduleVersionItem);

        //MODULE > SERVICES
        QStandardItem *moduleServicesItem = new QStandardItem(tr("Services:"));
        moduleNameItem->appendRow(moduleServicesItem);

        QJsonArray servicesArray = moduleObj.value("services").toArray();
        for (int j=0; j < servicesArray.size(); j++){
            //MODULE > SERVICES > SERVICE
            QJsonObject serviceObj = servicesArray.at(j).toObject();

            Settings::ServiceType serviceCode = (Settings::ServiceType) serviceObj.value("servicetype").toInt();
            QString serviceName = serviceObj.value("servicename").toString();

            QStandardItem *serviceItem= new QStandardItem(
                        tr("Name: ") +
                        serviceName +
                        " | " +
                        tr("Type: ") +
                        Settings::instance()->stringifyServiceType(serviceCode)
                        );
            moduleServicesItem->appendRow(serviceItem);

            switch (serviceCode) {
            case Settings::ServiceType::KReaderService:
                ui->cbDefaultReaderService->addItem(serviceName);
                break;
            case Settings::ServiceType::KPersister:
                ui->cbDefaultPersisterService->addItem(serviceName);
                break;
            case Settings::ServiceType::KExporter:
                ui->cbDefaultExporterService->addItem(serviceName);
                break;
            case Settings::ServiceType::KSynchronizer:
                ui->cbDefaultSyncronizerService->addItem(serviceName);
                break;
            case Settings::ServiceType::KCommunicator:
                ui->cbDefaultCommunicatorService->addItem(serviceName);
                break;
            case Settings::ServiceType::KPackager:
                ui->cbDefaultPackagerService->addItem(serviceName);
                break;
            default:
                qDebug() << "Unknown Service to populate in combobox.";
            }

        }

        //DEFAULT SERVICES
        QJsonObject defaultServicesObj = obj.value("defaultservices").toObject();

        ui->cbDefaultReaderService->setCurrentText(defaultServicesObj.value("reader").toString());
        ui->cbDefaultPersisterService->setCurrentText(defaultServicesObj.value("persister").toString());
        ui->cbDefaultExporterService->setCurrentText(defaultServicesObj.value("exporter").toString());
        ui->cbDefaultSyncronizerService->setCurrentText(defaultServicesObj.value("synchronizer").toString());
        ui->cbDefaultCommunicatorService->setCurrentText(defaultServicesObj.value("communicator").toString());
        ui->cbDefaultPackagerService->setCurrentText(defaultServicesObj.value("packager").toString());

    }

    //NETWORK
    ui->leMac->setText(obj.value("macaddress").toString());

    QJsonObject wirelessNetwork = obj.value("network").toObject();
    ui->leWirelessSSID->setText(wirelessNetwork.value("essid").toString());
    ui->leWirelessPassword->setText(wirelessNetwork.value("password").toString());

    ui->leServerAddress->setText(obj.value("serveraddress").toString());
    ui->leServerPort->setText(QString::number(obj.value("port").toInt()));

}

void RFIDMonitorInteractorWidget::clearForm()
{
    //clear all fields
    ui->leName->clear();
    ui->leID->clear();
    ui->leDevice->clear();
    ui->cbDefaultCommunicatorService->clear();
    ui->cbDefaultExporterService->clear();
    ui->cbDefaultPackagerService->clear();
    ui->cbDefaultPersisterService->clear();
    ui->cbDefaultReaderService->clear();
    ui->cbDefaultSyncronizerService->clear();
    ui->leMac->clear();
    ui->leWirelessPassword->clear();
    ui->leWirelessSSID->clear();
    ui->leServerAddress->clear();
    ui->leServerPort->clear();
}

void RFIDMonitorInteractorWidget::formEnabled(bool enabled)
{
    //lock the form waiting for some answer or procedure ends.
    ui->btSendToRasp->setEnabled(enabled);
    ui->btEdit->setEnabled(enabled);
    ui->btRetrieveFromRasp->setEnabled(enabled);
    ui->btCancel->setEnabled(enabled);

    ui->leName->setEnabled(enabled);
    ui->leID->setEnabled(enabled);
    ui->leMac->setEnabled(enabled);
    ui->leDevice->setEnabled(enabled);
    ui->cbDefaultCommunicatorService->setEnabled(enabled);
    ui->cbDefaultExporterService->setEnabled(enabled);
    ui->cbDefaultPackagerService->setEnabled(enabled);
    ui->cbDefaultPersisterService->setEnabled(enabled);
    ui->cbDefaultReaderService->setEnabled(enabled);
    ui->cbDefaultSyncronizerService->setEnabled(enabled);
    ui->leWirelessPassword->setEnabled(enabled);
    ui->leWirelessSSID->setEnabled(enabled);
    ui->leServerAddress->setEnabled(enabled);
    ui->leServerPort->setEnabled(enabled);
}

void RFIDMonitorInteractorWidget::sendCurrentConfiguration()
{
    //GENERAL

    m_raspSettings->setName(ui->leName->text());
    m_raspSettings->setId(ui->leID->text().toInt());
    m_raspSettings->setDevice(ui->leDevice->text());


    //DEFAULT SERVICES

    json::DefaultServices defaultServices;

    defaultServices.setReader(ui->cbDefaultReaderService->currentText());
    defaultServices.setPersister(ui->cbDefaultPersisterService->currentText());
    defaultServices.setExporter(ui->cbDefaultExporterService->currentText());
    defaultServices.setSynchronizer(ui->cbDefaultSyncronizerService->currentText());
    defaultServices.setCommunicator(ui->cbDefaultCommunicatorService->currentText());
    defaultServices.setPackager(ui->cbDefaultPackagerService->currentText());

    m_raspSettings->setDefaultServices(defaultServices);


    //NETWORK

    m_raspSettings->setMacAddress(ui->leMac->text());

    json::Network network;
    network.setEssid(ui->leWirelessSSID->text());
    network.setPassword(ui->leWirelessPassword->text());
    m_raspSettings->setNetworkConfiguration(network);

    m_raspSettings->setServerAddress(ui->leServerAddress->text());
    m_raspSettings->setServerPort(ui->leServerPort->text().toInt());

    QJsonObject objToSend;
    m_raspSettings->write(objToSend);
    NetworkCommunication::instance()->sendNewConfigToRasp(objToSend);


    qDebug() << " need to persistLocalyCurrentConfig(mac, json)";
}

void RFIDMonitorInteractorWidget::persistLocalyCurrentConfig(const QString &mac, const QByteArray &json)
{
    //persist on sqlite the current configuration sent to have a history of it.
}

void RFIDMonitorInteractorWidget::changeFormState(RFIDMonitorInteractorWidget::FormState state)
{

    switch (state) {
    case KBlocked:
        formEnabled(false);
        break;
    case KEmpty: {
        formEnabled(false);

        int rowCount = ui->treeViewAvailableModules->model()->rowCount();
        if(rowCount > 0)
            ui->treeViewAvailableModules->model()->removeRows(0, rowCount);

        ui->btRetrieveFromRasp->setEnabled(true);
        clearForm();
        } break;
    case KEditing:
        formEnabled(true);
        ui->btRetrieveFromRasp->setEnabled(false);
        ui->btEdit->setEnabled(false);
        break;
    case KWithObject:
        formEnabled(false);
        ui->btRetrieveFromRasp->setEnabled(true);
        ui->btEdit->setEnabled(true);
        break;
    default:
        break;
    }
}

void RFIDMonitorInteractorWidget::raspDisconnected()
{
    changeFormState(KBlocked);
}

void RFIDMonitorInteractorWidget::newConfigFromRaspArrived(const QJsonObject json)
{
    loadConfigurationFromJson(json);
    changeFormState(KWithObject);
}

void RFIDMonitorInteractorWidget::btSendToRaspClicked()
{
    sendCurrentConfiguration();
    changeFormState(KBlocked);
}

void RFIDMonitorInteractorWidget::btRetrieveConfigFromRaspClicked()
{
    NetworkCommunication::instance()->getConfigFromRasp();
}

void RFIDMonitorInteractorWidget::newConfigStatusFromRasp(QJsonObject obj)
{
    if(obj.value("success").toBool()){
        SystemMessagesWidget::instance()->writeMessage(tr("Configuration successfuly delivered."),
                                                       SystemMessagesWidget::KInfo,
                                                       SystemMessagesWidget::KDialogAndLog);
        changeFormState(KWithObject);
    }
    else{
        SystemMessagesWidget::instance()->writeMessage(tr("Failed to delivery the new configuration."),
                                                       SystemMessagesWidget::KError,
                                                       SystemMessagesWidget::KDialogAndLog);
        changeFormState(KEditing);
    }
}

void RFIDMonitorInteractorWidget::btEditClicked()
{
    changeFormState(KEditing);

}

void RFIDMonitorInteractorWidget::btCancelClicked()
{
    changeFormState(KEmpty);
}
