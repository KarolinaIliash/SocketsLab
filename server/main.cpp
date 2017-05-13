#include "mainwindow.h"
#include <QApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Server* server = new Server();
    server->open(3000);
    qDebug()<<server->serverAddress() << "  "<<server->serverPort();
    w.show();

    return a.exec();
}
