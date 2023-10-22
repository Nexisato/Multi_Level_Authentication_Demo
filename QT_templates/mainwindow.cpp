#include "mainwindow.h"
#include "ui_mainwindow.h"


//主页面，控制三个页面
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->addWidget(&homeWnd);
    ui->stackedWidget->addWidget(&filechoseWnd);

    btnGroup.addButton(ui->btn_home, 0);
    btnGroup.addButton(ui->btn_filechose, 1);



    connect(&btnGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // 设置默认选中的页面
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}
