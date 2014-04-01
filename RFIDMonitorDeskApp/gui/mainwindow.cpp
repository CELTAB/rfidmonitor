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

    //tab main index for tabs.
    tabSetUpConnectionIndex = 0;
    tabConnectionIndex = 1;

    m_networkConnConfigWidget = 0;
    m_serialConnConfigWidget = 0;

    m_readerInteractorWidget = 0;
    m_rfidmonitorInteractorWidget = 0;

    connect(ui->rbSerial, SIGNAL(clicked()), this, SLOT(rbSerialClicked()));
    connect(ui->rbNetwork, SIGNAL(clicked()), this, SLOT(rbNetworkClicked()));
    connect(ui->btCloseConnection, SIGNAL(clicked()), this, SLOT(btCloseConnectionClicked()));

    ui->tabMain->setTabEnabled(tabConnectionIndex, false);

    QVBoxLayout *vbLayout = new QVBoxLayout(ui->frMessages);
    ui->frMessages->setLayout(vbLayout);
    vbLayout->addWidget(SystemMessagesWidget::instance());
    this->showMaximized();
    //    this->activateWindow();
    //    this->raise();

}

MainWindow::~MainWindow()
{
    delete ui;
}   

void MainWindow::serialCommunicationReady()
{
    prepareReaderInteractorWidget(Settings::KSerial);
}

void MainWindow::networkCommunicationReady()
{
    m_networkConnConfigWidget->btStopRaspSearchClicked();
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

    ui->tabMain->setTabEnabled(tabConnectionIndex, true);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, false);
}

void MainWindow::rbSerialClicked()
{
    if(m_networkConnConfigWidget){
        m_networkConnConfigWidget->close();
    }

    if(! m_serialConnConfigWidget){
        m_serialConnConfigWidget = new SerialConnConfigWidget(this);
        ui->gbConnConfig->layout()->addWidget(m_serialConnConfigWidget);
        connect(m_serialConnConfigWidget, SIGNAL(serialCommunicationReady()), this, SLOT(serialCommunicationReady()));
    }
    m_serialConnConfigWidget->show();
}

void MainWindow::rbNetworkClicked()
{
    if(m_serialConnConfigWidget){
        m_serialConnConfigWidget->close();
    }

    if(! m_networkConnConfigWidget){
        m_networkConnConfigWidget = new NetworkConnConfigWidget(this);
        ui->gbConnConfig->layout()->addWidget(m_networkConnConfigWidget);
        connect(NetworkCommunication::instance(),
                SIGNAL(connectionEstablished()),
                this,
                SLOT(networkCommunicationReady()));
    }
    m_networkConnConfigWidget->show();
}

void MainWindow::btCloseConnectionClicked()
{
    ui->tabMain->setTabEnabled(tabConnectionIndex, false);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, true);

    if(m_rfidmonitorInteractorWidget){
        m_rfidmonitorInteractorWidget->close();
        m_rfidmonitorInteractorWidget->closeConnection();
        m_rfidmonitorInteractorWidget->deleteLater();
        m_rfidmonitorInteractorWidget = 0;
    } else if(m_readerInteractorWidget){
        m_readerInteractorWidget->close();
        m_readerInteractorWidget->closeConnection();
        m_readerInteractorWidget->deleteLater();
        m_readerInteractorWidget = 0;
    }
}
