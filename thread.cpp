#include "thread.h"
#include "Btree.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QDebug>
#include <QMutexLocker>
#include <QCoreApplication>
extern B_tree<int> *tree;
extern MainWindow *wptr;
QWaitCondition wt;
QMutex mute;

WorkThread::WorkThread(int type, int arg, QObject *parent) : QThread(parent)
{
    opr = type;
    oparg = arg;
    stopped = true;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void WorkThread::change()
{
    stopped = false;
}

void WorkThread::run()
{
    QString argstr;
    argstr.setNum(oparg);
    if (opr)
    {
        if(oparg == 0)
        {
            wptr->drawTree();
            while(stopped)
            {
                QCoreApplication::processEvents();
            }
        }
        else if (!tree->deleteItem(oparg))
            QMessageBox::critical(wptr, tr("错误"), tr("树中不存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    else
    {
        if (!tree->insertItem(oparg))
            QMessageBox::critical(wptr, tr("错误"), tr("树中已存在元素") + argstr,
                                  QMessageBox::Ok);
    }
    if(opr != 5)
        wptr->chooseOpr();
    stopped = true;


}

void threadPauseAndDraw()
{
    wptr->drawTree();
    //QMutexLocker locker(&mute);
   // mute.lock();
    //qDebug() << "s1";
    //wt.wait(&mute);
    //QThread::sleep(10);
    //qDebug() << "s2";
   // wt.wakeAll();
    while(1){
        QCoreApplication::processEvents();
    }
}
