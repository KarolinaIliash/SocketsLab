#include "client.h"

#include <QDebug>

Client::Client(QListWidget* listClosed, QListWidget* listOpened)
{
    //this->mw = mw;
    //mw = parent;
    this->listClosed = listClosed;
    this->listOpened = listOpened;
    connect(this, SIGNAL(readyRead()), this, SLOT(onServerMessage()));
    connect(this, SIGNAL(connected()), this, SLOT(onConnected()));
}

void Client::onConnect(const QHostAddress &adr, qint16 port)
{
    connectToHost(adr, port);
}

void Client::sendCommand(const QString& command, const qint64 &index)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << (qint64)0;
    out << command;
    out << index;
    out.device()->seek(0);
    out << (qint64)(buffer.size() - sizeof(qint64));
    write(buffer);
    qDebug() << buffer.size() - sizeof(qint64);
}

void Client::sendOpen(const qint64 &index)
{
    QString command = "Open";
    sendCommand(command, index);
}

void Client::sendClose(const qint64 &index)
{
    QString command = "Close";
    sendCommand(command, index);
}

void Client::changeList()
{
    for(int i = 0; i < programs.size(); i++){
        if(programs[i].isOpened){

            addItem(true, programs[i], i);
        }
        else{
            addItem(false, programs[i], i);
        }
    }
}

void Client::ChangeList(QString command,qint64 index)
{
    if(command == "Opened"){
        addItem(true, programs[index], index);
        removeItem(false, programs[index], index);
    }
    else{
        addItem(false, programs[index], index);
        removeItem(true, programs[index], index);
    }
}


void Client::addItem(bool isOpen, Program program, qint64 index)
{
    QListWidgetItem *wi = new QListWidgetItem(program.name);
    QVariant v;
    v.setValue(index);

    wi->setData(Qt::UserRole, v);
    if(isOpen){
         listOpened->addItem(wi);
    }
    else{
        listClosed->addItem(wi);
    }
}

void Client::removeItem(bool isOpen, Program program, qint64 index)
{
    QList<QListWidgetItem*> l;
    QListWidgetItem* it;
    if(isOpen){


        l = listOpened->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row = listOpened->row(it);
            delete listOpened->takeItem(row);
        }
    }
    else{
        l =  listClosed->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row  = listClosed->row(it);
            delete  listClosed->takeItem(row);
        }
    }
}

void Client::onServerMessage()
{
    QDataStream in(this);
    if(bufferSize == 0){
        if(bytesAvailable() < sizeof(qint64)){
            return;
        }
        in >> bufferSize;
    }
    if(bytesAvailable() < bufferSize){
        return;
    }
    bufferSize = 0;
    QString command;
    in >> command;
    if(command == "List"){
        int n;
        in >> n;
        Program p;
        programs.clear();
        for(int i =0; i < n; i++){
            in >> p;
            programs  << p;
        }
        changeList();
    }
    else if(command == "Opened" || command == "Closed"){
        qint64 index;
        in >> index;
        ChangeList(command, index);
    }
}

void Client::onConnected()
{
    qDebug()<<"Connected";

    sendCommand("Send", 0);
}

