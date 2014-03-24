#include "tabraspberrysenddata.h"
#include "ui_tabraspberrysenddata.h"

TabRaspberrySendData::TabRaspberrySendData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRaspberrySendData)
{
    ui->setupUi(this);
}

TabRaspberrySendData::~TabRaspberrySendData()
{
    delete ui;
}
