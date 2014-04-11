#include <QDebug>
#include <QPixmap>
#include <QPainter>

#include "rictlmb2b30widget.h"
#include "ui_rictlmb2b30widget.h"
#include "../communication/serialcommunication.h"
#include "../communication/networkcommunication.h"
#include "systemmessageswidget.h"

RICTLMB2B30Widget::RICTLMB2B30Widget(Settings::ConnectionType connType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RICTLMB2B30Widget),
    m_connectionType(connType),
    m_waitingForAnswer(false),
    m_waitingForCheck(false)
{
    ui->setupUi(this);

    m_timeout.setInterval(1000);
    m_timeout.setSingleShot(true);

    ui->leIdentification->setInputMask("hhhhhhhhhhhhhhhh"); //hexa only mask
    ui->leIdentification->setMaxLength(16);
    ui->leIdentification->setCursorPosition(0);

    connect(ui->leIdentification, SIGNAL(textChanged(QString)), this, SLOT(leIdentificationChanged(QString)));
    connect(ui->btWrite, SIGNAL(clicked()), this, SLOT(btWriteClicked()));
    connect(SerialCommunication::instance(), SIGNAL(newAnswer(QString)), this, SLOT(newAnswerFromSerialComm(QString)));
    connect(&m_timeout, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(ui->rbDecimal, SIGNAL(clicked()), this, SLOT(rbDecimalClicked()));
    connect(ui->rbHexadecimal, SIGNAL(clicked()), this, SLOT(rbHexadecimalClicked()));
    connect(ui->btNextIdentification, SIGNAL(clicked()), this, SLOT(incrementIdentification()));
}

RICTLMB2B30Widget::~RICTLMB2B30Widget()
{
    delete ui;
}

void RICTLMB2B30Widget::sendCommand(const QString &command)
{
    if(m_connectionType == Settings::KSerial){
        SerialCommunication::instance()->sendCommand(command,SerialCommunication::KASCII);
    }
    else if(m_connectionType == Settings::KNetwork){
        NetworkCommunication::instance()->sendNewCommandToReader(command);
    }
}

void RICTLMB2B30Widget::leIdentificationChanged(QString newText)
{
    m_identification.clear();
    if(ui->leIdentification->text().size() > 0){

        if(ui->rbDecimal->isChecked()){
            quint64 decimal = ui->leIdentification->text().toULongLong();
            m_identification.setNum(decimal,16);
        }else if(ui->rbHexadecimal->isChecked()){
            m_identification.append(ui->leIdentification->text());
        }
        QString finalIdentification;
        finalIdentification.fill('0', 16 - m_identification.size());
        finalIdentification.append(m_identification);
        m_identification.clear();
        m_identification.append(finalIdentification);
        ui->labelIdentificationStatus->setText(tr("The identification will be writted in hexadecimal, as: ") + m_identification);
    }else{
        ui->labelIdentificationStatus->clear();
    }
}

void RICTLMB2B30Widget::btWriteClicked()
{
    ui->labelRectangleStatus->close();

    if((!ui->rbDecimal->isChecked() && !ui->rbHexadecimal->isChecked())){
        SystemMessagesWidget::instance()->writeMessage(tr("Please, select the identification type."), SystemMessagesWidget::KWarning);
        return;
    }

    if(ui->leIdentification->text().size() < 1){
        SystemMessagesWidget::instance()->writeMessage(tr("The identification must have at least 1 characters."), SystemMessagesWidget::KWarning);
        return;
    }

    ui->btWrite->setEnabled(false);
    m_waitingForAnswer = true;
    sendCommand("L");
    sendCommand("K0");
    sendCommand("P" + m_identification);
    m_timeout.start();
}

void RICTLMB2B30Widget::newAnswerFromSerialComm(QString answer)
{
    if(m_waitingForAnswer){
        if(answer.contains(QString("P0"))){
            m_timeout.start(); //restart the timeout to have more time, now check the new identification.
            m_waitingForCheck = true;
        }else if(m_waitingForCheck && answer.contains(QString().setNum(answer.toULongLong()))){
            m_timeout.stop();
            m_waitingForCheck = false;

            QString message(tr("New identification defined successfully"));
            SystemMessagesWidget::instance()->writeMessage(message, SystemMessagesWidget::KInfo);
            ui->labelRectangleStatus->setText(message);
            ui->labelRectangleStatus->setStyleSheet("QLabel { background-color : green;}");
            ui->labelRectangleStatus->show();

        }else if(answer.contains(QString("P1")))
            SystemMessagesWidget::instance()->writeMessage(tr("Reader said: P1"), SystemMessagesWidget::KError);
        else if(answer.contains(QString("P12"))){
            SystemMessagesWidget::instance()->writeMessage(tr("The reader is operatin in K1 mode. Change it to K0! Reader said: P12"), SystemMessagesWidget::KError);
        }else if(answer.contains(QString("P2"))){
            SystemMessagesWidget::instance()->writeMessage(tr("Reader said: P2"), SystemMessagesWidget::KError);
        }
        ui->btWrite->setEnabled(true);
    }
}

void RICTLMB2B30Widget::timeout()
{
    m_waitingForAnswer = false;
    ui->btWrite->setEnabled(true);
    ui->labelRectangleStatus->setText(tr("Failed to define new identification."));
    ui->labelRectangleStatus->setStyleSheet("QLabel { background-color : red;}");
    ui->labelRectangleStatus->show();
}

void RICTLMB2B30Widget::rbDecimalClicked()
{
    ui->leIdentification->setInputMask("0000000000000000"); //numeric only mask
    leIdentificationChanged(QString());
}

void RICTLMB2B30Widget::rbHexadecimalClicked()
{
    ui->leIdentification->setInputMask("hhhhhhhhhhhhhhhh"); //hexa only mask
    leIdentificationChanged(QString());
}

void RICTLMB2B30Widget::incrementIdentification()
{
    if(ui->leIdentification->text().size() > 0){
        quint64 val = ui->leIdentification->text().toULongLong();
        val++;
        ui->leIdentification->setText(QString().setNum(val));
    }
}
