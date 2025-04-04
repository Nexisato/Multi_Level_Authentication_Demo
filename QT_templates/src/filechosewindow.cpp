#include "filechosewindow.h"
#include "ui_filechosewindow.h"

FileChoseWindow::FileChoseWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileChoseWindow)
{
    ui->setupUi(this);



    //链接槽函数
    connect(ui->btn_filechose, &QPushButton::clicked, this, &FileChoseWindow::btn_filechose_clicked);
    connect(ui->btn_delete, &QPushButton::clicked, this, &FileChoseWindow::btn_fliedelte_clicked);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &FileChoseWindow::listwidget_clicked);
    connect(ui->btn_clear, &QPushButton::clicked, this, &FileChoseWindow::btn_clear_clicked);
    connect(ui->btn_calculate, &QPushButton::clicked, this, &FileChoseWindow::btn_calculate_clicked);
}

FileChoseWindow::~FileChoseWindow()
{
    delete ui;
}

void FileChoseWindow::dropEvent(QDropEvent *event)//拖拽上传
{
    QList<QUrl> urls = event->mimeData()->urls();
    QFileInfo info;
    QStringList filenames;
    if (urls.isEmpty())
    {
        qDebug() << "empty";
        return ;
    }
    QStringList filepaths;

    for (int i = 0 ; i < urls.size(); i++)
    {
        filepaths.append(urls.at(i).toLocalFile());
    }

    for (int i = 0; i < filepaths.size(); i++)
    {
        QString filename;
        info = QFileInfo(filepaths[i]);//获得每个路径的文件信息
        filename = info.fileName();

        if (filemap.contains(filename))
        {
            QMessageBox::warning(this, "警告", "请勿上传重复文件");
            return ;
        }
        filenames.append(filename);
    }
    //插入文件名
    for (int i = 0; i < filepaths.size(); i++)
    {
        filemap.insert(filenames[i], filepaths[i]);

        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(0, 30));
        pItem->setText(filenames[i]);
        ui->listWidget->addItem(pItem);
    }

}


void FileChoseWindow::btn_filechose_clicked()
{
    QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            QCoreApplication::applicationDirPath(), tr("*"));
    QStringList filenames;
    QFileInfo info;


    if (filepaths.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请选择文件");
    }
    else
    {
        //获取文件名并判断是否有重复
        for (int i = 0; i < filepaths.size(); i++)
        {
            QString filename;
            info = QFileInfo(filepaths[i]);//获得每个路径的文件信息
            filename = info.fileName();

            if (filemap.contains(filename))
            {
                QMessageBox::warning(this, "警告", "请勿上传重复文件");
                return ;
            }
            filenames.append(filename);
        }
        //插入文件名
        for (int i = 0; i < filepaths.size(); i++)
        {
            filemap.insert(filenames[i], filepaths[i]);
            QCheckBox *checkbox = new QCheckBox;
            QListWidgetItem *pItem = new QListWidgetItem;
            pItem->setSizeHint(QSize(0, 30));
            //            pItem->setText(filenames[i]);
            checkbox->setText(filenames[i]);


            ui->listWidget->addItem(pItem);
            ui->listWidget->setItemWidget(pItem, checkbox);


            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(checkbox_state_changed(int)));
        }
    }
}


void FileChoseWindow::btn_fliedelte_clicked()
{
    int row = ui->listWidget->currentRow();
    if (row < 0)
    {
        return ;
    }

    for (int i = 0; i < ui->listWidget ->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item(i);
        //将QWidget 转化为QCheckBox  获取第i个item 的控件
        QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
        if (checkbox->isChecked())
        {
            ui->listWidget->takeItem(i);
            filemap.remove(checkbox->text());

            i = i - 1;
        }
    }


    ui->listwidget_fileinfo->clear();



}

void FileChoseWindow::listwidget_clicked()
{
    ui->listwidget_fileinfo->clear();
    ui->edit_pid->clear();
    ui->edit_hash->clear();
    QListWidgetItem *item = ui->listWidget->currentItem();
    QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
    QString filename = checkbox->text();
    QString filepath = filemap.value(filename);
    QFileInfo finfo(filepath);

    QString file_path = "文件路径: " + filepath;
    QString file_size = "文件大小(字节): " + QString::number(finfo.size());
    QString file_birthtime = "文件创建时间: " + finfo.birthTime().toString("yyyy/MM/dd hh:mm:ss");
    QString file_lastmodified = "文件最近修改时间: " + finfo.lastModified().toString("yyyy/MM/dd hh:mm:ss");
    QString file_lastread = "文件最近读取时间: " + finfo.lastRead().toString("yyyy/MM/dd hh:mm:ss");
    QString file_houzhui = "文件后缀: " + finfo.suffix(); //完整的文件后缀后缀
    QString file_basename = "文件名: " + finfo.baseName(); //文件名

    if (filemap.contains(filename + "_md5"))
    {
        ui->edit_hash->setText(filemap.value(filename + "_md5"));
    }
    if (filemap.contains(filename + "_pid"))
    {
        ui->edit_pid->setText(filemap.value(filename + "_pid"));
    }


    ui->listwidget_fileinfo->addItem(file_basename);
    ui->listwidget_fileinfo->addItem(file_houzhui);
    ui->listwidget_fileinfo->addItem(file_size);
    ui->listwidget_fileinfo->addItem(file_path);
    ui->listwidget_fileinfo->addItem(file_birthtime);
    ui->listwidget_fileinfo->addItem(file_lastmodified);
    ui->listwidget_fileinfo->addItem(file_lastread);


}

void FileChoseWindow::btn_clear_clicked()
{
    ui->listWidget->clear();
    ui->listwidget_fileinfo->clear();
    ui->edit_pid->clear();
    ui->edit_hash->clear();
    QStringList keylist = filemap.keys();
    for (int i = 0 ; i < keylist.size(); i++)
    {
        filemap.remove(keylist[i]);
    }

}

void FileChoseWindow::btn_calculate_clicked()
{
    QListWidgetItem *item_now = ui->listWidget->currentItem();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item_now));
    QString filename_now = checkbox_now->text();
    int flag = 1;
    QString savepath = "./assets.json";
    for (int i = 0; i < itemlist.size(); i++)
    {
        QString filename = itemlist[i];
        QString filepath = filemap.value(filename);

        //计算文件hash和身份
        std::string  md5 = loader::compute_file_md5(filepath.toStdString());
        std::string pid = loader::md5_to_entityId(md5);
        QString qmd5 = QString::fromStdString(md5);
        QString qpid = QString::fromStdString(pid);

        if (loader::write_json_single(savepath.toStdString(), filename.toStdString(), md5, pid))
        {

            filemap.insert(filename + "_md5", qmd5);
            filemap.insert(filename + "_pid", qpid);
            if (QString::compare(filename, filename_now, Qt::CaseSensitive) == 0)
            {
                flag = 0;
                ui->edit_hash->setText(QString::fromStdString(md5));
                ui->edit_pid->setText(QString::fromStdString(pid));
                ui->listwidget_fileinfo->addItem("计算成功，信息写入：" + savepath);
            }

        }

        else
        {
            ui->listwidget_fileinfo->addItem("计算失败");
        }

    }
    if (flag)
    {
        ui->listwidget_fileinfo->clear();
        ui->listwidget_fileinfo->addItem("计算成功，信息写入：" + savepath);
    }

    /*
    QListWidgetItem *item = ui->listWidget->currentItem();
    QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
    QString filename = checkbox->text();
    QString filepath = filemap.value(filename);
    QString savepath = "./assets.json";

    //计算文件hash和身份
    std::string  md5 = loader::compute_file_md5(filepath.toStdString());
    std::string pid = loader::md5_to_entityId(md5);

    QString qmd5 = QString::fromStdString(md5);
    QString qpid = QString::fromStdString(pid);

    if (loader::write_json_single(savepath.toStdString(), filename.toStdString(), md5, pid))
    {

        filemap.insert(filename + "_md5", qmd5);
        filemap.insert(filename + "_pid", qpid);
        ui->edit_hash->setText(QString::fromStdString(md5));
        ui->edit_pid->setText(QString::fromStdString(pid));
        ui->listwidget_fileinfo->addItem("计算成功，信息写入：" + savepath);
    }
    else
    {
        ui->listwidget_fileinfo->addItem("计算失败");
    }
    */
}

void FileChoseWindow::checkbox_state_changed(int)
{
    itemlist.clear();
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item(i);

        QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
        if (checkbox->isChecked())
        {
            QString checkboxStr = checkbox->text();
            itemlist.append(checkboxStr);
        }


    }
    qDebug() << itemlist;
    listwidget_clicked();

}
