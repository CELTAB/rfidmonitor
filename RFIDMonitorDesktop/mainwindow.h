#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "terminal/terminalwindow.h"
#include "rasp/configraspwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TerminalWindow *terminalWindow;
    ConfigRaspWindow *configRaspWindow;


public slots:
    void actionOpenTerminalClicked(bool);
    void actionConfigureRaspClicked(bool);
};

#endif // MAINWINDOW_H
