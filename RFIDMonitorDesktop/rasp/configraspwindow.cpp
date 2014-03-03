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

    ui->leFile->setEnabled(false);

    ui->leDataID->setEnabled(false);
    ui->leDataIP->setEnabled(false);
    ui->leDataName->setEnabled(false);
    ui->btCloneFile->setEnabled(false);
    ui->btSave->setEnabled(false);
    ui->btDiscard->setEnabled(false);

    needToBeSaved = false;
    monitorDataChanged();

    connect(ui->btCreateNewEmptyFile, SIGNAL(clicked()), this, SLOT(btCreateNewFileClicked()));
}

ConfigRaspWindow::~ConfigRaspWindow()
{
    delete ui;
}

void ConfigRaspWindow::monitorDataChanged()
{
    connect(ui->leDataID, SIGNAL(textChanged(QString)), this, SLOT(dataChanged(QString)));
    connect(ui->leDataIP, SIGNAL(textChanged(QString)), this, SLOT(dataChanged(QString)));
    connect(ui->leDataName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged(QString)));
}

void ConfigRaspWindow::dataChanged(QString)
{
    needToBeSaved = true;
}

void ConfigRaspWindow::btCreateNewFileClicked()
{
    QString newFilePath(QFileDialog::getSaveFileName(this,
                                                     tr("Select the directory and put a new file name."),
                                                     QCoreApplication::applicationDirPath(),
                                                     tr("JSON files (*.json)"),
                                                     0,
                                                     QFileDialog::DontConfirmOverwrite));
    if( ! newFilePath.isEmpty()){
        if( ! newFilePath.endsWith(".json"))
            newFilePath.append(".json");

        QFile newFile(newFilePath);
        if(newFile.exists()){
            QMessageBox msg;
            msg.setText(tr("The file you choosed is invalid because it already exists."
                           "Please choose a new name or delete the existing file."));
            msg.exec();
        }

        ui->leFile->setText(newFilePath);
    }
}
