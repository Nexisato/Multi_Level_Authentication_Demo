#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

////
/// \brief MainWindow::readJson
/// \param json JSON对象
/// \return true or false
/// 读取JSON文件
///
bool MainWindow::readJsonFile(QJsonObject &json)
{
    QString j_dir;
    j_dir = coredir + "signlist.json";
    QFile loadFile(j_dir);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray loaddata = loadFile.readAll();
    QJsonDocument readJsonDoc(QJsonDocument::fromJson(loaddata));
    json = readJsonDoc.object();
    loadFile.close();
    return true;
}

////
/// \brief MainWindow::saveJsonFile
/// \param json JSON对象
/// \return true or false
/// 写入信息至JSON文件
///
bool MainWindow::saveJsonFile(QJsonObject &json)
{
    QString j_dir;
    j_dir = coredir + "signlist.json";
    if(json.isEmpty())
    {
        return false;
    }
    QFile SaveFile(j_dir);
    if (!SaveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QJsonDocument SaveJsonDoc(json);
    SaveFile.write(SaveJsonDoc.toJson());
    SaveFile.close();
    return true;
}

////
/// \brief MainWindow::writeSerial2Json
/// \param serial 签名文件序列号
/// \return true or false
/// 将签发证书序列号写入JSON文件中
///
bool MainWindow::writeSerial2Json(const int &serial)
{
    QJsonObject addsign;
    addsign.insert("serialNumber",serial);
    addsign.insert("time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    addsign.insert("status",false);
    QJsonArray array = jsignlist["signlist"].toArray();
    array.append(addsign);
    jsignlist["signlist"] = array;
    saveJsonFile(jsignlist);
    return true;
}

/////
/// \brief MainWindow::writeStatus2Json
/// \param flag 更新证书状态标识 1-填充撤销 2-填充恢复 3-快捷撤销
/// \return true or false
/// 撤销或恢复操作对JSON文件的更新函数
/// flag为1或2时都是使用相同的逻辑对JSON文件中status键值对进行修改
/// flag为3时为ListWidget1中的操作
/// 区别在于3从LW中按Row编号取值，1 2都是循环json[]取值判断
///
bool MainWindow::writeStatus2Json(int flag)
{
    readJsonFile(jsignlist);
    if(jsignlist.isEmpty())
    {
        qDebug()<<"json file is empty";
        return false;
    }
    readJsonFile(jsignlist);
    QJsonArray array = jsignlist["signlist"].toArray();
    if(flag == 3)
    {
        //update listwidget2
        qDebug() << "index at lw1 for lightrevoke" << indexptr1;
        QJsonObject objson = array[indexptr1].toObject();
        certop.ser = QString::number(objson["serialNumber"].toInt());
        //qDebug() << "empty test early" << certop.ser;
        if(revokeCert())
        {
            objson["status"] = true;
            array[indexptr1] = objson;
            jsignlist["signlist"] = array;
            saveJsonFile(jsignlist);
            updateListWidget();
            showCrlInfo();
            //certop.ser = "";
            //qDebug() << "empty test late" << certop.ser;
            ui->pushButton_3->setEnabled(false);
            return true;
        }
    }
    else
    {
        for(int i=0;i<array.size();i++)
        {
            QJsonObject objson = array[i].toObject();
            if(objson["serialNumber"]==certop.ser.toInt())
            {
                if(flag == 1)
                {
                    objson["status"] = true;
                }
                else if(flag == 2)
                {
                    objson["status"] = false;
                }
                else
                {
                    qDebug() << "wrong arg2 set";
                    return false;
                }
                array[i] = objson;
                jsignlist["signlist"] = array;
                saveJsonFile(jsignlist);
            }
        }
        updateListWidget();
        return true;
    }
    return false;
}