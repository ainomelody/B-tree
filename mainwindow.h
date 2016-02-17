#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "Btree.h"
#include <QPoint>
#include "paintwidget.h"
#include <QScrollArea>


class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QLineEdit *txt_order;
    QPushButton *btn_ok, *btn_add, *btn_del, *btn_search, *btn_rgsearch;
    QPushButton *btn_cont;
    QLabel *lbl_tip;
    QPoint minPoint;
    QScrollArea *area;
    paintWidget *paint;
private slots:
    void ok_clicked();
    void add_clicked();
    void del_clicked();
    void search_clicked();
public slots:
    void chooseOpr();
    void execOpr();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
