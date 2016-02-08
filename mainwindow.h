#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "Btree.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QLineEdit *txt_order;
    QPushButton *btn_ok, *btn_add, *btn_del, *btn_search, *btn_rgsearch;
    QLabel *lbl_tip;
    B_tree<int> *tree;

    void paintEvent(QPaintEvent *);
private slots:
    void ok_clicked();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
