#ifndef FILECHOSEWINDOW_H
#define FILECHOSEWINDOW_H
#include "loader.h"
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include<QDateTime>
#include <QDropEvent>
#include <QMimeData>
#include <QCheckBox>
#include <list>
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
    virtual void dropEvent(QDropEvent *event) override;

private slots:
    void btn_filechose_clicked();
    void btn_fliedelte_clicked();
    void listwidget_clicked();
    void btn_clear_clicked();
    void btn_calculate_clicked();

    void checkbox_state_changed(int);

private:
    Ui::FileChoseWindow *ui;
    QMap<QString, QString> filemap;
    QStringList itemlist;
};

#endif // FILECHOSEWINDOW_H
