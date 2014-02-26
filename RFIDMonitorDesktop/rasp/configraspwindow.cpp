#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>

#include "configraspwindow.h"
#include "ui_configraspwindow.h"

ConfigRaspWindow::ConfigRaspWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigRaspWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::Window);
    this->setWindowModality(Qt::ApplicationModal);

    //TAB RASPBERRY CONFIGURATION
    setStateRaspConfig(KEmpty);
    connect(ui->btCreateNewEmptyFile, SIGNAL(clicked()), this, SLOT(btCreateNewFileClicked()));
    connect(ui->btDiscard, SIGNAL(clicked()), this, SLOT(btDiscardClicked()));
    connect(ui->btSave, SIGNAL(clicked()), this, SLOT(btSaveClicked()));
    connect(ui->btEdit, SIGNAL(clicked()), this, SLOT(btEditClicked()));
    connect(ui->btCloneFile, SIGNAL(clicked()), this, SLOT(btCloneFileClicked()));
    connect(ui->btOpenExistingFile, SIGNAL(clicked()), this, SLOT(btOpenExistingFileClicked()));
}

ConfigRaspWindow::~ConfigRaspWindow()
{
    delete ui;
}

ConfigRaspWindow::FormState ConfigRaspWindow::getConfigRaspState() const
{
    return configRaspState;
}

void ConfigRaspWindow::setStateRaspConfig(const ConfigRaspWindow::FormState &value)
{
    configRaspState = value;

    switch (configRaspState) {
    case KEmpty:
        clearFields();
        ui->leFile->setEnabled(false);
        ui->leDataID->setEnabled(false);
        ui->leDataIP->setEnabled(false);
        ui->leDataName->setEnabled(false);
        ui->leDataDefaultCommunication->setEnabled(false);
        ui->leDataDefaultExport->setEnabled(false);
        ui->leDataDefaultPersistence->setEnabled(false);
        ui->leDataDefaultReading->setEnabled(false);
        ui->btCreateNewEmptyFile->setEnabled(true);
        ui->btOpenExistingFile->setEnabled(true);
        ui->btCloneFile->setEnabled(false);
        ui->btSave->setEnabled(false);
        ui->btDiscard->setEnabled(false);
        ui->btEdit->setEnabled(false);
        break;
    case KEditing:
        ui->leFile->setEnabled(false);
        ui->leDataID->setEnabled(true);
        ui->leDataIP->setEnabled(true);
        ui->leDataName->setEnabled(true);
        ui->leDataDefaultCommunication->setEnabled(true);
        ui->leDataDefaultExport->setEnabled(true);
        ui->leDataDefaultPersistence->setEnabled(true);
        ui->leDataDefaultReading->setEnabled(true);
        ui->btCreateNewEmptyFile->setEnabled(false);
        ui->btOpenExistingFile->setEnabled(false);
        ui->btCloneFile->setEnabled(false);
        ui->btSave->setEnabled(true);
        ui->btDiscard->setEnabled(true);
        ui->btEdit->setEnabled(false);
        break;
    case KWithObject:
        ui->leFile->setEnabled(false);
        ui->leDataID->setEnabled(false);
        ui->leDataIP->setEnabled(false);
        ui->leDataName->setEnabled(false);
        ui->leDataDefaultCommunication->setEnabled(false);
        ui->leDataDefaultExport->setEnabled(false);
        ui->leDataDefaultPersistence->setEnabled(false);
        ui->leDataDefaultReading->setEnabled(false);
        ui->btCreateNewEmptyFile->setEnabled(true);
        ui->btOpenExistingFile->setEnabled(true);
        ui->btCloneFile->setEnabled(true);
        ui->btSave->setEnabled(false);
        ui->btDiscard->setEnabled(false);
        ui->btEdit->setEnabled(true);
        break;
    default:
        qDebug() << tr("INTERNAL ERROR: enum not defined.");
        break;
    }
}

void ConfigRaspWindow::clearFields()
{
    ui->leFile->setText("");
    ui->leDataID->setText("");
    ui->leDataIP->setText("");
    ui->leDataName->setText("");
    ui->leDataDefaultCommunication->setText("");
    ui->leDataDefaultExport->setText("");
    ui->leDataDefaultPersistence->setText("");
    ui->leDataDefaultReading->setText("");
}

void ConfigRaspWindow::btCreateNewFileClicked()
{
    QString destinationDirPath(QFileDialog::getExistingDirectory(this,
                                                     tr("Select the directory to put the new configuration file."),
                                                     QCoreApplication::applicationDirPath()));
    if( ! destinationDirPath.isEmpty()){
        QDir destinationDir(destinationDirPath);
        QFile newFile(destinationDir.path()+"/rfidmonitor-configuration.json");
        if(newFile.exists()){
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setText(tr("The file you choosed is invalid because it already exists. "
                           "Please choose a folder or delete the existing one."));
            msg.setFocus();
            msg.exec();
        }else{
            if(newFile.open(QIODevice::ReadWrite | QIODevice::Text)){
                //After the opening test, it must be closed and deleted, because
                //there is not assurance the user will really use the file.
                newFile.close();
                QFile::remove(newFile.fileName());

                if(raspConfigurationJson)
                    raspConfigurationJson->deleteLater();
                raspConfigurationJson = new RaspConfigurationJson;

                clearFields();
                ui->leFile->setText(newFile.fileName());

                setStateRaspConfig(KEditing);
            }
            else{
                QMessageBox msg;
                msg.setIcon(QMessageBox::Critical);
                msg.setText(tr("The file you choosed have no permissions to be created. "
                               "Please change the permissions or choose a new place to put the file."));
                msg.setFocus();
                msg.exec();
            }
        }
    }
}

void ConfigRaspWindow::btDiscardClicked()
{
    setStateRaspConfig(KEmpty);
}

void ConfigRaspWindow::btSaveClicked()
{
    QFile file(ui->leFile->text());
    if(file.open(QIODevice::WriteOnly | QIODevice::Text )){

        if(raspConfigurationJson){
            raspConfigurationJson->setId(ui->leDataID->text());
            raspConfigurationJson->setIp(ui->leDataIP->text());
            raspConfigurationJson->setName(ui->leDataName->text());
            raspConfigurationJson->insertDefaultService("communication", ui->leDataDefaultCommunication->text());
            raspConfigurationJson->insertDefaultService("export", ui->leDataDefaultExport->text());
            raspConfigurationJson->insertDefaultService("persistence", ui->leDataDefaultPersistence->text());
            raspConfigurationJson->insertDefaultService("reading", ui->leDataDefaultReading->text());

            QTextStream stream(&file);
            stream << raspConfigurationJson->generateJson();
            stream.flush();

            setStateRaspConfig(KWithObject);
        }else{
            qDebug() << tr("INTERNAL ERROR: cannot save because raspConfigurationJson object does not exist.");
        }
        file.close();

    }else{
        qDebug() << tr("INTERNAL ERROR: json file cannot be open to write.");
    }
}

void ConfigRaspWindow::btEditClicked()
{

}

void ConfigRaspWindow::btOpenExistingFileClicked()
{
    QString filePath(QFileDialog::getOpenFileName(this,
                                                     tr("Select configuration file you want to open."),
                                                     QCoreApplication::applicationDirPath(),
                                                     tr("JSON files (*.json)"),
                                                     0,
                                                     QFileDialog::DontConfirmOverwrite));
    if( ! filePath.isEmpty()){
        QFile file(filePath);
        if(file.open(QIODevice::ReadWrite | QIODevice::Text)){

            if(raspConfigurationJson)
                raspConfigurationJson->deleteLater();
            raspConfigurationJson = new RaspConfigurationJson;

            if( ! raspConfigurationJson->loadJson(file.readAll())){
                QMessageBox msg;
                msg.setIcon(QMessageBox::Critical);
                msg.setText(tr("The file you choosed could not be parsed."));
                msg.setFocus();
                msg.exec();
            }else{
                clearFields();

                ui->leFile->setText(filePath);
                ui->leDataID->setText(raspConfigurationJson->getId());
                ui->leDataIP->setText(raspConfigurationJson->getIp());
                ui->leDataName->setText(raspConfigurationJson->getName());
                ui->leDataDefaultCommunication->setText(raspConfigurationJson->getDefaultServices().value("communication"));
                ui->leDataDefaultExport->setText(raspConfigurationJson->getDefaultServices().value("export"));
                ui->leDataDefaultPersistence->setText(raspConfigurationJson->getDefaultServices().value("persistence"));
                ui->leDataDefaultReading->setText(raspConfigurationJson->getDefaultServices().value("reading"));

                setStateRaspConfig(KWithObject);
            }
            file.close();

        }
        else{
            QMessageBox msg;
            msg.setIcon(QMessageBox::Critical);
            msg.setText(tr("The file you choosed have no permissions to be read and modified."));
            msg.setFocus();
            msg.exec();
        }
    }

}

void ConfigRaspWindow::btCloneFileClicked()
{

}
