#include <QDebug>
#include <QString>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>

#include "terminalwindow.h"
#include "ui_terminalwindow.h"

TerminalWindow::TerminalWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TerminalWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::Window);
    this->setWindowModality(Qt::ApplicationModal);
    m_terminalCommunicator = new TerminalCommunicator(this);
    m_terminalCommunicator->setTextEdit(ui->teOutput);

    connect(ui->btRefreshDeviceList, &QPushButton::clicked, this, &TerminalWindow::btRefreshDeviceClicked);
    connect(ui->btConnect, &QPushButton::clicked, this, &TerminalWindow::btConnectToDeviceClicked);
    connect(ui->btDisconnect, &QPushButton::clicked, this, &TerminalWindow::btDisconnectFromDeviceClicked);
    connect(ui->leCommand, &QLineEdit::returnPressed, this, &TerminalWindow::leCommandLineSent);
    connect(ui->btSearchLogFile, &QPushButton::clicked, this, &TerminalWindow::btSearchLogFileClicked);
    connect(ui->btClearOutput, &QPushButton::clicked, this, &TerminalWindow::btClearOutput);

    refreshDeviceList();
    populateFields();

}

TerminalWindow::~TerminalWindow()
{
    delete ui;
}

void TerminalWindow::refreshDeviceList()
{
    ui->cbDeviceList->clear();
    ui->cbDeviceList->addItems(m_terminalCommunicator->availablePorts());
    ui->teOutput->append(tr("Device List refreshed."));
}

void TerminalWindow::connectToDevice()
{
    //set log file.
    m_terminalCommunicator->setLogFile(ui->leLogFilePath->text(), ui->cbLogType->currentData().toInt());

    //try to connect.
    if(m_terminalCommunicator->connectToDevice(ui->cbDeviceList->currentText())){
        ui->btDisconnect->setEnabled(true);

        ui->cbDeviceList->setEnabled(false);
        ui->btConnect->setEnabled(false);
        ui->btRefreshDeviceList->setEnabled(false);
        ui->cbBaudRate->setEnabled(false);
        ui->cbDataBits->setEnabled(false);
        ui->cbLogType->setEnabled(false);
        ui->cbOpenType->setEnabled(false);
        ui->cbParity->setEnabled(false);
        ui->cbStopBits->setEnabled(false);
        ui->btSearchLogFile->setEnabled(false);

        if( (ui->cbOpenType->currentText() == tr("Read/Write")) ||  (ui->cbOpenType->currentText() == tr("Write"))){
            ui->leCommand->setReadOnly(false);
            ui->leCommand->setFocus();
        }

    }

}

void TerminalWindow::disconnectFromDevice()
{
    if(m_terminalCommunicator->disconnectFromDevice()){
        ui->btDisconnect->setEnabled(false);
        ui->cbDeviceList->setEnabled(true);
        ui->btConnect->setEnabled(true);
        ui->btRefreshDeviceList->setEnabled(true);
        ui->leCommand->setReadOnly(true);
        ui->cbBaudRate->setEnabled(true);
        ui->cbDataBits->setEnabled(true);
        ui->cbLogType->setEnabled(true);
        ui->cbOpenType->setEnabled(true);
        ui->cbParity->setEnabled(true);
        ui->cbStopBits->setEnabled(true);
        ui->btSearchLogFile->setEnabled(true);
    }
}

void TerminalWindow::populateFields()
{

    ui->cbBaudRate->addItem("9600",QSerialPort::Baud9600);
    ui->cbBaudRate->addItem("1200",QSerialPort::Baud1200);
    ui->cbBaudRate->addItem("2400",QSerialPort::Baud2400);
    ui->cbBaudRate->addItem("4800",QSerialPort::Baud4800);
    ui->cbBaudRate->addItem("19200",QSerialPort::Baud19200);
    ui->cbBaudRate->addItem("38400",QSerialPort::Baud38400);
    ui->cbBaudRate->addItem("57600",QSerialPort::Baud57600);
    ui->cbBaudRate->addItem("115200",QSerialPort::Baud115200);

    ui->cbDataBits->addItem("8",QSerialPort::Data8);
    ui->cbDataBits->addItem("6",QSerialPort::Data6);
    ui->cbDataBits->addItem("7",QSerialPort::Data7);
    ui->cbDataBits->addItem("5",QSerialPort::Data5);

    ui->cbStopBits->addItem("1", QSerialPort::OneStop);
    ui->cbStopBits->addItem("2", QSerialPort::TwoStop);

    ui->cbParity->addItem("None", QSerialPort::NoParity);
    ui->cbParity->addItem("Odd", QSerialPort::OddParity);
    ui->cbParity->addItem("Mark", QSerialPort::MarkParity);
    ui->cbParity->addItem("Space", QSerialPort::SpaceParity);
    ui->cbParity->addItem("Even", QSerialPort::EvenParity);

    ui->cbLogType->addItem(tr("Append"), QIODevice::Append);
    ui->cbLogType->addItem(tr("Overwrite"), QIODevice::WriteOnly);
}

void TerminalWindow::searchLogFile()
{
    //The file name is defined here on the text field, but send to TerminalCommunicator class on connectToDevice();.

    QString fileName(QFileDialog::getOpenFileName(this, tr("Select log file"), QDir::homePath()));
    if(!fileName.isEmpty()){
        ui->leLogFilePath->setText(fileName);
    }else{
        ui->leLogFilePath->setText("");
    }
}

void TerminalWindow::clearOutput()
{
    ui->teOutput->clear();
}

void TerminalWindow::btRefreshDeviceClicked()
{
    refreshDeviceList();
}

void TerminalWindow::btConnectToDeviceClicked()
{
    connectToDevice();
}

void TerminalWindow::btDisconnectFromDeviceClicked()
{
    disconnectFromDevice();
}

void TerminalWindow::btSearchLogFileClicked()
{
    searchLogFile();
}

void TerminalWindow::btClearOutput()
{
    clearOutput();
}

void TerminalWindow::leCommandLineSent()
{
    QString command(ui->leCommand->text());
    if( ! command.isEmpty()){
        m_terminalCommunicator->sendCommand(command);
        ui->leCommand->clear();
    }
}
