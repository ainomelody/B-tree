#include "thread.h"
#include "Btree.h"
#include <QMessageBox>
#include "mainwindow.h"

extern B_tree<int> *tree;
extern MainWindow *wptr;
QWaitCondition wt;

WorkThread::WorkThread(int type, int arg, QObject *parent) : QThread(parent)
{
    opr = type;
    oparg = arg;

    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void WorkThread::run()
{
    QString argstr;

    argstr.setNum(oparg);
    if (opr)
    {
        if (!tree->deleteItem(oparg))
            QMessageBox::critical(wptr, tr("错误"), tr("树中不存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    else
    {
        if (!tree->insertItem(oparg))
            QMessageBox::critical(wptr, tr("错误"), tr("树中已存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    wptr->chooseOpr();
}

void threadPauseAndDraw()
{
    QMutex mute;

    wptr->drawTree();
    mute.lock();
    wt.wait(&mute);
    mute.unlock();
}
