#include <QFileDialog>
#include <QDir>

#include "readerinteractorwidget.h"
#include "ui_readerinteractorwidget.h"
#include "systemmessageswidget.h"

ReaderInteractorWidget::ReaderInteractorWidget(const Settings::ConnectionType type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderInteractorWidget),
    m_connectionType(type)
{
    ui->setupUi(this);

    // Log file.
    m_logFile = new QFile(this);
    m_useLogFile = false;

    // Define the default window state.
    ui->btLogTo->setEnabled(true);
    ui->cbLogType->setEnabled(true);
    ui->btSendCommand->setEnabled(false);
    ui->leCommand->setEnabled(false);
    ui->cbLogType->addItem("Append", QIODevice::Append);
    ui->cbLogType->addItem("Overwrite", QIODevice::WriteOnly);
    ui->cbInputType->addItem("ASCII", SerialCommunication::KASCII);
    ui->cbInputType->addItem("Number", SerialCommunication::KNumber);

    connect(ui->btSendCommand, SIGNAL(clicked()), this, SLOT(btSendCommandClicked()));
    connect(ui->leCommand, SIGNAL(returnPressed()), this, SLOT(leCommandReturnPressed()));
    connect(ui->btStartPauseReading, SIGNAL(clicked(bool)), this, SLOT(btStartPauseReadingClicked(bool)));
    connect(ui->btLogTo, SIGNAL(clicked()), this, SLOT(btLogToClicked()));
    connect(ui->btClearOutput, SIGNAL(clicked()), this, SLOT(btClearOutputClicked()));

    // instantiate the RI-CTL-MB2B-30 interactor and add it to the main tab.
    m_mb2b30 = new RICTLMB2B30Widget(m_connectionType, this);
    ui->tabWidget->addTab(m_mb2b30, "RI-CTL-MB2B-30");
}

ReaderInteractorWidget::~ReaderInteractorWidget()
{
    //When this interactor is ordered to close, close the log file first.
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

        ui->leCommand->clear();
        writeToOutput("Command sent to device: " + command);

        if(m_connectionType == Settings::KSerial){
            SerialCommunication::instance()->sendCommand(command,
                                                         (SerialCommunication::CommandType) ui->cbInputType->currentData().toInt());
        }
        else if(m_connectionType == Settings::KNetwork){
            NetworkCommunication::instance()->sendNewCommandToReader(command);
        }
    }
}

void ReaderInteractorWidget::writeToOutput(const QString &text)
{
    ui->teOutput->append(text);

    if(m_useLogFile && m_logFile->isOpen()){
        QTextStream logStream(m_logFile);
        logStream << text << QString("\r");
        logStream.flush();
    }
}

void ReaderInteractorWidget::newAnswerFromSerialComm(const QString answer)
{
    writeToOutput(answer);
}

void ReaderInteractorWidget::newAnswerFromNetworkComm(const QString answer)
{
    writeToOutput(answer);
}

void ReaderInteractorWidget::btSendCommandClicked()
{
    sendCommand(ui->leCommand->text());
}

void ReaderInteractorWidget::leCommandReturnPressed()
{
    sendCommand(ui->leCommand->text());
}

void ReaderInteractorWidget::btClearOutputClicked()
{
    ui->teOutput->clear();
}

void ReaderInteractorWidget::btLogToClicked()
{
    QString fileName(QFileDialog::getOpenFileName(this, tr("Select log file"), QDir::homePath()));
    if(!fileName.isEmpty()){
        // If a file was selected.


        if(m_logFile->isOpen())
            m_logFile->close();

        // Test if the log file can be used.
        m_logFile->setFileName(fileName);
        if(m_logFile->open(QIODevice::Append)){

            // The log file can be used.

            m_logFile->close();
            m_useLogFile = true;
            SystemMessagesWidget::instance()->writeMessage(tr("The selected log file is good."));
        }else{

            // The log file cannot be used.

            m_useLogFile = false;
            SystemMessagesWidget::instance()->writeMessage(tr("Cannot use the selected log file. It is not writable"));
        }
        ui->leLogFile->setText(fileName);

    }else{

        // No file was selected.

        ui->leLogFile->setText("");
        m_useLogFile = false;
    }
}

void ReaderInteractorWidget::btStartPauseReadingClicked(const bool checked)
{
    if(checked){
        // Start reading selected.

        // Try to open the log file to use if must use it.
        if(m_useLogFile){
            if( ! m_logFile->open((QIODevice::OpenModeFlag)ui->cbLogType->currentData().toInt())){
                m_useLogFile = false;
                SystemMessagesWidget::instance()->writeMessage(tr("Problem with the log file. It is not writable anymore, "
                                                                  "and is not going to be used."));
            }
        }

        ui->btStartPauseReading->setText("Pause");
        ui->btLogTo->setEnabled(false);
        ui->cbLogType->setEnabled(false);
        ui->btSendCommand->setEnabled(true);
        ui->leCommand->setEnabled(true);


        // Connect the signal of new messagens from connections, to display them in the QTextEdit and in the log file.
        if(m_connectionType == Settings::KSerial){
            connect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
        }
        else if(m_connectionType == Settings::KNetwork){
            connect(NetworkCommunication::instance(), SIGNAL(newReaderAnswer(QString)), this, SLOT(newAnswerFromNetworkComm(QString)));
        }

    }else{
        // Pause reading selected.

        if(m_logFile->isOpen())
            m_logFile->close();

        ui->btStartPauseReading->setText("Start");
        ui->btLogTo->setEnabled(true);
        ui->cbLogType->setEnabled(true);
        ui->btSendCommand->setEnabled(false);
        ui->leCommand->setEnabled(false);

        // Disconnect the signals to stop the output of answers.
        if(m_connectionType == Settings::KSerial){
            disconnect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
        }
        else if(m_connectionType == Settings::KNetwork){
            disconnect(NetworkCommunication::instance(), SIGNAL(newReaderAnswer(QString)), this, SLOT(newAnswerFromNetworkComm(QString)));
        }
    }

}
