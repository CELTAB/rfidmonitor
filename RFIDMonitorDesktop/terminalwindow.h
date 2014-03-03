#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>

#include "terminalcommunicator.h"

namespace Ui {
class TerminalWindow;
}

class TerminalWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = 0);
    ~TerminalWindow();

private:
    Ui::TerminalWindow *ui;
    TerminalCommunicator *m_terminalCommunicator;
    void refreshDeviceList();
    void connectToDevice();
    void disconnectFromDevice();
    void populateFields();
    void searchLogFile();
    void clearOutput();

public slots:
    void btRefreshDeviceClicked();
    void btConnectToDeviceClicked();
    void btDisconnectFromDeviceClicked();
    void btSearchLogFileClicked();
    void btClearOutput();
    void leCommandLineSent();
};

#endif // TERMINAL_H
