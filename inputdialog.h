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
public:
    oneInputDialog(const char *type, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~oneInputDialog();
};

#endif

