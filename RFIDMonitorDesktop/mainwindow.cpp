#include <QDebug>
#include <QAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionTerminal, SIGNAL(triggered(bool)), this, SLOT(btConfigReaderClicked(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btConfigReaderClicked(bool checked)
{
    terminalWindow = new TerminalWindow(this);
    terminalWindow->show();

}
