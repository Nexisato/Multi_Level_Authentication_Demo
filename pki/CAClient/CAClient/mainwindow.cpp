#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置lineEdit正则
    QRegExp regExpIP("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-4]|[01]?\\d\\d?)");
    QRegExp regExpPort("([0-9]{0,5})");
    QRegExp regExpReq("[A-Za-z_0-9]+"); //请求信息输入框正则 文件名使用
    QRegExp regExpEml("[a-zA-Z_0-9]+@([-A-Za-z]+\\.)+[a-zA-Z]{2,}"); //Email
    QRegExpValidator *pRegExpValidatorIp = new QRegExpValidator(regExpIP,this);
    QRegExpValidator *pRegExpValidatorPort = new QRegExpValidator(regExpPort,this);
    QRegExpValidator *pRegExpValidatorReq = new QRegExpValidator(regExpReq,this);
    QRegExpValidator *pRegExpValidatorEml = new QRegExpValidator(regExpEml,this);
    ui->lineEdit_9->setValidator(pRegExpValidatorIp);
    ui->lineEdit_10->setValidator(pRegExpValidatorPort);
    ui->lineEdit_8->setValidator(pRegExpValidatorReq);
    ui->lineEdit_6->setValidator(pRegExpValidatorEml);

    /***********************初始化数据部分*************************/
    //文件传输部分
    loadsize = 4*1024;
    totalbytes = 0;
    byteswritten = 0;
    bytestowrite = 0;
    tcpclient = new QTcpSocket(this);

    //公共部分
    reqdir = "../reqfile/";
    pridir = "../core/";

    //请求文件部分
    bits = 0; //推荐1024;
    req = X509_REQ_new();
    version = 1;
    /***********************初始化数据部分*************************/


    //成功连接服务器后开始传输文件
    connect(tcpclient,SIGNAL(connected()),this,SLOT(startTransfer()));
    //更新进度条
    connect(tcpclient,SIGNAL(bytesWritten(qint64)),this,
            SLOT(updateClientProgress(qint64)));
    //显示tcp连接错误
    connect(tcpclient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//选择文件函数
void MainWindow::selectFile()
{
    //打开单个文件，默认目录为[...\CAClient\reqfile],文件类型为.csr
    filename = QFileDialog::getOpenFileName(this,"select file",reqdir);//*/,"*.csr");
    //清空进度条
    ui->progressBar->setValue(0);
    if(!filename.isEmpty())
    {
        ui->pushButton_3->setEnabled(true);
        ui->textBrowser->append(getTime() + QString("打开文件 '%1' 成功").arg(filename));
    }
    else
    {
        ui->textBrowser->append(getTime() + "操作取消，未选择文件");
        return;
    }
}

//发送文件函数
//连接服务器及具体发送操作槽
void MainWindow::sendFile()
{
    ui->pushButton_3->setEnabled(false); //点击后无效按钮
    byteswritten = 0; //初始化变量 已发送字节为0
    ui->textBrowser->append(getTime() + "正在连接至服务器");
    tcpclient->connectToHost(QHostAddress(ui->lineEdit_9->text()),ui->lineEdit_10->text().toInt()); //连接操作
}

//文件发送具体操作
void MainWindow::startTransfer()
{
    localfile = new QFile(filename);
    if(!localfile->open(QFile::ReadOnly))
    {
        //will add qmessageboox
        ui->textBrowser->append(getTime() + "打开文件失败！");
        return;
    }
    totalbytes = localfile->size(); //文件总大小
    QDataStream sendOut(&outblock,QIODevice::WriteOnly); //文件流
    sendOut.setVersion(QDataStream::Qt_4_6);
    //得到文件名
    QString currentfilename = filename.right(filename.size() - filename.lastIndexOf('/')-1);
    //依次写入总大小信息空间，文件名大小信息空间，文件名（不包含路径）
    sendOut << qint64(0) << qint64(0) << currentfilename;
    totalbytes += outblock.size(); //总大小=文件名大小等信息+实际文件大小
    sendOut.device()->seek(0);
    sendOut<<totalbytes<<qint64((outblock.size() - sizeof(qint64)*2));
    //totalbytes是文件总大小，即两个quint64的大小+文件名+文件实际内容的大小
    //qint64((outblock.size() - sizeof(qint64)*2))得到的是文件名大小
    bytestowrite = totalbytes - tcpclient->write(outblock);
    //发送完头数据后剩余数据的大小，即文件实际内容的大小
    ui->textBrowser->append(getTime() + QString("已连接至服务器"));
    outblock.resize(0);
    ui->textBrowser->append(getTime() + "总大小：" + QString::number(totalbytes, 10));
    qDebug()<<"#####"<<totalbytes;
}

//更新进度条及发送文件函数
void MainWindow::updateClientProgress(qint64 numBytes)
{

    qDebug()<<"#######已发送："<<byteswritten<<"剩余："<<bytestowrite;
    byteswritten += (int)numBytes;
    //已经发送数据的大小
    if(bytestowrite > 0) //如果已经发送了数据
    {
        outblock = localfile->read(qMin(bytestowrite,loadsize));
      //每次发送loadsize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
      //就发送剩余数据的大小
        bytestowrite -= (int)tcpclient->write(outblock);
       //发送完一次数据后还剩余数据的大小
        outblock.resize(0);
       //清空发送缓冲区
    }
    else
    {
        localfile->close(); //没有发送任何数据，则关闭文件
    }
    ui->progressBar->setMaximum(totalbytes); //设置进度条
    ui->progressBar->setValue(byteswritten); //更新进度条
    if(byteswritten == totalbytes) //发送完毕
    {
        ui->textBrowser->append(getTime() + QString("传送文件 '%1' 成功").arg(filename));
        localfile->close();
        tcpclient->close();
    }
}

//错误信息显示函数
void MainWindow::displayError(QAbstractSocket::SocketError)
{
    ui->textBrowser->append(getTime() + "发生错误: " + tcpclient->errorString() + "... 操作取消");
    tcpclient->close();
    ui->progressBar->reset();
    ui->textBrowser->append(getTime() + "请检查网络或询问服务器管理员");
    ui->pushButton_3->setEnabled(true);
}

//获取系统当前时间函数
QString MainWindow::getTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[hh:mm:ss]  ");
    return current_date;
}

//设置信息输出对齐函数
QString MainWindow::setAline()
{
    QString aline = "            ";
    return aline;
}

//点击[选择文件]按钮事件
void MainWindow::on_pushButton_4_clicked()
{
    selectFile();
}

//连接服务器并[发送]按钮事件
void MainWindow::on_pushButton_3_clicked()
{
    QString ipaddr, port; //局部变量 用于判断ip地址与端口填写
    ipaddr = ui->lineEdit_9->text();
    port = ui->lineEdit_10->text();
    if(ipaddr.isEmpty() || port.isEmpty())
    {
        ui->textBrowser->append(getTime() + "请填写IP地址及端口");
        return;
    }
    sendFile();
}

//[生成证书请求]按钮事件
void MainWindow::on_pushButton_clicked()
{
    int ret1; //接收certReq函数返回值
    ret1 = certReq(rsapair); //调用生成请求文件函数并接受返回值
    if(ret1 != 1)
    {
        ui->textBrowser->append(getTime() + "生成证书请求文件失败，请更新版本或联系开发人员");
    }
    else
    {
        ui->textBrowser->append(getTime() + "证书文件生成成功");
        ui->pushButton->setEnabled(false);
    }
}

//[生成用户私钥]按钮事件
void MainWindow::on_pushButton_2_clicked()
{
    bits = ui->comboBox->currentText().toInt();
    if(!bits)
    {
        ui->textBrowser->append(getTime() + "请选择生成RSA公私钥对的比特长度");
    }
    if(generateKeypair())
    {
        ui->textBrowser->append(getTime() + "生成用户密钥对成功");
        ui->pushButton->setEnabled(true);
    }
    else
    {
        ui->textBrowser->append(getTime() + "生成密钥对失败，请更新版本或联系开发人员");
    }
}

//下拉菜单2响应事件
void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    ui->lineEdit_11->setEnabled(true);
    setEncryptedForm(index);
}