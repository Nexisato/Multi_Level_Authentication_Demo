#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

////
/// \brief MainWindow::selectCertFile
/// 选择证书文件函数
///
void MainWindow::selectCertFile()
{
    char name1[100];           //局部变量 撤销链路径
    X509 *r_usercert = NULL;   //待验证证书
    BIO *b = NULL;             //接收证书等待格式化
    X509_CRL *r_Crl = NULL;      //证书撤销链表
    QString r_showinfo;        //接收cetCertSubInfo函数返回值
    certop.usrurl = QFileDialog::getOpenFileName(this,"select file",signdir,"*.crt;;*.der;;*.pem;;*.cer");
    if(certop.usrurl.isNull())
    {
        ui->textBrowser->append(getTime() + "选择待验证证书失败");
        return;
    }
    else
    {
        ui->textBrowser->append(getTime() + "已选择 '" + certop.usrurl + "' 文件");
        loadRootCA(); //加载根证书信息
        /*开始加载根证书撤销链*/
        QString r_crlname = coredir + "Crl.crl";
        strcpy(name1,r_crlname.toStdString().c_str());
        b = BIO_new_file(name1,"r"); //读取根证书撤销链
        if(b == NULL)
        {
            ui->textBrowser->append(getTime() + "加载根证书撤销链失败，请确认文件存在");
            BIO_free(b);
            return;
        }
        else
        {
            r_Crl = PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
            ui->textBrowser->append(getTime() + "根证书撤销链文件加载成功...");
            b = NULL;
        }
        /*加载待验证证书文件*/
        b = BIO_new_file(certop.usrurl.toStdString().data(),"r"); //BIO读取待验证证书
        if(b == NULL)
        {
            ui->textBrowser->append(getTime() + "加载待验证证书文件失败");
            BIO_free(b);
            return;
        }
        //首先读取der格式证书
        r_usercert = d2i_X509_bio(b,NULL);
        //如果未成功读取则尝试pem格式
        if(!r_usercert)
        {
            //通过BIO类型使用此函数获取X509结构体
            b = BIO_new_file(certop.usrurl.toStdString().data(),"r");
            r_usercert = PEM_read_bio_X509(b,NULL,NULL,NULL);
            if(!r_usercert)
            {
                ui->textBrowser->append(getTime() + "无效的公钥安全文件，该证书或许已损坏");
                return;
            }
            certop.usercert = r_usercert;
            ui->textBrowser->append(getTime() + "PEM格式待验证证书文件加载成功...");
            b = NULL;
        }
        else
        {
            certop.usercert = r_usercert;
            ui->textBrowser->append(getTime() + "DER格式待验证证书文件加载成功...");
        }
        /*实例化结构体并调用读取信息函数*/
        certInfo info;
        r_showinfo = getCertSubInfo(&info);
        ui->textBrowser->append(getTime() + r_showinfo);
        setCertSubInfo(&info);
        ui->pushButton_10->setEnabled(true);
    }
}

////
/// \brief MainWindow::getCertSubInfo
/// \param info 证书信息结构体实例
/// \return cert info string
/// 获取待验证证书信息函数
///
QString MainWindow::getCertSubInfo(certInfo *info)
{
    QString r_str = "待验证证书内容如下：";
    //qDebug() << certop.usercert;
    X509 *x509 = certop.usercert;
    //证书版本号 0-V1 1-V2 2-V3
    int version = X509_get_version(x509) + 1;
    QString ver = QString::number(version);
    r_str += "\n            版本:       V" + ver;
    //EVP_PKEY *r_key = X509_get_pubkey(x509);
    asn1_string_st *r_before, *r_after;
    ASN1_TIME *r_be, *r_af;
    //中间值x2
    time_t tt;
    QDateTime dt;
    QString r_dt;
    //证书生效时间
    r_before= X509_get_notBefore(x509);
    r_be = ASN1_STRING_dup(r_before);
    tt = ASN1_GetTimeT(r_be);
    dt = QDateTime::fromTime_t(tt);
    r_dt = dt.toString(Qt::TextDate);
    r_str += "\n            签发时间: " + r_dt;
    //证书过期时间
    r_after= X509_get_notAfter(x509);
    r_af = ASN1_STRING_dup(r_after);
    tt = ASN1_GetTimeT(r_af);
    dt = QDateTime::fromTime_t(tt);
    r_dt = dt.toString(Qt::TextDate);
    r_str += "\n            过期时间:  " + r_dt;
    //qDebug() << dt.toString(Qt::TextDate);
    /*
    X509_NAME *name = X509_get_subject_name(x509); //待验证书
    X509_NAME *name1 = X509_get_issuer_name(x509); //签发证书
    int num1 = X509_NAME_entry_count(name1); //条目总数
    X509_NAME_ENTRY *entry;
    ASN1_OBJECT *obj;
    ASN1_STRING *str;
    char objtmp[80] = {0};
    int fn_nid;
    const char *objbuf;
    setlocale(LC_CTYPE, "");
   
    r_str += "\n            issuer's information:";
    for(int i=0; i<num1; i++)
    {
        char out[255] = {0};//输出
        entry = (X509_NAME_ENTRY *)X509_NAME_get_entry(name1,i);
        obj = X509_NAME_ENTRY_get_object(entry);
        str = X509_NAME_ENTRY_get_data(entry);
        fn_nid = OBJ_obj2nid(obj);
        if(fn_nid == NID_undef)
        {
            OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
        }
        else
        {
            objbuf = OBJ_nid2sn(fn_nid);
            strcpy(objtmp,objbuf);
        }
        BIO *mem = BIO_new(BIO_s_mem());
        BIO_set_close(mem, BIO_CLOSE);
        ASN1_STRING_print_ex(mem,str,ASN1_STRFLGS_ESC_QUOTE );
        BUF_MEM *bptr;
        BIO_get_mem_ptr(mem, &bptr);
        int len = bptr->length;
        char * pbuf = new char[len+1];
        memset(pbuf,0,len+1);
        memcpy(pbuf,bptr->data,len);
        strncpy(out,pbuf,strlen(pbuf));
        if (mem != NULL)
        {
            BIO_free(mem);
        }
        r_str += " \n";
        r_str += "            ";
        r_str += objtmp;
        r_str += ":\t";
        r_str += out;
//        r_str += objtmp;
//        r_str += ": ";
//        r_str += out;
//        r_str += ",  ";
        //qDebug() << i << out;
        delete [] pbuf;
    }
    */
   /*
    ASN1_INTEGER *serial = X509_get_serialNumber(x509); //获取序列号
    QString q_serial = i2s_ASN1_INTEGER(NULL,serial); //转换为qstring类型
    qDebug() << name;
    int num = X509_NAME_entry_count(name); //条目总数
    qDebug() << num;
//    X509_NAME_ENTRY *entry;
//    ASN1_OBJECT *obj;
//    ASN1_STRING *str;
//    char objtmp[80] = {0};
//    int fn_nid;
//    const char *objbuf;
    setlocale(LC_CTYPE, "");
    
    r_str += "\n            subject's information:";
    for(int i=0; i<num; i++)
    {
        char out[255] = {0};//输出
        entry = (X509_NAME_ENTRY *)X509_NAME_get_entry(name,i);
        obj = X509_NAME_ENTRY_get_object(entry);
        str = X509_NAME_ENTRY_get_data(entry);
        fn_nid = OBJ_obj2nid(obj);
        qDebug() << fn_nid;
        if(fn_nid == NID_undef)
        {
            OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
        }
        else
        {
            objbuf = OBJ_nid2sn(fn_nid);
            strcpy(objtmp,objbuf);
        }
        BIO *mem = BIO_new(BIO_s_mem());
        BIO_set_close(mem, BIO_CLOSE);
        ASN1_STRING_print_ex(mem,str,ASN1_STRFLGS_ESC_QUOTE );
        BUF_MEM *bptr;
        BIO_get_mem_ptr(mem, &bptr);
        int len = bptr->length;
        char * pbuf = new char[len+1];
        memset(pbuf,0,len+1);
        memcpy(pbuf,bptr->data,len);
        strncpy(out,pbuf,strlen(pbuf));
        if (mem != NULL)
        {
            BIO_free(mem);
        }
        //不应对i循环而应该对OBJ_obj2nid(obj)的返回值nid循环
        switch(fn_nid)
        {
        case 17 : //device
            info->device = out;
            break;
        case 13 : //别名
            info->common = out;
            break;
        case 48 : //邮箱
            info->emailAddr = out;
            break;
        default:
            break;
        }

        r_str += " \n";
        r_str += "            ";
        r_str += objtmp;
        r_str += ":\t";
        r_str += out;
        //qDebug() << i << out;
        delete [] pbuf;
    }
    
    r_str += + "\n            serialNumber:\t" + q_serial;
    info->serialnumber = q_serial;
    */
    return r_str;
    
}
////
/// \brief MainWindow::setCertSubInfo
/// 显示待验证证书信息函数
///

void MainWindow::setCertSubInfo(certInfo *info)
{
    //ui->lineEdit_7->setText(info->common);
    //ui->lineEdit_6->setText(info->device);
    
    //ui->lineEdit_9->setText(info->emailAddr);
    //ui->lineEdit_10->setText(info->serialnumber);
}

////
/// \brief MainWindow::checkByRootCert
/// \return true or false
/// 使用根证书检查待验证证书
///
bool MainWindow::checkByRootCert()
{
    OpenSSL_add_all_algorithms();
    X509 *x509 = certop.usercert; //传值
    X509 *root = certop.rootcert;
    EVP_PKEY * pcert = X509_get_pubkey(root); //获取公钥
    int ret = X509_verify(x509,pcert); //检查
    EVP_PKEY_free(pcert);
    if(ret == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

////
/// \brief MainWindow::checkByCrl
/// \return true or false
/// 通过循环查找撤销链序列号判断证书是否被撤销
///
bool MainWindow::checkByCrl()
{
    X509 *x509 = certop.usercert; //赋值 同上
    X509_CRL *crl = certop.crl;
    STACK_OF(X509_REVOKED) *revoked = X509_CRL_get_REVOKED(crl);
    X509_REVOKED *rc;
    ASN1_INTEGER *serial = X509_get_serialNumber(x509);
    //qDebug() << serial  <<", " << q_serial;
    int num = sk_X509_REVOKED_num(revoked);
    bool bf = true;
    for(int i=0; i<num; i++)
    {
        rc=sk_X509_REVOKED_value(revoked,i);
        if(ASN1_INTEGER_cmp(serial,X509_REVOKED_get0_serialNumber(rc))==0)
        {
            bf = false;
        }
    }
    return bf;
}

////
/// \brief MainWindow::checkByTime
/// \return true or false
/// 通过时效验证待验证证书文件
///
bool MainWindow::checkByTime()
{
    X509 *x509 = certop.usercert;
    time_t ct = QDateTime::currentDateTime().toTime_t();
    asn1_string_st *before = X509_get_notBefore(x509); //获取前后时间
    asn1_string_st *after = X509_get_notAfter(x509);
    ASN1_UTCTIME *be = ASN1_STRING_dup(before), *af = ASN1_STRING_dup(after);
    bool bf;
    if(ASN1_UTCTIME_cmp_time_t(be,ct)>=0||ASN1_UTCTIME_cmp_time_t(af,ct)<=0)
    {
        bf = false;
    }
    else
    {
        bf = true;
    }
    ASN1_UTCTIME_free(be);
    ASN1_UTCTIME_free(af);
    return bf;
}

////
/// \brief MainWindow::rootCaVerify
/// 证书服务器对待验证证书进行验证操作
///
void MainWindow::rootCaVerify()
{
    bool r_allchecked = true; //立一个所有验证的flag ;-)
    if(checkByRootCert())
    {
        ui->textBrowser->append(getTime() + "通过服务器根证书验证，该证书由此证书中心签发");
    }
    else
    {
        r_allchecked = false;
        ui->textBrowser->append(getTime() + "这是一个不受信任的证书[!]");
    }
    if(checkByCrl())
    {
        ui->textBrowser->append(getTime() + "通过根证书撤销链验证，该证书未被撤销");
    }
    else
    {
        r_allchecked = false;
        ui->textBrowser->append(getTime() + "该证书已被撤销[!]");
    }
    if(checkByTime())
    {
        ui->textBrowser->append(getTime() + "通过中心系统时效验证，该证书仍在有效期内");
    }
    else
    {
        r_allchecked = false;
        ui->textBrowser->append(getTime() + "该证书已过期[!]");
    }
    if(r_allchecked)
    {
        ui->textBrowser->append(getTime() + "该证书已通过中心验证，认定为有效证书");
    }
    else
    {
        ui->textBrowser->append(getTime() + "该证书未通过中心验证，认定为无效证书");
    }
}
