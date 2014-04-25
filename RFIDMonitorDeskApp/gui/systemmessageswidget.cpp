#include <QTime>
#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QDebug>

#include "systemmessageswidget.h"
#include "ui_systemmessageswidget.h"

SystemMessagesWidget *SystemMessagesWidget::instance()
{
    static SystemMessagesWidget *singleton = 0;
    if(! singleton)
        singleton = new SystemMessagesWidget;
    return singleton;
}

SystemMessagesWidget::SystemMessagesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemMessagesWidget)
{
    ui->setupUi(this);

    prepareLogfile();
}

void SystemMessagesWidget::prepareLogfile()
{
    m_logFile = 0;
    bool logDirIsOk = true;

    QString logsDir(QApplication::applicationDirPath() +
                    QDir::separator() +
                    "logs" +
                    QDir::separator());
    if( ! QDir(logsDir).exists()){
        logDirIsOk = QDir().mkdir(logsDir);
    }
    if(logDirIsOk){
        m_logFile = new QFile(
                    logsDir +
                    QString("%1_%2.log")
                        .arg(QApplication::applicationName())
                        .arg(QTime::currentTime().toString())
                    );
        if( ! m_logFile->open(QIODevice::ReadWrite | QIODevice::Text)){
            m_logFile->deleteLater();
            m_logFile = 0;
        }
    }
}

SystemMessagesWidget::~SystemMessagesWidget()
{
    delete ui;
    if(m_logFile && m_logFile->isOpen())
        m_logFile->close();
}

void SystemMessagesWidget::writeMessage(const QString &message,
                                        const SystemMessagesWidget::MessageLevel &messageLevel,
                                        const SystemMessagesWidget::MessageBehavior &messageBehavior)
{
    QString messageLevelString;
    QMessageBox::Icon icon;
    switch (messageLevel) {
    case KError:
        messageLevelString = tr("ERROR");
        icon = QMessageBox::Critical;
        break;
    case KInfo:
        messageLevelString = tr("INFORMATIVE");
        icon = QMessageBox::Information;
        break;
    case KWarning:
        messageLevelString = tr("WARNING");
        icon = QMessageBox::Warning;
        break;
    case KFatal:
        messageLevelString = tr("FATAL ERROR");
        icon = QMessageBox::Critical;
        break;
    case KDebug:
        messageLevelString = tr("DEBUG");
        break;
    default:
        messageLevelString = tr("UNKNOW MESSAGE LEVEL");
        icon = QMessageBox::Warning;
        break;
    }

    QString formatedMessage("["
                            + QTime::currentTime().toString()
                            + "]-["
                            + messageLevelString
                            + "] : "
                            + message);

    // Always write the message in the log file.
    if(m_logFile){

        qDebug() << formatedMessage;

        QTextStream stream(m_logFile);
        stream << formatedMessage << "\n";
        stream.flush();
    }else{
        qDebug() << "Cannot use the log file.";
    }

    // If is needed to write the message in another way than log file, check which and display.
    if( messageBehavior != KOnlyLogfile){

        if(messageBehavior == KOnlyTextbox || messageBehavior == KDialogAndTextbox){

            ui->teMessages->append(formatedMessage);
        }
        if(messageBehavior == KOnlyDialog || messageBehavior == KDialogAndTextbox){
            QMessageBox *box = new QMessageBox(this);
            box->setAttribute( Qt::WA_DeleteOnClose );
            box->setStandardButtons( QMessageBox::Ok );
            box->setWindowTitle(messageLevelString);
            box->setText(message);
            box->setIcon(icon);
            box->setModal(true);
            box->show();
        }
    }
}
