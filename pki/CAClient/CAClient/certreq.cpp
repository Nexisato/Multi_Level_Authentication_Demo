#include "mainwindow.h"
#include "ui_mainwindow.h"

////
/// \brief MainWindow::certReq
/// \param rsapair 公私钥对
/// \return 1 for success
/// 证书请求生成函数说明：
///     ret为x509等openssl提供函数的返回值
///     该返回值为1即表示函数执行成功，其他则表示失败
///     函数生成的请求为PEM格式
///
int MainWindow::certReq(RSA *rsapair) {
    certInfo certinfo;
    ret = X509_REQ_set_version(req, version);
    if (!ret) {
        printf("sign err at X509_REQ_set_version!\n");
        X509_REQ_free(req);
        return -1;
    }
    //开始读取用户输入内容并写入请求entry，同时向certInfo结构体赋值
    name = X509_NAME_new();
    strcpy(bytes, ui->lineEdit_4->text().toStdString().c_str());
    certinfo.common = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "userName", V_ASN1_UTF8STRING,
                                          (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 0, -1);
    strcpy(bytes, ui->lineEdit_5->text().toStdString().c_str());
    certinfo.device = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "deviceID", V_ASN1_UTF8STRING,
                                          (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes, ui->lineEdit_6->text().toStdString().c_str());
    certinfo.emailAddr = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(
        &entry, "emailAddress", V_ASN1_IA5STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    /* subject name */
    ret = X509_REQ_set_subject_name(req, name);
    if (!ret) {
        printf("sign err at X509_REQ_set_subject_name!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* pub key */
    pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, rsapair);    //使用自己的私钥签名
    ret = X509_REQ_set_pubkey(req, pkey);  //将公钥放入证书请求中
    if (!ret) {
        printf("sign err at X509_REQ_set_pubkey!\n");
        X509_REQ_free(req);
        return -1;
    }

    /*md = EVP_sha1(); //摘要
    ret = X509_REQ_digest(req, md, (unsigned char *)mdout, (unsigned int*)
    &mdlen); if(!ret)
    {
        printf("sign err at X509_REQ_digest!\n");
        X509_REQ_free(req);
        return -1;
    }
    */
    ret = X509_REQ_sign(req, pkey, NULL);  //放置摘要值
    if (!ret) {
        printf("sign err at X509_REQ_sign!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* 写入文件 PEM 格式 */
    char name1[100];  //生成的请求文件名称
    //路径变量'reqdir'写死在上层目录reqfile中
    QString c_filename = ui->lineEdit_8->text();
    if (c_filename.isEmpty()) {
        ui->textBrowser->append(getTime() + "请填写请求文件名称");
        return 0;
    }
    strcpy(name1, (reqdir + c_filename + ".csr").toStdString().c_str());
    b = BIO_new_file(name1, "w");
    PEM_write_bio_X509_REQ(b, req);  //生成请求文件
    BIO_free(b);

    //将结果输入到信息显示栏中
    ui->textBrowser->append(getTime() + "证书请求内容填写完成");
    ui->textBrowser->append(setAline() + "用户：" + certinfo.common);
    ui->textBrowser->append(setAline() + "设备：" + certinfo.device);
    ui->textBrowser->append(setAline() + "邮箱：" + certinfo.emailAddr);
    return 1;
}
