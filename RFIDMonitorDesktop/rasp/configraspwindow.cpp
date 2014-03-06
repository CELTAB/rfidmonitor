#include <QDebug>

#include "configraspwindow.h"
#include "ui_configraspwindow.h"

ConfigRaspWindow::ConfigRaspWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigRaspWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::Window);
    this->setWindowModality(Qt::ApplicationModal);

    tabRaspberryConfigurarion = new TabRaspberryConfiguration(this);
    ui->tabWidget->addTab(tabRaspberryConfigurarion, "Raspberry Pi Configuration");

    tabRaspberryConnection = new TabRaspberryConnection(this);
    ui->tabWidget->addTab(tabRaspberryConnection, "Connect to Raspberry Pi");

    tabRaspberrySendData = new TabRaspberrySendData(this);
    ui->tabWidget->addTab(tabRaspberrySendData, "Send Configuration to Raspberry");
}

ConfigRaspWindow::~ConfigRaspWindow()
{
    delete ui;
}
