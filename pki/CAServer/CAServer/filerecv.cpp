#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostInfo>
#include <qdir.h>

////
/// \brief MainWindow::start
/// 服务器启动监听函数
/// 获取界面中的数据并开启服务
///
void MainWindow::start()
{
    ui->pushButton->setEnabled(false);
    //进度条置0
    ui->progressBar->setValue(0);
    //bytesrecved = 0; //开始监听时清空之前数据
    QString hostip; //服务器ip
    int hostport; //服务器端口号
    hostip = ui->comboBox->currentText();
    hostport = ui->lineEdit->text().toInt();
    if(!tcpserver.listen(QHostAddress(hostip), hostport))
    {
        ui->textBrowser->append(getTime() + "error");
        close();
        return;
    }
    else
    {
        ui->textBrowser->append(getTime() + "服务器开启...正在监听...");
    }
}

////
/// \brief MainWindow::acceptConnection
/// 建立一个单线程的TCP通信
///
void MainWindow::acceptConnection()
{
    //tcpsktconn = new QTcpSocket;
    tcpserconn = tcpserver.nextPendingConnection();
    connect(tcpserconn,SIGNAL(readyRead()),this,SLOT(updateServerProgress()));
    connect(tcpserconn,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
    ui->textBrowser->append(getTime() + "接受连接");
    tcpserver.close(); //单线程
}

////
/// \brief MainWindow::updateServerProgress
/// 更新进度条并接收文件
/// 数据接收完成后清除缓存区数据
///
void MainWindow::updateServerProgress()
{
    QDataStream in(tcpserconn);
    in.setVersion(QDataStream::Qt_4_6);

    if(bytesrecved <= sizeof(qint64)*2)
    { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        if((tcpserconn->bytesAvailable() >= sizeof(qint64)*2)&&
                (filenamesize == 0))
        { //接收数据总大小信息和文件名大小信息
            in >> totalbytes >> filenamesize;
            bytesrecved += sizeof(qint64) * 2;
        }
        if((tcpserconn->bytesAvailable() >= filenamesize)&&
                (filenamesize != 0))
        {  //接收文件名，并建立文件
            in >> filename;
            QString tmpfname, tmpcurtime, tmpdirname;
            tmpfname = filename;
            int index = tmpfname.lastIndexOf(".");
            tmpfname.truncate(index);
            //qDebug() << tmpfname;
            QDateTime current_date_time = QDateTime::currentDateTime();
            tmpcurtime = current_date_time.toString("_MMdd");
            //qDebug() << tmpcurtime;
            tmpdirname = reqdir + tmpfname + tmpcurtime;
            QDir *rdir = new QDir; //建立附件文件夹
            bool exist = rdir->exists(tmpdirname);
            QString f_filename; //接收文件
            if (exist)
            {
                ui->textBrowser->append(getTime() + "附件文件夹已存在，文件已存入" + tmpdirname);
                f_filename = tmpdirname + "/" + filename;
            }
            else
            {

                bool ok = rdir->mkdir(tmpdirname);
                if (ok)
                {
                    ui->textBrowser->append(getTime() + "接收请求文件并附件文件夹");
                    f_filename = reqdir + filename;
                }
                else
                {
                    ui->textBrowser->append(getTime() + "创建文件夹失败");
                    return;
                }
            }
            ui->textBrowser->append(getTime() + QString("正在接收文件 '%1' ...").arg(filename));
            bytesrecved += filenamesize;
            //QString f_filename = reqdir + filename;
            localfile = new QFile(f_filename);
            if(!localfile->open(QFile::WriteOnly))
            {
                ui->textBrowser->append(getTime() + "接收文件失败");
                return;
            }
        }
        else
        {
            ui->textBrowser->append(getTime() + "有一个忘了叫什么的错误");
            return;
        }
    }
    if(bytesrecved < totalbytes)
    {  //如果接收的数据小于总数据，那么写入文件
        bytesrecved += tcpserconn->bytesAvailable();
        inblock = tcpserconn->readAll();
        localfile->write(inblock);
        inblock.resize(0);
    }
    ui->progressBar->setMaximum(totalbytes);
    ui->progressBar->setValue(bytesrecved);
    //更新进度条
    if(bytesrecved == totalbytes)
    { //接收数据完成时
        tcpserconn->close();
        localfile->close();
        ui->pushButton->setEnabled(true);
        ui->textBrowser->append(getTime() + QString("接收文件 '%1' 成功！").arg(filename));
        //更新treeview
        QDirModel *model = new QDirModel;
        ui->treeView->setModel(model);
        ui->treeView->setRootIndex(model->index("../reqfiles"));
        //完成一个传输，数据清零
        totalbytes = 0;
        bytesrecved = 0;
        filenamesize = 0;
    }
}

////
/// \brief MainWindow::displayError
/// TCP错误显示函数
///
void MainWindow::displayError(QAbstractSocket::SocketError) //错误处理
{
    qDebug() << tcpserconn->errorString();
    tcpserconn->close();
    ui->progressBar->reset();
    //ui->serverStatusLabel->setText(tr("服务端就绪"));
    ui->pushButton->setEnabled(true);
}

////
/// \brief MainWindow::getLocalIpAddr
/// 自动获取本机所处网络环境IP地址
///
void MainWindow::getLocalIpAddr()
{
    ui->comboBox->addItem("127.0.0.1");
    QString localHostName = QHostInfo::localHostName();
    //qDebug() <<"localHostName:"<<localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    //循环添加获取的IP地址
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //qDebug() <<"IPV4 Address: "<< address.toString();
            ui->comboBox->addItem(address.toString());
        }
    }
}
////
/// \brief MainWindow::getTime
/// \return 系统当前时间
/// 获取系统时间函数，用于textbrowser信息显示
///
QString MainWindow::getTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[hh:mm:ss]  ");
    return current_date;
}