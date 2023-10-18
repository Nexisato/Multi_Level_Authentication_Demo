#ifndef FILECHOSEWINDOW_H
#define FILECHOSEWINDOW_H

#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include<QDateTime>
namespace Ui
{
    class FileChoseWindow;
}

class FileChoseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FileChoseWindow(QWidget *parent = nullptr);
    ~FileChoseWindow();


private slots:
    void btn_filechose_clicked();
    void btn_fliedelte_clicked();
    void listwidget_clicked();
    void btn_clear_clicked();
private:
    Ui::FileChoseWindow *ui;
    QMap<QString, QString> filemap;
};

#endif // FILECHOSEWINDOW_H
