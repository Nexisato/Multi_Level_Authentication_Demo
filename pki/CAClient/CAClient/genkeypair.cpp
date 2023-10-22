#include "mainwindow.h"
#include "ui_mainwindow.h"

////
/// \brief MainWindow::generateKeypair
/// \return true or false
/// 生成用户公私钥对函数
///
bool MainWindow::generateKeypair() {
    pkey = EVP_PKEY_new();
    rsapair = RSA_generate_key(bits, e, NULL, NULL);  //生成公私钥对
    BIO *bp;
    char name1[100];  //复用
    QString g_filename, pwd;
    g_filename = ui->lineEdit_8->text();
    if (g_filename.isEmpty()) {
        ui->textBrowser->append(getTime() + "请填写请求文件名称");
        return false;
    }
    strcpy(name1, (pridir + g_filename + ".prikey")
                      .toStdString()
                      .c_str());  // name1 存文件名
    bp = BIO_new_file(name1, "w");
    pwd = ui->lineEdit_11->text();
    if (ui->comboBox_2->currentIndex() == 8) {
        PEM_write_bio_RSAPrivateKey(bp, rsapair, NULL, NULL, 0, NULL, NULL);
        return true;
    } else {
        if (pwd.isEmpty()) {
            ui->textBrowser->append(getTime() + "私钥保护口令为空，请输入口令");
            return false;
        } else {
            strcpy(name1, pwd.toStdString().c_str());
            ui->textBrowser->append(getTime() + "已生成具备口令保护的用户私钥");
            PEM_write_bio_RSAPrivateKey(
                bp, rsapair, ecpform /*EVP_des_ede3_ofb()*/,
                (unsigned char *)name1, strlen(name1), NULL, NULL);
            return true;
        }
    }
    return false;
}

////
/// \brief MainWindow::setEncryptedForm
/// 设置私钥加密方式函数
///
void MainWindow::setEncryptedForm(int index1) {
    switch (index1) {
        case 0: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede3_cfb1方式生成私钥");
            ecpform = EVP_des_ede3_cfb1();
        } break;
        case 1: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede3_cfb8方式生成私钥");
            ecpform = EVP_des_ede3_cfb8();
        } break;
        case 2: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ofb方式生成私钥");
            ecpform = EVP_des_ofb();
        } break;
        case 3: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede_ofb方式生成私钥");
            ecpform = EVP_des_ede_ofb();
        } break;
        case 4: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede3_ofb方式生成私钥");
            ecpform = EVP_des_ede3_ofb();
        } break;
        case 5: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_cbc方式生成私钥");
            ecpform = EVP_des_cbc();
        } break;
        case 6: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede_cbc方式生成私钥");
            ecpform = EVP_des_ede_cbc();
        } break;
        case 7: {
            ui->textBrowser->append(getTime() +
                                    "设置：使用des_ede3_cbc方式生成私钥");
            ecpform = EVP_des_ede3_cbc();
        } break;
        case 8: {
            ui->textBrowser->append(
                getTime() + "未输入私钥保护口令，使用强制无口令方式生成私钥");
            ui->lineEdit_11->setEnabled(false);
        } break;
        default:
            break;
    }
}