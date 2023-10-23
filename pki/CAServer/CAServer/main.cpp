#include <QApplication>

#include "mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // check where is the path
    QString executableDir = QCoreApplication::applicationDirPath();
    std::cout << "ExecutableDir: " << executableDir.toStdString() << std::endl;

    QString currentDir = QDir::currentPath();
    std::cout << "CurrentDir: " << currentDir.toStdString() << std::endl;
    //

    MainWindow w;
    w.show();

    return a.exec();
}
