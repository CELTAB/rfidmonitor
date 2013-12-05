#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <algorithm>
#include <thread>

#include "widget.h"
#include "ui_widget.h"
#include "datareader.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_dataReader(0)
{
    ui->setupUi(this);
    m_dataReader = new DataReader(this);

    connect(ui->btSelectFile, SIGNAL(clicked()), SLOT(onChooseFileClicked()));
    connect(ui->btWrite, SIGNAL(clicked()), SLOT(onWriteClicked()));
    connect(ui->btClear, SIGNAL(clicked()), SLOT(onClearClicked()));
//    connect(this, SIGNAL(dataReady(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    connect(this, SIGNAL(dataReady(QByteArray)), m_dataReader, SLOT(writeData(QByteArray)));


    QDir dev("/dev","tty*",QDir::Name,QDir::System);
    QStringList items = dev.entryList();
    ui->comboBox->addItems(items);

    if(items.contains("ttyUSB0"))
        ui->comboBox->setCurrentText("ttyUSB0");

}

Widget::~Widget()
{
    delete ui;
}

void Widget::onChooseFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath(), tr("Image Files (*.csv)"));
    ui->lineEdit->setText(fileName);
}

void Widget::onWriteClicked()
{
    if(m_dataReader->isOpen())
        m_dataReader->closeDevice();

    m_dataReader->openDevice(ui->comboBox->currentText());
    QFile file(ui->lineEdit->text());
    if(file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        QString line;
        int qt = 0;
        while(!(line = stream.readLine()).isNull()) {
            line.append("\n");
            QStringList rest = line.split(" ");
            QString sixteen = rest.at(2);
            sixteen = QString().sprintf("%016d", sixteen.toInt());
            QString data = QString("%1 %2 %3").arg(rest.at(0)).arg(rest.at(1)).arg(sixteen);
            emit dataReady(QByteArray(data.toStdString().c_str()));
            qt++;
        }
        ui->plainTextEdit->appendPlainText(QString("Total RFID = %1").arg(qt));
    }
}

void Widget::onClearClicked()
{

}
