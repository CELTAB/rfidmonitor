#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "gui/serialconnconfigwidget.h"
#include "gui/networkconnconfigwidget.h"
#include "gui/readerinteractorwidget.h"
#include "gui/rfidmonitorinteractorwidget.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SerialConnConfigWidget *m_serialConnConfigWidget;
    NetworkConnConfigWidget *m_networkConnConfigWidget;

    ReaderInteractorWidget *m_readerInteractorWidget;
    RFIDMonitorInteractorWidget *m_rfidmonitorInteractorWidget;

    void prepareReaderInteractorWidget(Settings::ConnectionType connectionType);
    void prepareRFIDMonitorInteractorWidget();

    int tabSetUpConnectionIndex;
    int tabConnectionIndex;
public slots:
    void serialCommunicationReady();
    void networkCommunicationReady();
    void rbSerialClicked();
    void rbNetworkClicked();
    void btCloseConnectionClicked();
};

#endif // MAINWINDOW_H
