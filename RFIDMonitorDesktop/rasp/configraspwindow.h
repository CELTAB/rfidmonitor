#ifndef CONFIGRASPWINDOW_H
#define CONFIGRASPWINDOW_H

#include <QWidget>

namespace Ui {
class ConfigRaspWindow;
}

class ConfigRaspWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigRaspWindow(QWidget *parent = 0);
    ~ConfigRaspWindow();

private:
    Ui::ConfigRaspWindow *ui;
    bool needToBeSaved;
    void monitorDataChanged();

public slots:
    void dataChanged(QString);
    void btCreateNewFileClicked();
};

#endif // CONFIGRASPWINDOW_H
