#ifndef RFIDMONITORINTERACTORWIDGET_H
#define RFIDMONITORINTERACTORWIDGET_H

#include <QWidget>
#include <QJsonObject>

#include "../json/rfidmonitorsettings.h"

namespace Ui {
class RFIDMonitorInteractorWidget;
}

class RFIDMonitorInteractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RFIDMonitorInteractorWidget(QWidget *parent = 0);
    ~RFIDMonitorInteractorWidget();
    void closeConnection();
    enum FormState {KBlocked = 1, KEditing, KWithObject, KEmpty};

private:
    Ui::RFIDMonitorInteractorWidget *ui;
    void loadConfigurationFromJson(const QJsonObject &obj);
    void clearForm();
    void formEnabled(bool enabled);
    void sendCurrentConfiguration();
    void persistLocalyCurrentConfig(const QString &mac, const QByteArray &json);
    json::RFIDMonitorSettings *m_raspSettings;
    void changeFormState(FormState state);

public slots:
    void raspDisconnected();
    void newConfigFromRaspArrived(const QJsonObject json);
    void newAnswerFromRaspArrived(const QString answer);
    void btSendToRaspClicked();
    void getConfigFromRaspClicked();
    void newConfigStatusFromRasp(QJsonObject obj);
    void btEditClicked();
    void btCancelClicked();
};

#endif // RFIDMONITORINTERACTORWIDGET_H
