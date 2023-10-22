#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <filechosewindow.h>
#include <homewindow.h>

#include <QButtonGroup>
#include <QMainWindow>
#include <QPainter>
#include <QPropertyAnimation>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
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

#endif  // MAINWINDOW_H
