#include "client.h"

#include <QDebug>

Client::Client(QListWidget* listForOpen, QListWidget* listOpened)
{
    this->listForOpen = listForOpen;
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
    qDebug() << buffer.size() - sizeof(qint64);
    write(buffer);

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

void Client::ChangeList()
{
    for(int i = 0; i < programs.size(); i++){
            addItem(true, programs[i], i);
            if(programs[i].isOpened)
                addItem(false, programs[i], i);
    }
}

void Client::ChangeList(QString command, qint64 index)
{
        if(command == "Opened"){
            addItem(false, programs[index], index);
        }
        else{
            removeItem(false, programs[index], index);
        }
}


void Client::addItem(bool isForOpen, Program program, qint64 index)
{
    QListWidgetItem *wi = new QListWidgetItem(program.name);
    QVariant v;
    v.setValue(index);

    wi->setData(Qt::UserRole, v);
    if(isForOpen){
        listForOpen->addItem(wi);
    }
    else{
        listOpened->addItem(wi);
    }
}

void Client::removeItem(bool isForOpen, Program program, qint64 index)
{
    QList<QListWidgetItem*> l;
    QListWidgetItem* it;
    if(!isForOpen){
        l = listOpened->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row = listOpened->row(it);
            delete listOpened->takeItem(row);
        }
    }
    else{
        l =  listForOpen->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row  = listForOpen->row(it);
            delete  listForOpen->takeItem(row);
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
        ChangeList();
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

