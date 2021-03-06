/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the RFIDMonitor project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QDateTime>
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
    if( ! QDir::current().exists("logs/")){
        logDirIsOk = QDir::current().mkdir("logs/");
    }

    if(logDirIsOk){
        m_logFile = new QFile(
                    QDir::current().absolutePath()   +
                    QString("/logs/%1_%2.log")
                        .arg(QApplication::applicationName())
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"))
                    );

        if( ! m_logFile->open(QIODevice::ReadWrite | QIODevice::Text)){
            qDebug() << "here";
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
