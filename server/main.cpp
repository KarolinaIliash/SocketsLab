#include <QApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server;
    server.open(3000);
    qDebug()<<server.serverAddress() << "  "<<server.serverPort();
    return a.exec();
}
