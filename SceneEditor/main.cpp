#include <QtGui/QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"
#include <iostream>
#include <QtXml>
#include <QFile>
#include <QMessageBox>

#include "core.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(":qdarkstyle/style.qss");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    a.setStyleSheet(ts.readAll());

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
