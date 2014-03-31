#include <QFileDialog>
#include <QDir>

#include "readerinteractorwidget.h"
#include "ui_readerinteractorwidget.h"
#include "systemmessageswidget.h"

ReaderInteractorWidget::ReaderInteractorWidget(const Settings::ConnectionType type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderInteractorWidget),
    m_serialCommunication(0),
    m_networkCommunication(0),
    m_connectionType(type)
{
    ui->setupUi(this);
    m_logFile = new QFile(this);

    ui->btLogTo->setEnabled(true);
    ui->cbLogType->setEnabled(true);
    ui->btSendCommand->setEnabled(false);
    ui->leCommand->setEnabled(false);

    ui->cbLogType->addItem("Append", QIODevice::Append);
    ui->cbLogType->addItem("Overwrite", QIODevice::WriteOnly);

    connect(ui->btSendCommand, SIGNAL(clicked()), this, SLOT(btSendCommandClicked()));
    connect(ui->leCommand, SIGNAL(returnPressed()), this, SLOT(btSendCommandClicked()));
    connect(ui->btStartPauseReading, SIGNAL(clicked(bool)), this, SLOT(btStartPauseReadingClicked(bool)));
    connect(ui->btLogTo, SIGNAL(clicked()), this, SLOT(btLogToClicked()));
    connect(ui->btClearOutput, SIGNAL(clicked()), this, SLOT(btClearOutputClicked()));
}

ReaderInteractorWidget::~ReaderInteractorWidget()
{
    if(m_logFile->isOpen())
        m_logFile->close();

    delete ui;
}

void ReaderInteractorWidget::closeConnection()
{
    if(m_connectionType == Settings::KSerial){
        SerialCommunication::instance()->disconnectFromDevice();
    }
    else if(m_connectionType == Settings::KNetwork){
        NetworkCommunication::instance()->closeTCPConnection();
    }
}

void ReaderInteractorWidget::sendCommand(const QString &command)
{
    if( ! command.isEmpty()){
        if(m_connectionType == Settings::KSerial){
            SerialCommunication::instance()->sendCommand(command);
        }
        else if(m_connectionType == Settings::KNetwork){
            NetworkCommunication::instance()->sendNewCommandToReader(command);
        }
    }
}

void ReaderInteractorWidget::newAnswerFromSerialComm(const QString answer)
{
    ui->teOutput->append(answer);
}

void ReaderInteractorWidget::newAnswerFromNetworkComm(const QString answer)
{
    ui->teOutput->append(answer);
    if(m_logFile->isOpen()){
        QTextStream logStream(m_logFile);
        logStream << answer << QString("\r");
        logStream.flush();
    }
}

void ReaderInteractorWidget::btSendCommandClicked()
{
    sendCommand(ui->leCommand->text());
    ui->leCommand->clear();
}

void ReaderInteractorWidget::btClearOutputClicked()
{
    ui->teOutput->clear();
}

void ReaderInteractorWidget::btLogToClicked()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Select log file"), QDir::homePath()));
    if(!fileName.isEmpty()){
        ui->leLogFile->setText(fileName);
    }else{
        ui->leLogFile->setText("");
    }
}

void ReaderInteractorWidget::btStartPauseReadingClicked(const bool checked)
{
    if(checked){
        //start reading

        if(m_logFile->isOpen())
            m_logFile->close();

        if( ! ui->leLogFile->text().isEmpty()){
            m_logFile->setFileName(ui->leLogFile->text());
            if(m_logFile->open((QIODevice::OpenModeFlag)ui->cbLogType->currentData().toInt()) && m_logFile->isWritable()){
                SystemMessagesWidget::instance()->writeMessage(tr("Log file ok."));
            }else{
                if(m_logFile->isOpen())
                    m_logFile->close();
                SystemMessagesWidget::instance()->writeMessage(tr("Failed to use the log file."));
            }
        }


        ui->btStartPauseReading->setText("Pause");
        ui->btLogTo->setEnabled(false);
        ui->cbLogType->setEnabled(false);
        ui->btSendCommand->setEnabled(true);
        ui->leCommand->setEnabled(true);

        if(m_connectionType == Settings::KSerial){
            connect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
        }
        else if(m_connectionType == Settings::KNetwork){
            connect(NetworkCommunication::instance(), SIGNAL(newReaderAnswer(QString)), this, SLOT(newAnswerFromNetworkComm(QString)));
        }
    }else{
        //pause reading

        if(m_logFile->isOpen())
            m_logFile->close();

        ui->btStartPauseReading->setText("Start");
        ui->btLogTo->setEnabled(true);
        ui->cbLogType->setEnabled(true);
        ui->btSendCommand->setEnabled(false);
        ui->leCommand->setEnabled(false);

        if(m_connectionType == Settings::KSerial){
            disconnect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
        }
        else if(m_connectionType == Settings::KNetwork){
            disconnect(NetworkCommunication::instance(), SIGNAL(newReaderAnswer(QString)), this, SLOT(newAnswerFromNetworkComm(QString)));
        }
    }

}
