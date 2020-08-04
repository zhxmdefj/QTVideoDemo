#include "cvcamera.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CVCamera window;
    window.show();
    
    return a.exec();
}
