#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QPainter>
#include "Btree.h"

template <typename Item>
class B_tree;

class paintWidget : public QWidget
{
    Q_OBJECT

private:
    int pxPerNum;           //一个数字的宽度
    int nodeHeight;         //结点的高度
    int pxBetweenLevel;     //两层之间的间距
    int pointSize;          //显示数字的大小
    int pxBetweenBtNode;    //最底层结点之间的距离
    int maxNodeWidth;       //结点的最大宽度
    int bottomX;            //最底层结点的横坐标

    void paintEvent(QPaintEvent *);
    void drawNode(B_tree<int>::BTNode *node, QPainter *painter);
    int nodeWidth(B_tree<int>::BTNode *node);   //计算一个结点的宽度
    int numWidth(int num);                      //一个数的宽度
    int levelSpace(int level);                  //返回由level指定的层的空隙宽
public:
    explicit paintWidget(QWidget *parent = 0);
};

#endif // PAINTWIDGET_H
