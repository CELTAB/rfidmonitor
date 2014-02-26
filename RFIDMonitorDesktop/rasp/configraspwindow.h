#ifndef CONFIGRASPWINDOW_H
#define CONFIGRASPWINDOW_H

#include <QWidget>

#include "raspconfigurationjson.h"

namespace Ui {
class ConfigRaspWindow;
}

class ConfigRaspWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigRaspWindow(QWidget *parent = 0);
    ~ConfigRaspWindow();
    enum FormState {KEmpty=0, KWithObject, KEditing, KNew};
    ConfigRaspWindow::FormState getConfigRaspState() const;
    void setStateRaspConfig(const ConfigRaspWindow::FormState &value);
    void clearFields();

private:
    Ui::ConfigRaspWindow *ui;
    ConfigRaspWindow::FormState configRaspState;
    RaspConfigurationJson *raspConfigurationJson;

public slots:
    void btCreateNewFileClicked();
    void btDiscardClicked();
    void btSaveClicked();
    void btEditClicked();
    void btOpenExistingFileClicked();
    void btCloneFileClicked();
};

#endif // CONFIGRASPWINDOW_H
