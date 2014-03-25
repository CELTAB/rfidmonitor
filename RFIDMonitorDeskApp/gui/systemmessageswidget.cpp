#include <QTime>
#include <QMessageBox>

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
}

SystemMessagesWidget::~SystemMessagesWidget()
{
    delete ui;
}

void SystemMessagesWidget::writeMessage(const QString message,
                                        const SystemMessagesWidget::MessageLevel messageLevel,
                                        const SystemMessagesWidget::MessageBehavior messageBehavior)
{
    QString messageLevelString;
    QMessageBox::Icon icon;
    switch (messageLevel) {
    case KError:
        messageLevelString = "ERROR";
        icon = QMessageBox::Critical;
        break;
    case KInfo:
        messageLevelString = "INFORMATIVE";
        icon = QMessageBox::Information;
        break;
    case KWarning:
        messageLevelString = "WARNING";
        icon = QMessageBox::Warning;
        break;
    case KFatal:
        messageLevelString = "FATAL ERROR";
        icon = QMessageBox::Critical;
        break;
    default:
        messageLevelString = "UNKNOW MESSAGE LEVEL";
        icon = QMessageBox::Warning;
        break;
    }

    if(messageBehavior == KOnlyLog || messageBehavior == KDialogAndLog){

        ui->teMessages->append(QString("["
                                       + QTime::currentTime().toString()
                                       + "]-["
                                       + messageLevelString
                                       + "] : "
                                       + message));
    }
    if(messageBehavior == KOnlyDialog || messageBehavior == KDialogAndLog){
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
