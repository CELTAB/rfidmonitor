#include "readerinteractorwidget.h"
#include "ui_readerinteractorwidget.h"

ReaderInteractorWidget::ReaderInteractorWidget(Settings::ConnectionType type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderInteractorWidget),
    m_serialCommunication(0),
    m_networkCommunication(0),
    m_connectionType(type)
{
    ui->setupUi(this);

    connect(ui->btSendCommand, SIGNAL(clicked()), this, SLOT(btSendCommandClicked()));

    if(m_connectionType == Settings::KSerial){
        connect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
    }
//    else if(m_connectionType == Settings::KNetwork){
//        connect(NetworkCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromNetworkComm(QString)));
//    }
}

ReaderInteractorWidget::~ReaderInteractorWidget()
{
    delete ui;
}

void ReaderInteractorWidget::sendCommand(const QString command)
{
    if(m_connectionType == Settings::KSerial){
        SerialCommunication::instance()->sendCommand(command);
    }
//    else if(m_connectionType == Settings::KNetwork){
//        NetworkCommunication::instance()->sendCommand(command);
//    }
}

void ReaderInteractorWidget::newAnswerFromSerialComm(QString answer)
{
    ui->teOutput->append(answer);
}

void ReaderInteractorWidget::btSendCommandClicked()
{
    sendCommand(ui->leCommand->text());
    ui->leCommand->clear();
}
