#include "mainwindow.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPainter>
#include "windows.h"
#include <QDebug>
#include "inputdialog.h"
#include "thread.h"

extern B_tree<int> *tree;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /*设置阶数的UI*/

    setWindowTitle(tr("B树演示程序"));
    setFixedSize(500,200);

    lbl_tip = new QLabel(this);
    lbl_tip->setGeometry(185, 50, 100, 20);
    lbl_tip->setText(tr("设置B树阶数"));

    txt_order = new QLineEdit(this);
    txt_order->setGeometry(120, 85, 200, 30);
    btn_ok = new QPushButton(this);
    btn_ok->setGeometry(330, 85, 50,30);
    btn_ok->setText(tr("确定"));
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(ok_clicked()));

}

MainWindow::~MainWindow()
{
    if (tree != NULL)
        delete tree;
}

void MainWindow::ok_clicked()
{
    int n;
    bool ok;


    n = txt_order->text().toInt(&ok);
    if (!ok || n < 2)
    {
        QMessageBox::critical(this, tr("错误"), tr("阶数设置不正确，请使用大于等于2的十进制数字"), QMessageBox::Ok);
        return;
    }
    /*基本操作的UI*/
    tree = new B_tree<int>(n);
    delete btn_ok;
    delete txt_order;
    btn_ok = NULL;
    txt_order = NULL;

    lbl_tip->setText(tr("选择要执行的操作"));
    btn_add = new QPushButton(this);
    btn_add->setGeometry(52, 85, 60, 30);
    btn_add->setText(tr("添加"));
    btn_add->show();

    btn_del = new QPushButton(this);
    btn_del->setGeometry(162, 85, 60, 30);
    btn_del->setText(tr("删除"));
    btn_del->show();

    btn_search = new QPushButton(this);
    btn_search->setGeometry(274, 85, 60, 30);
    btn_search->setText(tr("查找"));
    btn_search->show();

    btn_rgsearch = new QPushButton(this);
    btn_rgsearch->setGeometry(386, 85, 60, 30);
    btn_rgsearch->setText(tr("区间查找"));
    btn_rgsearch->show();

    btn_cont = new QPushButton(this);
    btn_cont->setText(tr("继续"));


    minPoint = pos();
    area = new QScrollArea(this);
    area->hide();
    paint = new paintWidget(this);
    area->setWidget(paint);
    paint->hide();
    area->setAlignment(Qt::AlignCenter);                            //居中显示
    connect(btn_add, SIGNAL(clicked(bool)), this, SLOT(add_clicked()));
    connect(btn_del, SIGNAL(clicked(bool)), this, SLOT(del_clicked()));
    connect(btn_cont, SIGNAL(clicked(bool)), this, SLOT(cont_clicked()));
}

void MainWindow::execOpr()
{
    btn_add->hide();
    btn_del->hide();
    btn_search->hide();
    btn_rgsearch->hide();

    btn_cont->show();
    lbl_tip->hide();

    QDesktopWidget desktop;
    QRect rc = desktop.availableGeometry();

    /*获取任务栏高度*/
    RECT rect;
    HWND hWnd = FindWindowW(TEXT("Shell_TrayWnd"), NULL);
    GetWindowRect(hWnd, &rect);

    setFixedSize(rc.width(), rc.height() - rect.bottom + rect.top);
    move(-7, 0);
    btn_cont->setGeometry(rc.width() / 2 - 35,0, 70, 30);
    area->setGeometry(0, 50, width(), height() - 50);
    paint->setGeometry(0, 0, width(), height());
    area->show();
    paint->show();
    repaint();
}

void MainWindow::chooseOpr()
{
    btn_add->show();
    btn_del->show();
    btn_search->show();
    btn_rgsearch->show();
    btn_cont->hide();
    paint->hide();
    area->hide();
    lbl_tip->show();


    setFixedSize(500, 200);
    move(minPoint);
}

void MainWindow::add_clicked()
{
    int toAdd;
    oneInputDialog *add_dlg = new oneInputDialog("添加", this);

    toAdd = add_dlg->exec();
    delete add_dlg;
    execOpr();

    WorkThread *addThread = new WorkThread(0, toAdd);
    addThread->run();
}

void MainWindow::del_clicked()
{
    int toDel;
    oneInputDialog *del_dlg = new oneInputDialog("删除", this);

    toDel = del_dlg->exec();
    delete del_dlg;
    execOpr();

    WorkThread *delThread = new WorkThread(1, toDel);
    delThread->run();
}

void MainWindow::drawTree()
{
    btn_cont->setDisabled(true);
    paint->repaint();
    btn_cont->setEnabled(true);
}

void MainWindow::cont_clicked()
{
    wt.wakeAll();
}
