#include <openssl/x509v3.h>

#include <QDateTime>

#include "mainwindow.h"
#include "ui_mainwindow.h"

////
/// \brief MainWindow::revokeCert
/// \return true or false
/// 撤销证书事件函数
///
bool MainWindow::revokeCert() {
    int ret;                //接收返回值
    BIO *bpint = NULL;      // BIO大数对象
    char name1[100];        //局部变量 数据类型转换中间值
    X509 *x509 = NULL;      // x509根证书对象
    EVP_PKEY *pkey = NULL;  //根证书密钥对象
    time_t t;               // time_t类型时间
    ASN1_TIME *rvtime;      //用户证书被撤销时间
    X509_REVOKED *revoked;  // x509类型序列号 如0x1c3d87e0
    ASN1_INTEGER *serial;   // ASN1类型序列号 如0x1c3ad100
    QString r_crlname;      //局部变量 存储core路径
    loadRootCA();           //调用载入根证书信息函数
    x509 = certop.rootcert;
    pkey = certop.pkey;
    r_crlname = coredir + "Crl.crl";
    strcpy(name1, r_crlname.toStdString().c_str());
    if (certop.crl == NULL) {
        bpint = BIO_new_file(name1, "w");
        if (bpint == NULL) {
            ui->textBrowser->append(getTime() + "读取证书链失败，尝试创建");
            if (!createCrl()) {
                ui->textBrowser->append(getTime() +
                                        "创建证书撤销链文件失败，请重试");
                return false;
            }
        } else {
            certop.crl = PEM_read_bio_X509_CRL(bpint, NULL, NULL, NULL);
        }
    }
    /*添加被撤销的证书序列号*/
    revoked = X509_REVOKED_new();
    // qDebug() << "revoked" << revoked;
    serial = ASN1_INTEGER_new();
    // qDebug() << "serial" << serial;
    ret = ASN1_INTEGER_set(serial, certop.ser.toLong());
    // qDebug() << ret << serial;
    // qDebug() << certop.ser;
    if (!checkCrlSerial(serial))  //验证是否重复撤销
    {
        ui->textBrowser->append(getTime() + "该证书已被撤销，请勿重复撤销证书");
        BIO_free(bpint);
        return false;
    }
    if (X509_REVOKED_set_serialNumber(revoked, serial)) {
        rvtime = ASN1_TIME_new();
        t = time(NULL);
        ASN1_TIME_set(rvtime, t + 2000);
        X509_CRL_set_nextUpdate(certop.crl, rvtime);
        X509_REVOKED_set_revocationDate(revoked, rvtime);
        if (X509_CRL_add0_revoked(certop.crl, revoked)) {
            // qDebug() << "ok1";
            X509_CRL_sort(certop.crl);  // 排序
            bpint = BIO_new_file(name1, "wb");
            // qDebug() << name1;
            PEM_write_bio_X509_CRL(bpint, certop.crl);
            BIO_free(bpint);
            return true;
        } else
            return false;
    }
    return false;
}

////
/// \brief MainWindow::createCrl
/// \return true or false
/// 生成根证书撤销链函数
///
bool MainWindow::createCrl() {
    time_t t;
    X509_NAME *issuer;
    ASN1_TIME *lastUpdate, *nextUpdate;
    X509_CRL *crl = NULL;
    EVP_PKEY *pkey;
    BIO *bp;
    X509 *x509;
    loadRootCA();
    pkey = certop.pkey;
    x509 = certop.rootcert;
    crl = X509_CRL_new();
    /*设置版本*/
    X509_CRL_set_version(crl, 3);
    /*设置颁发者*/
    issuer = X509_NAME_dup(X509_get_issuer_name(x509));
    X509_CRL_set_issuer_name(crl, issuer);
    /*设置上次发布时间*/
    lastUpdate = ASN1_TIME_new();
    t = time(NULL);
    ASN1_TIME_set(lastUpdate, t);
    X509_CRL_set_lastUpdate(crl, lastUpdate);
    /*设置下次发布时间*/
    nextUpdate = ASN1_TIME_new();
    t = time(NULL);
    //设置下次发布时间，默认1000个单位
    ASN1_TIME_set(nextUpdate, t + 1000);
    X509_CRL_set_nextUpdate(crl, nextUpdate);
    /*签名*/
    X509_CRL_sign(crl, pkey, EVP_md5());
    /*写入文件*/
    char name1[100];  //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1, r_crlname.toStdString().c_str());
    bp = BIO_new_file(name1, "wb");
    PEM_write_bio_X509_CRL(bp, crl);
    certop.crl = crl;
    BIO_free(bp);
    return true;
}

////
/// \brief MainWindow::initCrlList
/// 初始化根证书撤销列表函数
///
void MainWindow::initCrlList() {
    //存储初始化函数中消息
    BIO *biof = NULL;  //接收CRL等待格式化
    char name1[100];   //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1, r_crlname.toStdString().c_str());
    if (certop.crl == NULL) {
        biof = BIO_new_file(name1, "r");
        if (biof == NULL) {
            BIO_free(biof);
            return;
        } else {
            certop.crl = PEM_read_bio_X509_CRL(biof, NULL, NULL, NULL);
        }
    }
    STACK_OF(X509_REVOKED) * revoked;
    revoked = X509_CRL_get_REVOKED(certop.crl);
    int num = sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc = NULL;
    ui->listWidget_2->clear();
    ui->listWidget_2->addItem("序号\t撤销序列号\t证书撤销时间(AST)");
    for (int i = 0; i < num; i++) {
        rc = sk_X509_REVOKED_value(revoked, i);
        asn1_string_st *revTime =
            (asn1_string_st *)X509_REVOKED_get0_revocationDate(rc);
        ASN1_TIME *rt = ASN1_STRING_dup(revTime);
        time_t tt = ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget_2->addItem(
            QString::number(i + 1) + '\t' +
            i2s_ASN1_INTEGER(NULL, X509_REVOKED_get0_serialNumber(rc)) + "\t" +
            dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

////
/// \brief MainWindow::showCrlInfo
/// 显示撤销链信息函数
///
void MainWindow::showCrlInfo() {
    //存储初始化函数中消息
    BIO *biof = NULL;  //接收CRL等待格式化
    char name1[100];   //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1, r_crlname.toStdString().c_str());
    if (certop.crl == NULL) {
        biof = BIO_new_file(name1, "r");
        if (biof == NULL) {
            ui->textBrowser->append(getTime() +
                                    "载入撤销链失败，请确认文件存在");
            BIO_free(biof);
            return;
        } else {
            certop.crl = PEM_read_bio_X509_CRL(biof, NULL, NULL, NULL);
        }
    }
    STACK_OF(X509_REVOKED) * revoked;
    revoked = X509_CRL_get_REVOKED(certop.crl);
    int num = sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc = NULL;
    ui->listWidget_2->clear();
    ui->listWidget_2->addItem("序号\t撤销序列号\t证书撤销时间(AST)");
    for (int i = 0; i < num; i++) {
        rc = sk_X509_REVOKED_value(revoked, i);
        asn1_string_st *revTime =
            (asn1_string_st *)X509_REVOKED_get0_revocationDate(rc);
        ASN1_TIME *rt = ASN1_STRING_dup(revTime);
        time_t tt = ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget_2->addItem(
            QString::number(i + 1) + '\t' +
            i2s_ASN1_INTEGER(NULL, X509_REVOKED_get0_serialNumber(rc)) + "\t" +
            dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

////
/// \brief MainWindow::checkCrlSerial
/// \return true or false
/// 判断证书的撤销状态，防止证书被重复撤销
///
bool MainWindow::checkCrlSerial(ASN1_INTEGER *serial) {
    X509_CRL *crl = certop.crl;
    STACK_OF(X509_REVOKED) *revoked = X509_CRL_get_REVOKED(crl);
    X509_REVOKED *rc;
    int num = sk_X509_REVOKED_num(revoked);
    bool bf = true;  // flag
    for (int i = 0; i < num; i++) {
        rc = sk_X509_REVOKED_value(revoked, i);
        if (ASN1_INTEGER_cmp(serial, X509_REVOKED_get0_serialNumber(rc)) == 0) {
            bf = false;
        }
    }
    return bf;
}

////change ASN1_Time to time_t
/// copy from
/// http://stackoverflow.com/questions/10975542/asn1-time-to-time-t-conversion
/// copy date 2016.2.1
/// from openssl_ca/v1.0 authored by qool
///
time_t MainWindow::ASN1_GetTimeT(ASN1_TIME *time) {
    struct tm t;
    const char *str = (const char *)time->data;
    size_t i = 0;
    memset(&t, 0, sizeof(t));
    if (time->type == V_ASN1_UTCTIME) { /* two digit year */
        t.tm_year = (str[i++] - '0') * 10;
        t.tm_year += (str[i++] - '0');
        if (t.tm_year < 70) t.tm_year += 100;
    } else if (time->type == V_ASN1_GENERALIZEDTIME) { /* four digit year */
        t.tm_year = (str[i++] - '0') * 1000;
        t.tm_year += (str[i++] - '0') * 100;
        t.tm_year += (str[i++] - '0') * 10;
        t.tm_year += (str[i++] - '0');
        t.tm_year -= 1900;
    }
    t.tm_mon = (str[i++] - '0') * 10;
    t.tm_mon += (str[i++] - '0') - 1;  // -1 since January is 0 not 1.
    t.tm_mday = (str[i++] - '0') * 10;
    t.tm_mday += (str[i++] - '0');
    t.tm_hour = (str[i++] - '0') * 10;
    t.tm_hour += (str[i++] - '0');
    t.tm_min = (str[i++] - '0') * 10;
    t.tm_min += (str[i++] - '0');
    t.tm_sec = (str[i++] - '0') * 10;
    t.tm_sec += (str[i++] - '0');
    /* Note: we did not adjust the time based on time zone information */
    return mktime(&t);
}

////
/// \brief MainWindow::restoreCert
/// 恢复被撤销证书函数
///
bool MainWindow::restoreCert() {
    int r_iptr =
        indexptr2;  // Bug maybe occurs here,indexptr2 will begin a big num
    if (indexptr2 < 0) {
        ui->textBrowser->append(getTime() + "请选择要恢复的证书");
        return false;
    }
    QString r_crlserial;  //局部变量 撤销序列号
    char name1[100];      //局部变量 撤销链路径
    STACK_OF(X509_REVOKED) *revoked = X509_CRL_get_REVOKED(certop.crl);
    X509_REVOKED *rc = sk_X509_REVOKED_value(revoked, r_iptr);
    r_crlserial = i2s_ASN1_INTEGER(NULL, X509_REVOKED_get0_serialNumber(rc));
    /*json操作*/
    //可以将r_crlserial值传入certop结构体中简化writeStatus2Json函数
    certop.ser = i2s_ASN1_INTEGER(NULL, X509_REVOKED_get0_serialNumber(rc));
    writeStatus2Json(2);
    // writeStatus2Json(2, r_crlserial);
    //    readJsonFile(jsignlist);
    //    if(jsignlist.isEmpty())
    //    {

    //        qWarning("Error: json empty.");
    //        return false;
    //    }
    //    QJsonArray signArray = jsignlist["signlist"].toArray();
    //    for(int i=0;i<signArray.size();i++)
    //    {
    //        QJsonObject objson = signArray[i].toObject();
    //        if(objson["serialNumber"]==r_crlserial.toInt())
    //        {
    //            objson["status"] = false;
    //            signArray[i] = objson;
    //            jsignlist["signlist"] = signArray;
    //            saveJsonFile(jsignlist);
    //        }
    //    }
    //    updateListWidget();
    /*--------*/
    BIO *bp = NULL;
    if (certop.crl == NULL) {
        ui->textBrowser->append(getTime() +
                                "根证书撤销链未正确载入，请检查文件存在性");
        return false;
    }
    sk_X509_REVOKED_delete(X509_CRL_get_REVOKED(certop.crl),
                           indexptr2);  // restore
    X509_CRL_sort(certop.crl);          // 排序
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1, r_crlname.toStdString().c_str());
    bp = BIO_new_file(name1, "wb");
    PEM_write_bio_X509_CRL(bp, certop.crl);
    showCrlInfo();  //刷新列表内容
    BIO_free(bp);
    indexptr2 = -1;
    ui->textBrowser->append(getTime() + "序列" + r_crlserial + "证书" +
                            "恢复成功");
    return true;
}