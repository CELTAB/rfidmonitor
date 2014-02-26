#ifndef TABRASPBERRYCONFIGURATION_H
#define TABRASPBERRYCONFIGURATION_H

#include <QWidget>

#include "raspconfigurationjson.h"

namespace Ui {
class TabRaspberryConfiguration;
}

class TabRaspberryConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit TabRaspberryConfiguration(QWidget *parent = 0);
    ~TabRaspberryConfiguration();
    enum FormState {KEmpty=0, KWithObject, KEditing, KNew};
    TabRaspberryConfiguration::FormState getConfigRaspState() const;
    void setStateRaspConfig(const TabRaspberryConfiguration::FormState &value);
    void clearFields();

private:
    Ui::TabRaspberryConfiguration *ui;
    TabRaspberryConfiguration::FormState configRaspState;
    RaspConfigurationJson *raspConfigurationJson;

public slots:
    void btCreateNewFileClicked();
    void btDiscardClicked();
    void btSaveClicked();
    void btEditClicked();
    void btOpenExistingFileClicked();
    void btCloneFileClicked();
};

#endif // TABRASPBERRYCONFIGURATION_H
