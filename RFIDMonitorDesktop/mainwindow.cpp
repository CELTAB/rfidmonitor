#include <QDebug>
#include <QAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionTerminal, SIGNAL(triggered(bool)), this, SLOT(actionOpenTerminalClicked(bool)));
    connect(ui->actionRaspberry_Pi, SIGNAL(triggered(bool)), this, SLOT(actionConfigureRaspClicked(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actionOpenTerminalClicked(bool)
{
    terminalWindow = new TerminalWindow(this);
    terminalWindow->show();

}

void MainWindow::actionConfigureRaspClicked(bool)
{
    configRaspWindow = new ConfigRaspWindow(this);
    configRaspWindow->show();
}
