#ifndef SERVER_H
#define SERVER_H
#include <QtNetwork>
#include <QDataStream>
#include <QByteArray>
#include <QVector>
#include<QDebug>

class Server: public QTcpServer
{
    Q_OBJECT
private:
    QList<QTcpSocket*> clients;
public:
    Server();
    void open(qint16 port);
private slots:
    void onNewConnection();
    void onClientMessage();
    void onClientDisconnected();
private:
    struct Program{
        QString name;
        QString address;
        bool isOpened = false;

        Program(){}

        Program(QString pname, QString paddress, bool pisOpened = false){
            name = pname;
            address = paddress;
            isOpened = pisOpened;
        }

        friend QDataStream &operator<<(QDataStream &out, const Program &p){
            out << p.name << p.isOpened;
            return out;
        }
    };

    QVector<Program> programs;

    qint64 bufferSize = 0;
private:
    void SendList(QTcpSocket* soc);
    void SendInfo(QString name, qint64 index);
    void OpenProgram(qint64 index);
    void CloseProgram(qint64 index);
};

#endif // SERVER_H
