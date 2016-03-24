#include "thread.h"
#include "Btree.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QCoreApplication>

extern B_tree<int> *tree;
extern MainWindow *wptr;

WorkThread::WorkThread(int type, int arg, QObject *parent) : QThread(parent)
{
    opr = type;
    oparg = arg;
    running = true;
//    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void WorkThread::change()
{
    running = false;
}

void WorkThread::run()
{
    QString argstr;
    argstr.setNum(oparg);
    if (opr == 1)
    {
        if (tree->deleteItem(oparg))
        {
            if (wptr->box_checked())
                QMessageBox::information(wptr, tr("提示"), tr("运行时间为") +
                                         QString::number(tree->getTime()) + tr("ms"),
                                         QMessageBox::Ok);
        }
        else
            QMessageBox::critical(wptr, tr("错误"), tr("树中不存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    else if (opr == 0)
    {
        if (tree->insertItem(oparg))
        {
            if (wptr->box_checked())
                QMessageBox::information(wptr, tr("提示"), tr("运行时间为") +
                                         QString::number(tree->getTime()) + tr("ms"),
                                         QMessageBox::Ok);
        }
        else
            QMessageBox::critical(wptr, tr("错误"), tr("树中已存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    else
    {
        wptr->drawTree();
        while(running && wptr->isVisible())
        {
            QCoreApplication::processEvents();
        }
    }
    if(opr != 5)
        wptr->chooseOpr();
    running = true;
}
