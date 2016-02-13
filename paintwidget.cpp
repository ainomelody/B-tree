#include "paintwidget.h"

paintWidget::paintWidget(QWidget *parent) : QWidget(parent)
{

}


void paintWidget::paintEvent(QPaintEvent *)
{
    QPainter paint;

    paint.begin(this);
    paint.drawLine(0, 0, 1000, 1000);
    paint.end();
}

