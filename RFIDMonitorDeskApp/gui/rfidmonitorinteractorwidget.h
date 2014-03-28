#ifndef RFIDMONITORINTERACTORWIDGET_H
#define RFIDMONITORINTERACTORWIDGET_H

#include <QWidget>

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

private:
    Ui::RFIDMonitorInteractorWidget *ui;
    void loadConfigurationFromJson(const QByteArray &byteArray);
    void clearForm();
    void disableAllForm();
    void sendCurrentConfiguration();
    void persistLocalyCurrentConfig(const QString &mac, const QByteArray &json);

public slots:
    void raspDisconnected();
    void newConfigFromRaspArrived(QByteArray json);
    void newAnswerFromRaspArrived(QString answer);
    void btSendToRaspClicked();
};

#endif // RFIDMONITORINTERACTORWIDGET_H
