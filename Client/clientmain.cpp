#include "clientwindow.h"
#include <QApplication>
#include <iostream>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientWindow w;

    w.show();
    return a.exec();
}
