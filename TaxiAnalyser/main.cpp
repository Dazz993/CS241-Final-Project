#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Chengdu Taxi Order Analyser");
    w.show();
    return a.exec();
}
