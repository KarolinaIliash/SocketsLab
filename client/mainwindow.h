#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QListWidget>
#include<QListWidgetItem>
#include<QList>

#include"client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Client* client;
public:
private slots:
    void on_listForOpen_doubleClicked(const QModelIndex &index);
    void on_listOpened_doubleClicked(const QModelIndex &index);
};

#endif // MAINWINDOW_H
