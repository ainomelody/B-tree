#include "mainwindow.h"
#include <QApplication>

MainWindow *wptr;
B_tree<int> *tree;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    wptr = &w;

    w.show();

    return a.exec();
}
