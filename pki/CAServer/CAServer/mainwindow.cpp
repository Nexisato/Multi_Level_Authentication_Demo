#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qprocess.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*/--------------------变量初始化部分----------------------\*/
    //公共部分
    coredir = "../core/";        //根证书及根证书私钥相对路径
    reqdir = "../reqfiles/";     //存储请求文件相对路径
    reqfindir = "../reqfin/";    //存储完成签发的请求文件相对路径
    signdir = "../signedfiles/"; //存储签发文件相对路径

    indexptr1 = -1;              //ListWidget2索引
    indexptr2 = -1;              //ListWidget2索引

    //文件接收部分
    totalbytes = 0;              //接收文件全部大小=文件真实大小+文件名等信息大小
    bytesrecved = 0;             //已接收的文件大小
    filenamesize = 0;            //文件名等信息大小

    //shell命令
    shellcmd = "";
    clickflag = 0;
    /*\-----------------------------------------------------/*/


    /*/--------------------函数功能初始化---------------------\*/
    getLocalIpAddr();   //获取本机IP地址
    initCrlList();      //初始化撤销链信息
    updateListWidget(); //初始化证书签发列表
    /*\-----------------------------------------------------/*/

    clicktimer = new QTimer(this);
    connect(clicktimer, SIGNAL(timeout()), this, SLOT(mouseClicked()));

    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

    //设置treeview内容，临时
    QDirModel *model = new QDirModel;
    //从缺省目录创建数据
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index("../reqfiles"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

////
/// \brief MainWindow::loadRootCA
/// \return true or false
/// 载入根证书信息函数
/// 调用loadCert及loadKey为certop结构体赋值
///
bool MainWindow::loadRootCA()
{
    //声明变量及传值
    X509 *rootcert = NULL;
    EVP_PKEY *pkey = NULL;
    rootcert = loadCert();
    pkey = loadKey();
    certop.rootcert = rootcert;
    certop.pkey = pkey;
    if (pkey == NULL || rootcert == NULL)
    {
        ui->textBrowser->append(getTime() + "加载根证书或密钥失败，请重试");
        return false;
    }
    else
    {
        ui->textBrowser->append(getTime() + "根证书及密钥加载成功...");
        return true;
    }
}

//点击[接收文件]按钮事件
void MainWindow::on_pushButton_clicked()
{
    start();
}

//点击[刷新IP]按钮事件
void MainWindow::on_pushButton_6_clicked()
{
    ui->comboBox->clear(); //清除下拉列表内容
    getLocalIpAddr();
}

//点击[根证书签名]按钮事件
void MainWindow::on_pushButton_5_clicked()
{
    signCertFile();
    ui->pushButton_5->setEnabled(false);
}

//点击[选择请求文件]按钮事件
void MainWindow::on_pushButton_4_clicked()
{
    selectReqFile();
}

//点击[撤销证书]按钮事件
void MainWindow::on_pushButton_7_clicked()
{
    certop.ser = ui->lineEdit_2->text();
    if(certop.ser.isEmpty())
    {
        ui->textBrowser->append(getTime() + "序列号输入值为空，请重试");
        return;
    }
    if(revokeCert())
    {
        ui->textBrowser->append(getTime() + "撤销证书成功，该证书已不具备效用");
        writeStatus2Json(1);
        showCrlInfo();
    }
}

//点击[生成撤销链]按钮事件
void MainWindow::on_pushButton_9_clicked()
{
    if(createCrl())
    {
        ui->textBrowser->append(getTime() + "已生成新的撤销链");
    }
}

//点击[恢复证书]按钮事件
void MainWindow::on_pushButton_8_clicked()
{
    if(!restoreCert())
    {
        ui->textBrowser->append(getTime() + "证书恢复失败，请重试");
        return;
    }
    ui->pushButton_8->setEnabled(false);
}

//ListWidget2行点击事件
void MainWindow::on_listWidget_2_currentRowChanged(int currentRow)
{
    ui->pushButton_8->setEnabled(true); //激活按键
    ui->pushButton_3->setEnabled(false); //disable另一个tab按键
    indexptr2 = currentRow - 1; //获取当前位置索引值
    qDebug() << "lw2" << indexptr2;
}

//点击[选择证书文件]按钮事件
void MainWindow::on_pushButton_2_clicked()
{
    selectCertFile();
}

//点击[验证证书]按钮事件
void MainWindow::on_pushButton_10_clicked()
{
    rootCaVerify();
}

//ListWidget行点击事件
void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_8->setEnabled(false);
    indexptr1 = currentRow - 1;
    qDebug() << "lw1" << indexptr1;
}

//证书签发tab点击[撤销证书按钮事件]
void MainWindow::on_pushButton_3_clicked()
{
    lightRevokeCert();
}

//treeview单击事件
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    //idx = index;
    if(1 == clickflag)
    {
        clickflag = 0;
        return;
    }
    index1 = index.parent().data().toString();
    index2 = index.data().toString();
    index3 = index1 + "/" + index2;
    clicktimer->start(300);
}

//treeview双击事件
void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    qDebug() << shellcmd;
    clickflag += 1;
    clicktimer->stop();
    QFileInfo fileInfo("../" + index3);
    if("" == shellcmd && "" != fileInfo.suffix())
    {
        ui->textBrowser->append(getTime() + "无法打开，请确认文件");
        return;
    }
    else
    {
        ui->textBrowser->append(getTime() + "打开" + index3);
        QProcess::execute(shellcmd);
    }
}

void MainWindow::mouseClicked()
{
    //qDebug() << "单击";
    clicktimer->stop();
    if("reqfiles" == index1)
    {
        shellcmd = "";
        QFileInfo fileInfo("../" + index3);
        //qDebug() << fileInfo.suffix();
        if("" == fileInfo.suffix())
        {
            ui->textBrowser->append(getTime() + "选中" + index3 + "文件夹");
        }
        else
        {
            ui->textBrowser->append(getTime() + "选中" + index3 + "文件");
        }
    }
    else
    {
        QString shellpath;
        shellpath = "..\\reqfiles\\";
        shellpath = shellpath + index1 + "\\" + index2;
        ui->textBrowser->append(getTime() + "选中" + index3 + "文件，双击查看内容");
        shellcmd = "explorer " + shellpath;
        //QProcess::execute(shellcmd);
    }
}