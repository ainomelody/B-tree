#include "mainwindow.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPainter>
#include "windows.h"
#include <QDebug>
#include "inputdialog.h"
#include <QFileDialog>
#include <queue>
#include <QFile>

extern B_tree<int> *tree;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /*设置阶数的UI*/

    setWindowTitle(tr("B树演示程序"));
    setFixedSize(500,200);

    lbl_tip = new QLabel(this);
    lbl_tip->setGeometry(185, 45, 100, 20);
    lbl_tip->setText(tr("设置B树阶数"));

    txt_order = new QLineEdit(this);
    txt_order->setGeometry(120, 80, 200, 30);
    btn_ok = new QPushButton(this);
    btn_ok->setGeometry(330, 80, 50,30);
    btn_ok->setText(tr("确定"));
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(ok_clicked()));

}

MainWindow::~MainWindow()
{
    if (tree != NULL)
    {
        tree->getThread()->change();
        delete tree;
    }
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

    QDesktopWidget desktop;
    QRect rc = desktop.availableGeometry();

    /*获取任务栏高度*/
    RECT rect;
    HWND hWnd = FindWindowW(TEXT("Shell_TrayWnd"), NULL);
    GetWindowRect(hWnd, &rect);

    setFixedSize(rc.width(), rc.height() - rect.bottom + rect.top);

    move(-7, 0);

    lbl_tip->setText(tr("选择要执行的操作"));
    lbl_tip->setGeometry(width() / 2 -50, 5, 100, 15);

    int space = (width() - 60 * 7) / 8;

    btn_add = new QPushButton(this);
    btn_add->setGeometry(space, 25, 60, 30);
    btn_add->setText(tr("添加"));
    btn_add->show();

    btn_del = new QPushButton(this);
    btn_del->setGeometry(space * 2 + 60, 25, 60, 30);
    btn_del->setText(tr("删除"));
    btn_del->show();

    btn_search = new QPushButton(this);
    btn_search->setGeometry(space * 3 + 120, 25, 60, 30);
    btn_search->setText(tr("查找"));
    btn_search->show();

    btn_rgsearch = new QPushButton(this);
    btn_rgsearch->setGeometry(space * 4 + 180, 25, 60, 30);
    btn_rgsearch->setText(tr("区间查找"));
    btn_rgsearch->show();

    btn_load = new QPushButton(this);
    btn_load->setGeometry(space * 5 + 240, 25, 60, 30);
    btn_load->setText(tr("加载"));
    btn_load->show();

    btn_save = new QPushButton(this);
    btn_save->setGeometry(space * 6 + 300, 25, 60, 30);
    btn_save->setText(tr("保存"));
    btn_save->show();

    btn_cont = new QPushButton(this);
    btn_cont->setText(tr("继续"));
    btn_cont->setGeometry(space * 7 + 360, 25, 60, 30);
    btn_cont->show();
    btn_cont->setDisabled(true);

    treeThread = tree->getThread();


    chk_show = new QCheckBox(this);
    chk_show->setText(tr("显示操作时间"));
    chk_show->setCheckState(Qt::Unchecked);
    chk_show->setGeometry(lbl_tip->x(), 60, 90, 20);
    chk_show->show();

    area = new QScrollArea(this);
    area->setGeometry(0, 90, width(), height() - 90);
    area->setMaximumSize(10000, 5000);
    area->show();

    paint = new paintWidget(this);
    area->setWidget(paint);
    paint->setGeometry(0, 0, width(), height());
    paint->show();

    connect(btn_add, SIGNAL(clicked(bool)), this, SLOT(add_clicked()));
    connect(btn_del, SIGNAL(clicked(bool)), this, SLOT(del_clicked()));
    connect(btn_save, SIGNAL(clicked(bool)), this, SLOT(save_clicked()));
    connect(btn_load, SIGNAL(clicked(bool)), this, SLOT(load_clicked()));
    connect(btn_search, SIGNAL(clicked(bool)), this, SLOT(search_clicked()));
    connect(btn_cont, SIGNAL(clicked(bool)), treeThread, SLOT(change()));
    connect(btn_rgsearch, SIGNAL(clicked(bool)), this, SLOT(rgsearch_clicked()));
}

void MainWindow::execOpr()
{
    btn_add->setDisabled(true);
    btn_del->setDisabled(true);
    btn_search->setDisabled(true);
    btn_rgsearch->setDisabled(true);
    btn_load->setDisabled(true);
    btn_save->setDisabled(true);
    chk_show->setDisabled(true);

    btn_cont->setEnabled(true);
}

void MainWindow::chooseOpr()
{
    btn_add->setEnabled(true);
    btn_del->setEnabled(true);
    btn_search->setEnabled(true);
    btn_rgsearch->setEnabled(true);
    btn_load->setEnabled(true);
    btn_save->setEnabled(true);
    chk_show->setEnabled(true);

    paint->removeMark();
    btn_cont->clearFocus();
    btn_cont->setDisabled(true);
}

void MainWindow::add_clicked()
{
    int toAdd;
    oneInputDialog *add_dlg = new oneInputDialog("添加", this);

    toAdd = add_dlg->exec();
    bool isReject = add_dlg->isRejected;
    delete add_dlg;
    if (isReject)
        return;
    execOpr();

    WorkThread *addThread = new WorkThread(0, toAdd);
    connect(addThread, SIGNAL(finished()), addThread, SLOT(deleteLater()));
    addThread->run();
}

void MainWindow::del_clicked()
{
    int toDel;
    oneInputDialog *del_dlg = new oneInputDialog("删除", this);

    toDel = del_dlg->exec();
    bool isReject = del_dlg->isRejected;
    delete del_dlg;

    if (isReject)
        return;

    execOpr();

    WorkThread *delThread = new WorkThread(1, toDel);
    connect(delThread, SIGNAL(finished()), delThread, SLOT(deleteLater()));
    delThread->run();
}

void MainWindow::search_clicked()
{
    int toSearch;
    oneInputDialog *sch_dlg = new oneInputDialog("查找", this);

    toSearch = sch_dlg->exec();
    bool isReject = sch_dlg->isRejected;
    delete sch_dlg;

    if (isReject)
        return;
    paint->markNum(toSearch);

    execOpr();
    treeThread->run();
    chooseOpr();
}

void MainWindow::drawTree()
{
    btn_cont->setDisabled(true);
    paint->repaint();
    btn_cont->setEnabled(true);
}

void MainWindow::save_clicked()
{
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), tr(""),
                                            tr("所有文件(*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("错误"), tr("无法写入文件 ") +
                              fileName, QMessageBox::Ok);
        return;
    }
    tree->saveToFile(&file);

    QMessageBox::information(this, tr("提示"), tr("保存完成"), QMessageBox::Ok);

    if (box_checked())
        QMessageBox::information(this, tr("提示"), tr("运行时间为") +
                                 QString::number(tree->getTime())+ tr("ms"),
                                 QMessageBox::Ok);
}

void MainWindow::load_clicked()
{
    QString fileName;
    int n;

    fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), tr(""),
                                            tr("所有文件(*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("错误"), tr("无法读取文件 ") +
                              fileName, QMessageBox::Ok);
        return;
    }

    if (!tree->isEmpty())
        if (QMessageBox::question(this, tr("提示"), tr("是否销毁当前的树并从文件中加载？"),
                                  QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
        {
            QMessageBox::warning(this, tr("提示"), tr("操作终止"), QMessageBox::Ok);
            return;
        }

    QTime count;
    int time;

    count.start();
    disconnect(btn_cont, SIGNAL(clicked(bool)), treeThread, SLOT(change()));

    file.read((char *)&n, sizeof(int));
    delete tree;
    tree = new B_tree<int>(n);
    tree->loadFromFile(&file);
    treeThread = tree->getThread();
    connect(btn_cont, SIGNAL(clicked(bool)), treeThread, SLOT(change()));
    time = count.elapsed();

    QMessageBox::information(this, tr("提示"), tr("加载完成！树的阶数为 ") + QString::number(n), QMessageBox::Ok);

    if (box_checked())
        QMessageBox::information(this, tr("提示"), tr("运行时间为") + QString::number(time) + tr("ms"),
                                 QMessageBox::Ok);
}

bool MainWindow::box_checked()
{
    return (chk_show->isChecked());
}

void MainWindow::rgsearch_clicked()
{
    rangeSearchInputDialog *sch_dlg = new rangeSearchInputDialog(this);
    int result;

    result = sch_dlg->exec();
    if (result == QDialog::Rejected)
    {
        delete sch_dlg;
        return;
    }

    std::set<int> *range = tree->rangeSerach(sch_dlg->LBound, sch_dlg->UBound);
    //range由paint后续管理并释放
    paint->rangeMark(range);

    QMessageBox::information(this, tr("提示"), tr("找到") + QString::number(range->size())
                             + tr("个元素"), QMessageBox::Ok);

    if (box_checked())
        QMessageBox::information(this, tr("提示"), tr("运行时间为") + QString::number(tree->getTime())
                                 + tr("ms"), QMessageBox::Ok);
    delete sch_dlg;

    execOpr();
    treeThread->run();
    chooseOpr();
}
