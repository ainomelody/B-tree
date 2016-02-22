#include "paintwidget.h"
#include "mainwindow.h"
#include <stack>
#include <cmath>
#include <QString>
#include <QDebug>
extern MainWindow *wptr;
extern B_tree<int> *tree;

long long  power(int x, int y)
{
    long long result = 1;

    for (int i = 0; i < y; i++)
        result *= x;
    return result;
}

paintWidget::paintWidget(QWidget *parent) : QWidget(parent)
{
    pointSize = 12;
    nodeHeight = 15;
    pxPerNum = 10;
    pxBetweenLevel = 20;
    maxNodeWidth = pxPerNum * 8 * tree->n;
    pxBetweenBtNode = 10;
}


void paintWidget::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    std::stack<B_tree<int>::BTNode *> stk;
    B_tree<int>::BTNode *lpNode, *lpLastNode = NULL;

    paint.setPen(Qt::black);
    paint.setBrush(palette().background());    //设置绘图的背景色与控件背景色相同
    bottomX = pxBetweenBtNode;

    setGeometry(0, 0, (pxBetweenBtNode + maxNodeWidth) *
                power(tree->n, tree->getLevelNum() - 1) + pxBetweenBtNode,
                (nodeHeight + pxBetweenLevel) * tree->getLevelNum() + pxBetweenLevel);

    if (tree->root->keynum == 0 && tree->root->ptr[0] == NULL)
        return;
    /*后序遍历，绘图*/
    stk.push(tree->root);
    while (!stk.empty())
    {
        lpNode = stk.top();
        if (lpNode->ptr[0] != NULL && lpLastNode != lpNode->ptr[lpNode->keynum])        //不是最底层结点，且子结点未被处理
            for (int i = lpNode->keynum; i >= 0; i--)
                stk.push(lpNode->ptr[i]);
        else
        {
            drawNode(lpNode, &paint);
            lpLastNode = lpNode;
            stk.pop();
        }
    }
}

int paintWidget::nodeWidth(B_tree<int>::BTNode *node)
{
    int result = 0;

    for (int i = 1; i <= node->keynum; i++)
        result += numWidth(node->key[i]);

    if (result == 0)            //空结点，删除过程中出现
        return pxPerNum;

    return result;
}

int paintWidget::numWidth(int num)
{
    QString str;

    str.setNum(num);
    return str.length() * pxPerNum;
}

void paintWidget::drawNode(B_tree<int>::BTNode *node, QPainter *painter)
{
    int width = nodeWidth(node);
    int level = tree->nodeLevel(node);
    int y = level * (pxBetweenLevel + nodeHeight) - nodeHeight;
    int childY = y + pxBetweenLevel + nodeHeight;

    if (node->ptr[0] == NULL)                                   //最底层结点
    {
        node->X = bottomX;
        bottomX += width + pxBetweenBtNode;
        if (node->parent && node->parent->keynum == 0)  //父结点只有一个子结点
            bottomX += width + pxBetweenBtNode;
    }
    else
    {
        if (node->keynum == 0)
            node->X = node->ptr[0]->X + nodeWidth(node->ptr[0]) - (width - pxBetweenBtNode) / 2;
        else
            node->X = (node->ptr[0]->X + node->ptr[node->keynum]->X +
                       nodeWidth(node->ptr[node->keynum]) - width) / 2;
    }

    painter->drawRect(node->X, y, width, nodeHeight);           //结点边界，矩形

    int x = node->X;                                            //显示数字用，左边界横坐标
    for (int i = 0; i <= node->keynum; i++)
    {
        if (i != 0)
        {
            int nwidth = numWidth(node->key[i]);                //数字宽度
            painter->drawText(x, y, nwidth, nodeHeight,
                              Qt::AlignCenter, QString().setNum(node->key[i])); //显示数字
            x += nwidth;                                        //改动横坐标
            if (i < node->keynum)                               //绘制分割线
                painter->drawLine(x, y, x, y + nodeHeight);
        }
        if (node->ptr[i] != NULL)
            painter->drawLine(x, y + nodeHeight,
                              node->ptr[i]->X + nodeWidth(node->ptr[i]) / 2, childY);
    }
}
