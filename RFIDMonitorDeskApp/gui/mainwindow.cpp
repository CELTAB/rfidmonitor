#include <QFrame>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "systemmessageswidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_networkConnConfigWidget = 0;
    m_serialConnConfigWidget = 0;

    m_readerInteractorWidget = 0;
    m_rfidmonitorInteractorWidget = 0;

    connect(ui->rbSerial, SIGNAL(clicked()), this, SLOT(rbSerialClicked()));
    connect(ui->rbNetwork, SIGNAL(clicked()), this, SLOT(rbNetworkClicked()));

    //Lock Work
    ui->toolBox->setItemEnabled(1, false);

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
    prepareReaderInteractorWidget(Settings::KSerial);
}

void MainWindow::networkCommunicationReady(Settings::InteractionType interactionType)
{
    if(interactionType == Settings::KReader)
        prepareReaderInteractorWidget(Settings::KNetwork);
    else if(interactionType == Settings::KRFIDMonitor)
        prepareRFIDMonitorInteractorWidget();
}

void MainWindow::prepareReaderInteractorWidget(Settings::ConnectionType connectionType)
{
    if(! m_readerInteractorWidget)
        m_readerInteractorWidget = new ReaderInteractorWidget(connectionType,this);

    ui->frWork->layout()->addWidget(m_readerInteractorWidget);
    m_readerInteractorWidget->show();

    //Unlock Work
    ui->toolBox->setItemEnabled(1, true);
    //Lock ConnectionConfiguration
    ui->toolBox->setItemEnabled(0, false);

    //Set current Connection
    ui->toolBox->setItemText(0, tr("Set up the Connection - Connected to: Serial"));


}

void MainWindow::prepareRFIDMonitorInteractorWidget()
{
    if(! m_rfidmonitorInteractorWidget)
        m_rfidmonitorInteractorWidget = new RFIDMonitorInteractorWidget(this);

    ui->frWork->layout()->addWidget(m_rfidmonitorInteractorWidget);
    m_rfidmonitorInteractorWidget->show();

    //Unlock Work
    ui->toolBox->setItemEnabled(1, true);
    //Lock ConnectionConfiguration
    ui->toolBox->setItemEnabled(0, false);

    //Set current Connection
    ui->toolBox->setItemText(0, tr("Set up the Connection - Connected to: Network"));
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
        connect(m_networkConnConfigWidget,
                SIGNAL(networkCommunicationReady(Settings::InteractionType)),
                this,
                SLOT(networkCommunicationReady(Settings::InteractionType)));
    }
    m_networkConnConfigWidget->show();
}
