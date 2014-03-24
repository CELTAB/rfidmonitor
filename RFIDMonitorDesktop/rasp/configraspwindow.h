#ifndef CONFIGRASPWINDOW_H
#define CONFIGRASPWINDOW_H

#include <QWidget>

#include "raspconfigurationjson.h"
#include "tabraspberryconfiguration.h"
#include "tabraspberryconnection.h"
#include "tabraspberrysenddata.h"

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
    TabRaspberryConfiguration *tabRaspberryConfigurarion;
    TabRaspberryConnection *tabRaspberryConnection;
    TabRaspberrySendData *tabRaspberrySendData;

};

#endif // CONFIGRASPWINDOW_H
