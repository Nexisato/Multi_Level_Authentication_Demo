#ifndef WIDGET_H
#define WIDGET_H
#pragma execution_character_set("utf-8")
#include <mainwindow.h>

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Login : public QWidget {
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void btn_login_clicked();
    void btn_cancel_clicked();

private:
    Ui::Login *ui;
    QString user = "admin";
    QString pass = "admin";

    MainWindow mainwnd;
};
#endif  // LOGIN_H
