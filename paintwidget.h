#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QPainter>

class paintWidget : public QWidget
{
    Q_OBJECT

private:
    void paintEvent(QPaintEvent *);
public:
    explicit paintWidget(QWidget *parent = 0);
};

#endif // PAINTWIDGET_H
