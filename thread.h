#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class WorkThread : public QThread
{
    Q_OBJECT
private:
    int opr, oparg;
    bool running;
public:
    /*type指定执行的操作：0插入，1删除；arg传入要插入、删除的元素，parent自带*/
    WorkThread(int type, int arg, QObject *parent = 0);
    void run();
public slots:
    void change();
};

#endif // THREAD

