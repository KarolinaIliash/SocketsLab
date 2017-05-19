#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new Client(ui->listForOpen, ui->listOpened);
    client->connectToHost(QHostAddress("127.0.0.1"), 3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_listForOpen_doubleClicked(const QModelIndex &index)
{
    int i = ui->listForOpen->currentRow();
    QListWidgetItem* it;
    it = ui->listForOpen->item(i);

    QVariant v = it->data(Qt::UserRole);
    int ind = v.value<qint64>();

    client->sendOpen(ind);
}


void MainWindow::on_listOpened_doubleClicked(const QModelIndex &index)
{
    int i = ui->listOpened->currentRow();
    QListWidgetItem* it = ui->listOpened->takeItem(i);


    QVariant v = it->data(Qt::UserRole);
    int ind = v.value<qint64>();

    client->sendClose(ind);
}

