#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QPainter>
#include <QPropertyAnimation>
#include <filechosewindow.h>
#include <homewindow.h>
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QButtonGroup btnGroup;

    FileChoseWindow filechoseWnd;
    HomeWindow homeWnd;
};

#endif // MAINWINDOW_H
