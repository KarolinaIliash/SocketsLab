#ifndef CLIENT_H
#define CLIENT_H

#include<QtNetwork>
#include<QByteArray>
#include<QDataStream>
#include<QString>
#include<QListWidget>
#include<QListWidgetItem>


class Client: public QTcpSocket
{
    Q_OBJECT
public:
    Client(QListWidget* listClosed, QListWidget* listOpened);
    void onConnect(const QHostAddress &adr, qint16 port);
    void sendCommand(const QString& command, const qint64 &index);
    void sendOpen(const qint64& index);
    void sendClose(const qint64& index);
    void changeList();
    void ChangeList(QString command, qint64 index);
private slots:
    void onServerMessage();
    void onConnected();
public:
    struct Program{
        QString name;
        bool isOpened;
        friend QDataStream &operator>>(QDataStream &in, Program &p){
            in >> p.name >> p.isOpened;
            return in;
        }
        friend QDataStream &operator<<(QDataStream &out, const Program &p){
            out << p.name << p.isOpened;
            return out;
        }
    };

private:

    QVector<Program> programs;

    qint64 bufferSize = 0;
    QListWidget* listClosed;
    QListWidget* listOpened;

    void addItem(bool isOpen, Program program, qint64 index);
    void removeItem(bool isOpen, Program program, qint64 index);
};

#endif // CLIENT_H
