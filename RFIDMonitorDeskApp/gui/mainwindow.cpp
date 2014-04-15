#include <QFrame>
#include <QVBoxLayout>
#include <QApplication>
#include <QTranslator>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "systemmessageswidget.h"
#include "communication/networkcommunication.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Indexes for tabs.
    tabSetUpConnectionIndex = 0;
    tabConnectionIndex = 1;

    // Connection Configuration Windows
    m_networkConnConfigWidget = 0;
    m_serialConnConfigWidget = 0;

    // Interactor Windows
    m_readerInteractorWidget = 0;
    m_rfidmonitorInteractorWidget = 0;

    connect(ui->rbSerial, SIGNAL(clicked()), this, SLOT(rbSerialClicked()));
    connect(ui->rbNetwork, SIGNAL(clicked()), this, SLOT(rbNetworkClicked()));
    connect(ui->btCloseConnection, SIGNAL(clicked()), this, SLOT(btCloseConnectionClicked()));

    // Set the SetUpConnection tab as enabled and the Connection tab not enabled.
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, true);
    ui->tabMain->setTabEnabled(tabConnectionIndex, false);

    // Creates a layout for the SystemMessagesWidget
    QVBoxLayout *vbLayout = new QVBoxLayout(ui->frMessages);
    ui->frMessages->setLayout(vbLayout);
    vbLayout->addWidget(SystemMessagesWidget::instance());
}

MainWindow::~MainWindow()
{
    delete ui;
}   

void MainWindow::serialCommunicationReady()
{
    // A serial communication always is to interact with Reader.
    prepareReaderInteractorWidget(Settings::KSerial);
}

void MainWindow::networkCommunicationReady()
{
    m_networkConnConfigWidget->stopSearchingRasp();
    SystemMessagesWidget::instance()->writeMessage("Successfuly connect to rasp.");

    if(m_networkConnConfigWidget->isReaderInteractorSelected()){
        prepareReaderInteractorWidget(Settings::KNetwork);
    }else{
        prepareRFIDMonitorInteractorWidget();
    }
}

void MainWindow::prepareReaderInteractorWidget(const Settings::ConnectionType &connectionType)
{
    if(! m_readerInteractorWidget)
        m_readerInteractorWidget = new ReaderInteractorWidget(connectionType,this);

    ui->tabConnection->layout()->addWidget(m_readerInteractorWidget);
    m_readerInteractorWidget->show();

    ui->tabMain->setTabEnabled(tabConnectionIndex, true);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, false);
}

void MainWindow::prepareRFIDMonitorInteractorWidget()
{
    if(! m_rfidmonitorInteractorWidget)
        m_rfidmonitorInteractorWidget = new RFIDMonitorInteractorWidget(this);

    ui->tabConnection->layout()->addWidget(m_rfidmonitorInteractorWidget);
    m_rfidmonitorInteractorWidget->show();

    // Change the enabled tab to Connection tab.
    ui->tabMain->setTabEnabled(tabConnectionIndex, true);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, false);
}

void MainWindow::rbSerialClicked()
{
    // If m_networkConnConfigWidget exists, hide it.
    if(m_networkConnConfigWidget){
        m_networkConnConfigWidget->close();
    }
    // Ensure the m_serialConnConfigWidget is instantied only 1 time (kind of singleton).
    if(! m_serialConnConfigWidget){
        m_serialConnConfigWidget = new SerialConnConfigWidget(this);
        ui->gbConnConfig->layout()->addWidget(m_serialConnConfigWidget);
        // Connect the success serial connection signal to the equivalent slot in this class.
        connect(m_serialConnConfigWidget, SIGNAL(serialCommunicationReady()), this, SLOT(serialCommunicationReady()));
    }
    m_serialConnConfigWidget->show();
}

void MainWindow::rbNetworkClicked()
{
    // If m_serialConnConfigWidget exists, hide it.
    if(m_serialConnConfigWidget){
        m_serialConnConfigWidget->close();
    }
    // Ensure the m_networkConnConfigWidget is instantied only 1 time (kind of singleton).
    if(! m_networkConnConfigWidget){
        m_networkConnConfigWidget = new NetworkConnConfigWidget(this);
        ui->gbConnConfig->layout()->addWidget(m_networkConnConfigWidget);
        // Connect the success network connection signal to the equivalent slot in this class.
        connect(NetworkCommunication::instance(),SIGNAL(connectionEstablished()), this, SLOT(networkCommunicationReady()));
    }
    m_networkConnConfigWidget->show();
}

void MainWindow::btCloseConnectionClicked()
{
    // Change the enabled tab to SetUpConnection tab.
    ui->tabMain->setTabEnabled(tabConnectionIndex, false);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, true);

    if(m_rfidmonitorInteractorWidget){
        // If the m_rfidmonitorInteractorWidget is open, hide it, close its connection, mark to delete and clean the pointer.
        m_rfidmonitorInteractorWidget->close();
        m_rfidmonitorInteractorWidget->closeConnection();
        m_rfidmonitorInteractorWidget->deleteLater();
        m_rfidmonitorInteractorWidget = 0;
    } else if(m_readerInteractorWidget){
        // If the m_readerInteractorWidget is open, hide it, close its connection, mark to delete and clean the pointer.
        m_readerInteractorWidget->close();
        m_readerInteractorWidget->closeConnection();
        m_readerInteractorWidget->deleteLater();
        m_readerInteractorWidget = 0;
    }
}
