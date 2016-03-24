#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class oneInputDialog : public QDialog
{
    Q_OBJECT
private:
    QLabel *lbl_msg;
    QLineEdit *txt_input;
    QPushButton *btn_ok;
private slots:
    void ok_clicked();
    void setReject();
public:
    bool isRejected;
    oneInputDialog(const char *type, QWidget *parent = 0, Qt::WindowFlags f = 0);
};

class rangeSearchInputDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *lbl_LBound, *lbl_UBound, *lbl_msg;
    QLineEdit *txt_LBound, *txt_UBound;
    QPushButton *btn_ok;
private slots:
    void ok_clicked();
public:
    int LBound, UBound;
    rangeSearchInputDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
};

#endif

