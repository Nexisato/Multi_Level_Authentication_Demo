#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <fstream>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <qstring.h>
#include <qdir.h>
#include <QQueue>
#include <QJsonObject>
#include <QJsonArray>
#include <QTreeWidgetItem>
#include <qdirmodel.h>
#include <qtreeview.h>
#include <qtimer.h>

namespace Ui {
class MainWindow;
}

//证书相关信息结构体
struct certInfo
{
    QString common;         //用户名
    QString device;        //设备
    QString emailAddr;      //邮箱地址
    QString serialnumber;   //序列号
};

//证书操作变量结构体
struct certOpera
{
    QString usrurl = NULL;  //待验证证书路径
    X509 *usercert = NULL;  //x509 用户证书
    X509 *rootcert = NULL;  //x509 根证书
    X509_CRL * crl = NULL;  //x509_crl 根证书撤销链
    EVP_PKEY *pkey = NULL;  //EVP 根证书私钥
    QString ser;            //撤销证书时使用
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    /*/----------------------共用变量-----------------------\*/
    QString coredir;        //储存core文件夹路径 存放根证书及密钥
    QString reqdir;         //储存reqfiles文件夹路径 存放待处理请求文件
    QString signdir;        //存储signedfiles文件夹路径 存放生成用户证书及密钥
    QString reqfindir;      //储存reqfin文件夹路径 存放处理完成的请求文件
    QString reqfilename;    //接受的证书请求文件名

    certOpera certop;       //实例化证书操作结构体
    int indexptr1;          //ListWidget1索引
    int indexptr2;          //ListWidget2索引

    QList<QString> queue;   //未用
    QJsonObject jsignlist;  //JSON对象

    QString shellcmd;       //传递构造的shell命令
    int clickflag;          //鼠标点击事件flag
    QTimer *clicktimer;     //判断双击间隔 300
    QString index1, index2, index3;
    /*\---------------------------------------------------/*/


    /*/-----------------文件接收使用到的变量------------------\*/
    QTcpServer tcpserver;   //tcpserver对象
    QTcpSocket *tcpserconn; //tcp套接字连接对象
    qint64 totalbytes;      //接收数据总大小
    qint64 bytesrecved;     //已接受到的文件大小
    qint64 filenamesize;    //文件名大小
    QString filename;       //接收文件名
    QFile *localfile;       //本地文件
    QByteArray inblock;     //接收数据缓冲区
    /*\---------------------------------------------------/*/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void start(); //开启监听函数

    void getLocalIpAddr(); //获取本地IPv4地址函数

    void acceptConnection(); //建立连接

    void updateServerProgress(); //更新进度条并接收数据

    void displayError(QAbstractSocket::SocketError socketerror); //网络错误显示



    QString getTime(); //获取当前系统时间函数

    X509 * loadCert(); //X509 载入根证书函数

    EVP_PKEY * loadKey(); //EVP 载入根证书私钥函数

    bool loadRootCA(); //整合载入根证书信息函数

    //签发证书
    bool createCertFromRequestFile(int serialNumber, int days,
                                   char *requestFile, char *pubCert, int format);

    void signCertFile(); //签名处理函数

    void selectReqFile(); //选择请求文件函数

    void lightRevokeCert(); //证书签发tab处理撤销证书函数



    bool revokeCert(); //撤销证书函数

    bool restoreCert(); //恢复被撤销证书函数

    bool createCrl(); //生成证书撤销链函数

    void initCrlList(); //初始化证书撤销列表函数

    void showCrlInfo(); //显示撤销链信息函数

    bool checkCrlSerial(ASN1_INTEGER *serial); //检查撤销状态

    time_t ASN1_GetTimeT(ASN1_TIME* time); //ASN1_Time时间转为time_t时间



    void selectCertFile(); //选择证书文件函数

    QString getCertSubInfo(certInfo *info); //获取待验证书内容函数

    void setCertSubInfo(certInfo *info); //显示待验证内容函数

    bool checkByRootCert(); //根证书验证函数

    bool checkByCrl(); //根证书撤销链验证函数

    bool checkByTime(); //时效验证函数

    void rootCaVerify(); //CA服务器验证函数



    bool readJsonFile(QJsonObject &json); //读取JSON文件函数

    bool saveJsonFile(QJsonObject &json); //保存JSON文件函数

    bool writeSerial2Json(const int &serial); //写入签发序列号

    bool writeStatus2Json(int flag); //写入证书状态

    bool updateListWidget(); //更新ListWidget内容



    void on_pushButton_clicked();   //点击[接收文件]按钮

    void on_pushButton_6_clicked(); //点击[刷新IP]按钮

    void on_pushButton_5_clicked(); //点击[根证书签名]按钮

    void on_pushButton_4_clicked(); //点击[选择请求文件]按钮

    void on_pushButton_7_clicked(); //点击[撤销证书]按钮

    void on_pushButton_9_clicked(); //点击[生成撤销链]按钮

    void on_pushButton_8_clicked(); //点击[恢复证书]按钮

    void on_pushButton_2_clicked(); //点击[选择证书文件]按钮

    void on_listWidget_2_currentRowChanged(int currentRow); //ListWidget2行点击槽

    void on_pushButton_10_clicked(); //点击[验证证书]按钮

    void on_listWidget_currentRowChanged(int currentRow); //ListWidget行点击槽

    void on_pushButton_3_clicked(); //证书签发tab点击[撤销证书]按钮

    void on_treeView_clicked(const QModelIndex &index); //treeview单击事件槽

    void on_treeView_doubleClicked(const QModelIndex &index); //treeview双击事件槽

    void mouseClicked(); //处理 单击-双击-单击 机制槽

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H