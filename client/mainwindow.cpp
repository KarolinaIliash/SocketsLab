#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(ui->listClosed, ui->listOpened);
    client->connectToHost(QHostAddress("192.168.1.105"), 3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*void MainWindow::addItem(bool isOpen, Client::Program program, int index)
{
    QListWidgetItem *wi = new QListWidgetItem(program.name);
    QVariant v;
    v.setValue(index);

    wi->setData(Qt::UserRole, v);
    if(isOpen){
        ui->listOpened->addItem(wi);
    }
    else{
        ui->listClosed->addItem(wi);
    }
}

void MainWindow::removeItem(bool isOpen, Client::Program program, int index)
{
    QList<QListWidgetItem*> l;
    QListWidgetItem* it;
    if(isOpen){
        l = ui->listOpened->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row = ui->listOpened->row(it);
            delete ui->listOpened->takeItem(row);
        }
    }
    else{
        l = ui->listClosed->findItems(program.name, Qt::MatchExactly);
        if(l.size() != 0 ){
            it = l.first();
            int row  = ui->listClosed->row(it);
            delete ui->listClosed->takeItem(row);
        }
    }
}*/

void MainWindow::on_listClosed_doubleClicked(const QModelIndex &index)
{
    int i = ui->listClosed->currentRow();
    QListWidgetItem* it = ui->listClosed->takeItem(i);


    QVariant v = it->data(Qt::UserRole);
    int ind = v.value<qint64>();

    client->sendCommand("Open", ind);
}

void MainWindow::on_listOpened_doubleClicked(const QModelIndex &index)
{
    int i = ui->listClosed->currentRow();
    QListWidgetItem* it = ui->listClosed->takeItem(i);


    QVariant v = it->data(Qt::UserRole);
    int ind = v.value<qint64>();

    client->sendCommand("Close", ind);
}
