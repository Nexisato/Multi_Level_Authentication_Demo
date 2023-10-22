#include "login.h"

#include "./ui_login.h"

//登陆界面
Login::Login(QWidget *parent) : QWidget(parent), ui(new Ui::Login) {
    ui->setupUi(this);
    // set front size
    this->setMinimumSize(470, 200);
    this->setMaximumSize(470, 200);

    // connect enums and slots

    connect(ui->btn_login, &QPushButton::clicked, this,
            &Login::btn_login_clicked);
    connect(ui->btn_cancel, &QPushButton::clicked, this,
            &Login::btn_cancel_clicked);
}

Login::~Login() { delete ui; }

//回车键登录
void Login::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Return:
            btn_login_clicked();
            break;
        case Qt::Key_Enter:
            btn_login_clicked();
            break;
        default:
            break;
    }
}

void Login::btn_login_clicked() {
    mainwnd.show();
    this->close();
}

/*
void Login::btn_login_clicked()
{
    QString username = ui->edit_username_2->text();
    QString password = ui->edit_password_2->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "用户名和口令不能为空");
    }
    else
    {
        int user_flag = QString::compare(username, user, Qt::CaseSensitive);
        int pass_flag = QString::compare(password, pass, Qt::CaseSensitive);


        if (!user_flag && !pass_flag)
        {
            mainwnd.show();
            this->close();
        }
        else
        {
            QMessageBox::information(this, "提示", "用户名或密码错误");
        }
    }
}
*/

void Login::btn_cancel_clicked() { this->close(); }
