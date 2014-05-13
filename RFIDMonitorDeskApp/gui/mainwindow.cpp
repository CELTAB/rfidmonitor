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

    this->setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);


    SystemMessagesWidget::instance()->writeMessage(
                tr("System started."),
                SystemMessagesWidget::KDebug,
                SystemMessagesWidget::KOnlyLogfile
                );

    ui->btCloseConnection->setIcon(QIcon(":/icons/icon-cancel"));

    // Indexes for tabs.
    tabSetUpConnectionIndex = 0;
    tabConnectionIndex = 1;

    // Connection Configuration Windows
    m_networkConnConfigWidget = 0;
    m_serialConnConfigWidget = 0;

    // Manipulator Windows
    m_readerManipulatorWidget = 0;
    m_rfidmonitorManipulatorWidget = 0;    

    connect(ui->rbSerial, SIGNAL(clicked()), this, SLOT(rbSerialClicked()));
    connect(ui->rbNetwork, SIGNAL(clicked()), this, SLOT(rbNetworkClicked()));
    connect(ui->btCloseConnection, SIGNAL(clicked()), this, SLOT(btCloseConnectionClicked()));

    connect(ui->rbNetwork, SIGNAL(clicked(bool)), ui->gbConnConfig, SLOT(setVisible(bool)));

    ui->rbNetwork->setChecked(true);
    rbNetworkClicked();

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

    SystemMessagesWidget::instance()->writeMessage(
                tr("System closed normaly."),
                SystemMessagesWidget::KDebug,
                SystemMessagesWidget::KOnlyLogfile
                );
}   

void MainWindow::serialCommunicationReady()
{
    // A serial communication always is to interact with Reader.
    prepareReaderManipulatorWidget(Settings::KSerial);
}

void MainWindow::networkCommunicationReady()
{
    if(m_networkConnConfigWidget->isReaderManipulatorSelected()){
        prepareReaderManipulatorWidget(Settings::KNetwork);
    }else{
        prepareRFIDMonitorManipulatorWidget();
    }
}

void MainWindow::prepareReaderManipulatorWidget(const Settings::ConnectionType &connectionType)
{
    if(! m_readerManipulatorWidget)
        m_readerManipulatorWidget = new ReaderManipulatorWidget(connectionType,this);

    ui->tabConnection->layout()->addWidget(m_readerManipulatorWidget);
    m_readerManipulatorWidget->show();

    ui->tabMain->setTabEnabled(tabConnectionIndex, true);
    ui->tabMain->setTabEnabled(tabSetUpConnectionIndex, false);
}

void MainWindow::prepareRFIDMonitorManipulatorWidget()
{
    if(! m_rfidmonitorManipulatorWidget)
        m_rfidmonitorManipulatorWidget = new RFIDMonitorManipulatorWidget(this);

    ui->tabConnection->layout()->addWidget(m_rfidmonitorManipulatorWidget);
    m_rfidmonitorManipulatorWidget->show();

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

    if(m_rfidmonitorManipulatorWidget){
        // If the m_rfidmonitorManipulatorWidget is open, hide it, close its connection, mark to delete and clean the pointer.
        m_rfidmonitorManipulatorWidget->close();
        m_rfidmonitorManipulatorWidget->closeConnection();
        m_rfidmonitorManipulatorWidget->deleteLater();
        m_rfidmonitorManipulatorWidget = 0;
    } else if(m_readerManipulatorWidget){
        // If the m_readerManipulatorWidget is open, hide it, close its connection, mark to delete and clean the pointer.
        m_readerManipulatorWidget->close();
        m_readerManipulatorWidget->closeConnection();
        m_readerManipulatorWidget->deleteLater();
        m_readerManipulatorWidget = 0;
    }
}
