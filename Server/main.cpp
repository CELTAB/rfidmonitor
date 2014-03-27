#include <QCoreApplication>

#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPServer server;
    server.start();

    return a.exec();
}
