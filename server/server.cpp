#include "server.h"

#include<QDir>
#include<QProcess>

Server::Server()
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    Program prog;/*("C:\\Program Files\\Notepad++", "notepad++");
    programs.push_back(prog);*/
    prog.address = "C:\\Program Files\\Internet Explorer";
    prog.name = "iexplore";
    programs.push_back(prog);
    prog.address = "C:\\Program Files\\Android\\Android Studio\\bin";
    prog.name = "studio";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Mozilla Firefox";
    prog.name = "firefox";
    programs.push_back(prog);
}

void Server::open(qint16 port)
{
    listen(QHostAddress::Any, port);
}

void Server::onNewConnection()
{
    qDebug() << "New Connection";
    QTcpSocket* soc = nextPendingConnection();
    connect(soc, SIGNAL(readyRead()), this, SLOT(onClientMessage()));
    connect(soc, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

    clients << soc;
}

void Server::onClientMessage(){
    qDebug()<<"Message";
    QTcpSocket* soc = qobject_cast<QTcpSocket*>(sender());
    if(soc == nullptr) return;
    QDataStream in(soc);
    if(bufferSize == 0){
        qDebug()<<sizeof(qint64);
        if(soc->bytesAvailable() < sizeof(qint64)){
            return;
        }
        in >> bufferSize;
    }
    qDebug() << soc->bytesAvailable();
    if(soc->bytesAvailable() < bufferSize){
        return;
    }
    bufferSize = 0;
    QString command;
    in >> command;
    if(command == "Send"){
        SendList(soc);
    }
    else if(command == "Open"){
        qint64 index;
        in >> index;
        OpenProgram(index); //TODO
        SendInfo("Opened", index);

    }
    else if(command == "Close"){
        qint64 index;
        in >> index;
        CloseProgram(index); //TODO
        SendInfo("Closed", index);
    }
}


void Server::SendList(QTcpSocket *soc)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << (qint64)0;
    QString command = "List";
    out<< command;
    out << programs.size();
    for(auto &prog: programs){
        out << prog;
    }
    out.device()->seek(0);
    out << (qint64)(buffer.size() - sizeof(qint64));
    soc->write(buffer);
    qDebug() << buffer.size() - sizeof(qint64);
}

void Server::SendInfo(QString command, qint64 index)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << (qint64)0;
    out << command << index;
    out.device()->seek(0);
    out << (qint64)(buffer.size() - sizeof(qint64));
    for(auto& soc: clients){
        soc->write(buffer);
    }
}

void Server::OpenProgram(qint64 index)
{
    if(programs[index].isOpened == false){
        QProcess process;
        QDir::setCurrent(programs[index].address);
        process.startDetached(programs[index].name);
        programs[index].isOpened = true;
    }
}

void Server::CloseProgram(qint64 index)
{
    if(programs[index].isOpened){
        QProcess process;
        QDir::setCurrent(programs[index].address);
        process.setProgram(programs[index].name);
        process.close();
        programs[index].isOpened = false;
    }
}


void Server::onClientDisconnected(){
    QTcpSocket* soc = qobject_cast<QTcpSocket*>(sender());
    if(soc == nullptr) return;
    clients.removeAll(soc);
    soc->deleteLater();
}

