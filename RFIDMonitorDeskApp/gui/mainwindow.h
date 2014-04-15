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

/**
 * @brief The MainWindow class is the main Widget of the app.
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /**
     * @brief m_serialConnConfigWidget holds the window instance of serial connection configuration.
     */
    SerialConnConfigWidget *m_serialConnConfigWidget;

    /**
     * @brief m_networkConnConfigWidget holds the window instance of network connection configuration.
     */
    NetworkConnConfigWidget *m_networkConnConfigWidget;

    /**
     * @brief m_readerInteractorWidget holds the window instance of the reader interactor.
     */
    ReaderInteractorWidget *m_readerInteractorWidget;

    /**
     * @brief m_rfidmonitorInteractorWidget holds the window instance of the RFIDMonitor interactor.
     */
    RFIDMonitorInteractorWidget *m_rfidmonitorInteractorWidget;

    /**
     * @brief prepareReaderInteractorWidget is a function called when the connection (network or serial) is ready,
     * and the user wants to interact with the reader.
     *
     * @param connectionType inform the function which connection (KNetwork or KSerial) will be used in the interaction.
     */
    void prepareReaderInteractorWidget(const Settings::ConnectionType &connectionType);

    /**
     * @brief prepareRFIDMonitorInteractorWidget is a function called when the network connection is ready and the
     * user wants to interact with the RFIDMonitor.
     */
    void prepareRFIDMonitorInteractorWidget();

    /**
     * @brief tabSetUpConnectionIndex holds the index of SetUpConnection tab for simple use in the code.
     */
    int tabSetUpConnectionIndex;

    /**
     * @brief tabConnectionIndex holds the index of Connection tab for simple use in the code.
     */
    int tabConnectionIndex;

public slots:
    /**
     * @brief serialCommunicationReady is a slot called when the serial communication is ready.
     */
    void serialCommunicationReady();

    /**
     * @brief networkCommunicationReady is a slot called when the network communication is ready.
     */
    void networkCommunicationReady();

    /**
     * @brief rbSerialClicked is a slot called when user select the Serial connection Type.
     *
     * This slot remove a possible existing Network connection configuration window,
     * and instantiate a new Serial configuration window.
     */
    void rbSerialClicked();

    /**
     * @brief rbNetworkClicked is a slot called when user select the Network connection Type.
     *
     * This slot remove a possible existing Serial connection configuration window,
     * and instantiate a new Network configuration window.
     */
    void rbNetworkClicked();

    /**
     * @brief btCloseConnectionClicked is a slot called to close a open connection configuration
     * window (serial or network). Consequently the window close the connection it is using.
     */
    void btCloseConnectionClicked();
};

#endif // MAINWINDOW_H
