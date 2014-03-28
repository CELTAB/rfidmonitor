#include <QApplication>
#include <QListView>

#include <time.h>
#include <stdlib.h>

#include "gui/mainwindow.h"
#include "gui/devicemodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
