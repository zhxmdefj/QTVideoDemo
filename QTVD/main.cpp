#include "cvcamera.h"
#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CVCamera window;
    GLWidget glWindow;
    glWindow.show();
    
    return a.exec();
}
