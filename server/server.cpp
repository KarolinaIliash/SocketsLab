#include "server.h"

#include<QDir>
#include<QProcess>

Server::Server()
{
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    Program prog("notepad++", "C:\\Program Files\\Notepad++", "Notepad++");
    programs.push_back(prog);
    prog.address = "C:\\Program Files\\Internet Explorer";
    prog.name = "iexplore";
    prog.prName = "Interner Explorer";
    programs.push_back(prog);
    prog.address = "C:\\Program Files\\Android\\Android Studio\\bin";
    prog.name = "studio64";
    prog.prName = "Android Studio";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Mozilla Firefox";
    prog.name = "firefox";
    prog.prName = "Mozilla Firefox";
    programs.push_back(prog);
    prog.address = "D:\\Program Files (x86)\\Photoshop";
    prog.name = "Photoshop";
    prog.prName = prog.name;
    programs.push_back(prog);
    prog.address = "D:\\Program Files (x86)\\VideoLAN\\VLC";
    prog.name = "vlc";
    prog.prName = "VideoLan";
    programs.push_back(prog);
    prog.address = "D:\\Program Files (x86)\\Total Commander";
    prog.name = "TOTALCMD64";
    prog.prName = "Total Commander";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Adobe\\Acrobat Reader DC\\Reader";
    prog.name = "AcroRd32";
    prog.prName = "Adobe Reader";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Google\\Chrome\\Application";
    prog.name = "chrome";
    prog.prName = "Google Chrome";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Microsoft Office\\Office12";
    prog.name = "EXCEL";
    prog.prName = "Microsoft Office Excel";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Microsoft Office\\Office12";
    prog.name = "OIS";
    prog.prName = "Microsoft Office Picture Manager";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Microsoft Office\\Office12";
    prog.name = "POWERPNT";
    prog.prName = "Microsoft Office Power Point";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Microsoft Office\\Office12";
    prog.name = "WINWORD";
    prog.prName = "Microsoft Office Word";
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\Skype\\Phone";
    prog.name = "Skype";
    prog.prName = prog.name;
    programs.push_back(prog);
    prog.address = "C:\\Program Files (x86)\\TeXstudio";
    prog.name = "texstudio";
    prog.prName = "Tex Studio";
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
    bufsize << qint64(0);
}

void Server::onClientMessage(){
    qDebug()<<"Message";
    QTcpSocket* soc = qobject_cast<QTcpSocket*>(sender());
    if(soc == nullptr) return;
    int ind = clients.indexOf(soc);
    QDataStream in(soc);
    if(bufsize[ind] == 0){
        if(soc->bytesAvailable() < sizeof(qint64)){
            return;
        }
        in >> bufsize[ind];
    }
    qDebug() << soc->bytesAvailable();
    qDebug() << in.device()->size();
    if(soc->bytesAvailable() < bufsize[ind]){
        return;
    }
    bufsize[ind] = 0;
    QString command;
    in >> command;
    qint64 index;
    in >> index;
    if(command == "Send"){
        SendList(soc);
    }
    else if(command == "Open"){
        if(!programs[index].isOpened){
        OpenProgram(index);
        SendInfo("Opened", index);}

    }
   else if(command == "Close"){
        CloseProgram(index);
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
    if(!programs[index].isOpened){
    if(programs[index].proc == nullptr){
        QProcess* process = new QProcess;
        programs[index].proc = process;
    }
    QDir::setCurrent(programs[index].address);
    programs[index].proc->start(programs[index].name);
    programs[index].isOpened = true;}
}

void Server::CloseProgram(qint64 index)
{
    if(programs[index].isOpened){
        programs[index].proc->kill();
        programs[index].isOpened = false;
        delete programs[index].proc;
        programs[index].proc = nullptr;
    }
    programs[index].isOpened = false;
}


void Server::onClientDisconnected(){
    QTcpSocket* soc = qobject_cast<QTcpSocket*>(sender());
    if(soc == nullptr) return;
    int ind = clients.indexOf(soc);
    clients.removeAll(soc);
    bufsize.remove(ind);
    soc->deleteLater();
}

