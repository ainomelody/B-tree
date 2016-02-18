#include "inputdialog.h"
#include <QMessageBox>

oneInputDialog::oneInputDialog(const char *type, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setFixedSize(261, 65);
    lbl_msg = new QLabel(this);
    lbl_msg->setGeometry(80, 5, 111, 21);
    lbl_msg->setText(tr("输入要") + tr(type) + tr("的元素"));
    txt_input = new QLineEdit(this);
    txt_input->setGeometry(10, 30, 131, 25);
    btn_ok = new QPushButton(this);
    btn_ok->setGeometry(170, 30, 75, 25);
    btn_ok->setText(tr("确定"));
    setWindowTitle(tr(type));

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

oneInputDialog::~oneInputDialog()
{
    delete lbl_msg;
    delete btn_ok;
    delete txt_input;
}
