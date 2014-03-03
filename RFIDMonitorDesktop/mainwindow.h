#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "terminalwindow.h"

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

public slots:
    void btConfigReaderClicked(bool checked);
};

#endif // MAINWINDOW_H
