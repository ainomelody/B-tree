#include "inputdialog.h"
#include <QMessageBox>

oneInputDialog::oneInputDialog(const char *type, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setFixedSize(261, 65);
    setWindowTitle(tr(type));
    isRejected = false;

    lbl_msg = new QLabel(this);
    lbl_msg->setGeometry(80, 5, 111, 21);
    lbl_msg->setText(tr("输入要") + tr(type) + tr("的元素"));

    txt_input = new QLineEdit(this);
    txt_input->setGeometry(10, 30, 131, 25);

    btn_ok = new QPushButton(this);
    btn_ok->setGeometry(170, 30, 75, 25);
    btn_ok->setText(tr("确定"));

    connect(this, SIGNAL(rejected()), this, SLOT(setReject()));
    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(ok_clicked()));
}

void oneInputDialog::ok_clicked()
{
    int result;
    bool ok;

    result = txt_input->text().toInt(&ok);
    if (!ok)
    {
        QMessageBox::critical(this, tr("错误"), tr("请输入十进制数字！"), QMessageBox::Ok);
        return;
    }
    done(result);
}

void oneInputDialog::setReject()
{
    isRejected = true;
}

rangeSearchInputDialog::rangeSearchInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setFixedSize(338, 160);
    setWindowTitle(tr("区间查找"));

    lbl_msg = new QLabel(this);
    lbl_msg->setGeometry(120, 10, 91, 20);
    lbl_msg->setText(tr("输入上限和下限"));

    lbl_LBound = new QLabel(this);
    lbl_LBound->setGeometry(30, 40, 41, 16);
    lbl_LBound->setText(tr("下限"));

    txt_LBound = new QLineEdit(this);
    txt_LBound->setGeometry(90, 40, 201, 20);

    lbl_UBound = new QLabel(this);
    lbl_UBound->setGeometry(30, 80, 31, 16);
    lbl_UBound->setText(tr("上限"));

    txt_UBound = new QLineEdit(this);
    txt_UBound->setGeometry(90, 80, 201, 20);

    btn_ok = new QPushButton(this);
    btn_ok->setGeometry(130, 120, 75, 23);
    btn_ok->setText(tr("确定"));

    connect(btn_ok, SIGNAL(clicked(bool)), this, SLOT(ok_clicked()));
}

void rangeSearchInputDialog::ok_clicked()
{
    int convLBound, convUBound;
    bool ok;

    convLBound = txt_LBound->text().toInt(&ok);
    if (!ok)
    {
        QMessageBox::critical(this, tr("错误"), tr("上限请输入数字"), QMessageBox::Ok);
        return;
    }

    convUBound = txt_UBound->text().toInt(&ok);
    if (!ok)
    {
        QMessageBox::critical(this, tr("错误"), tr("下限请输入数字"), QMessageBox::Ok);
        return;
    }

    if (convLBound > convUBound)
    {
        QMessageBox::critical(this, tr("错误"), tr("上限小于下限"), QMessageBox::Ok);
        return;
    }

    LBound = convLBound;
    UBound = convUBound;
    done(QDialog::Accepted);
}
