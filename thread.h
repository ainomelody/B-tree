#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

extern QWaitCondition wt;

class WorkThread : public QThread
{
    Q_OBJECT
private:
    int opr, oparg;
    bool stopped;
public:
    /*type指定执行的操作：0插入，1删除；arg传入要插入、删除的元素，parent自带*/
    WorkThread(int type, int arg, QObject *parent = 0);
    void run();
public slots:
    void change();
};

void threadPauseAndDraw();

#endif // THREAD

