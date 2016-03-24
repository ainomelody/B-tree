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
#include <QCheckBox>
#include "thread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QLineEdit *txt_order;
    QPushButton *btn_ok, *btn_add, *btn_del, *btn_search, *btn_rgsearch;
    QPushButton *btn_load, *btn_save;
    QPushButton *btn_cont;
    QLabel *lbl_tip;
    QScrollArea *area;
    paintWidget *paint;
    WorkThread* treeThread;
    QCheckBox *chk_show;
private slots:
    void ok_clicked();
    void add_clicked();
    void del_clicked();
    void load_clicked();
    void save_clicked();
    void search_clicked();
    void rgsearch_clicked();
public:
    void chooseOpr();
    void execOpr();
    void drawTree();
    bool box_checked();
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
