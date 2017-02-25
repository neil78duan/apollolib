#include "mainwindow.h"
#include <QApplication>


#ifdef WIN32
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Ws2_32.lib")

#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
