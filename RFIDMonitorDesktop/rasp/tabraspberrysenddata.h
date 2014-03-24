#ifndef TABRASPBERRYSENDDATA_H
#define TABRASPBERRYSENDDATA_H

#include <QWidget>

namespace Ui {
class TabRaspberrySendData;
}

class TabRaspberrySendData : public QWidget
{
    Q_OBJECT

public:
    explicit TabRaspberrySendData(QWidget *parent = 0);
    ~TabRaspberrySendData();

private:
    Ui::TabRaspberrySendData *ui;
};

#endif // TABRASPBERRYSENDDATA_H
