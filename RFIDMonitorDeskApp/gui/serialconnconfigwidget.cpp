#include <QFileDialog>

#include "serialconnconfigwidget.h"
#include "ui_serialconnconfigwidget.h"

SerialConnConfigWidget::SerialConnConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialConnConfigWidget)
{
    ui->setupUi(this);

    m_serialCommunication = SerialCommunication::instance();


    connect(ui->btRefreshDeviceList, &QPushButton::clicked, this, &SerialConnConfigWidget::btRefreshDeviceClicked);
    connect(ui->btConnect, &QPushButton::clicked, this, &SerialConnConfigWidget::btConnectToDeviceClicked);
//    connect(ui->leCommand, &QLineEdit::returnPressed, this, &SerialConnConfigWidget::leCommandLineSent);
    connect(ui->btSearchLogFile, &QPushButton::clicked, this, &SerialConnConfigWidget::btSearchLogFileClicked);
//    connect(ui->btClearOutput, &QPushButton::clicked, this, &SerialConnConfigWidget::btClearOutput);

    refreshDeviceList();
    populateFields();
}

SerialConnConfigWidget::~SerialConnConfigWidget()
{
    delete ui;
}

void SerialConnConfigWidget::refreshDeviceList()
{
    ui->cbDeviceList->clear();
    ui->cbDeviceList->addItems(m_serialCommunication->availablePorts());
}

void SerialConnConfigWidget::connectToDevice()
{
    //set log file.
    m_serialCommunication->setLogFile(ui->leLogFilePath->text(), ui->cbLogType->currentData().toInt());

    //try to connect.
    if(m_serialCommunication->connectToDevice(ui->cbDeviceList->currentText())){

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

        //Notify MainWindow the connection is done.
        emit serialCommunicationReady();

//        if( (ui->cbOpenType->currentText() == tr("Read/Write")) ||  (ui->cbOpenType->currentText() == tr("Write"))){
//            ui->leCommand->setReadOnly(false);
//            ui->leCommand->setFocus();
//        }

    }
}

void SerialConnConfigWidget::disconnectFromDevice()
{
    if(m_serialCommunication->disconnectFromDevice()){
        ui->cbDeviceList->setEnabled(true);
        ui->btConnect->setEnabled(true);
        ui->btRefreshDeviceList->setEnabled(true);
//        ui->leCommand->setReadOnly(true);
        ui->cbBaudRate->setEnabled(true);
        ui->cbDataBits->setEnabled(true);
        ui->cbLogType->setEnabled(true);
        ui->cbOpenType->setEnabled(true);
        ui->cbParity->setEnabled(true);
        ui->cbStopBits->setEnabled(true);
        ui->btSearchLogFile->setEnabled(true);
    }
}

void SerialConnConfigWidget::populateFields()
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

void SerialConnConfigWidget::searchLogFile()
{
    //The file name is defined here on the text field, but send to TerminalCommunicator class on connectToDevice();.

    QString fileName(QFileDialog::getOpenFileName(this, tr("Select log file"), QDir::homePath()));
    if(!fileName.isEmpty()){
        ui->leLogFilePath->setText(fileName);
    }else{
        ui->leLogFilePath->setText("");
    }
}

void SerialConnConfigWidget::clearOutput()
{
//    ui->teOutput->clear();
}

void SerialConnConfigWidget::btRefreshDeviceClicked()
{
    refreshDeviceList();
}

void SerialConnConfigWidget::btConnectToDeviceClicked()
{
    connectToDevice();
}

void SerialConnConfigWidget::btDisconnectFromDeviceClicked()
{
    disconnectFromDevice();
}

void SerialConnConfigWidget::btSearchLogFileClicked()
{
    searchLogFile();
}

void SerialConnConfigWidget::btClearOutput()
{
    clearOutput();
}

void SerialConnConfigWidget::leCommandLineSent()
{
//    QString command(ui->leCommand->text());
//    if( ! command.isEmpty()){
//        m_serialCommunication->sendCommand(command);
//        ui->leCommand->clear();
//    }
}
