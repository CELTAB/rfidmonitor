/****************************************************************************
**
** www.celtab.org.br
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the RFIDMonitor project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "gui/serialconnconfigwidget.h"
#include "gui/networkconnconfigwidget.h"
#include "gui/readermanipulatorwidget.h"
#include "gui/rfidmonitormanipulatorwidget.h"
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
     * @brief m_readerManipulatorWidget holds the window instance of the reader Manipulator.
     */
    ReaderManipulatorWidget *m_readerManipulatorWidget;

    /**
     * @brief m_rfidmonitorManipulatorWidget holds the window instance of the RFIDMonitor Manipulator.
     */
    RFIDMonitorManipulatorWidget *m_rfidmonitorManipulatorWidget;

    /**
     * @brief prepareReaderManipulatorWidget is a function called when the connection (network or serial) is ready,
     * and the user wants to interact with the reader.
     *
     * @param connectionType inform the function which connection (KNetwork or KSerial) will be used in the interaction.
     */
    void prepareReaderManipulatorWidget(const Settings::ConnectionType &connectionType);

    /**
     * @brief prepareRFIDMonitorManipulatorWidget is a function called when the network connection is ready and the
     * user wants to interact with the RFIDMonitor.
     */
    void prepareRFIDMonitorManipulatorWidget();

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
    void closeConnection();
};

#endif // MAINWINDOW_H
