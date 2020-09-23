#include "cvcamera.h"
#include "glwidget.h"
#include "qtglwidget.h"
#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CVCamera window;
    GLWidget glWindow;
    QtGLwidget qtGlWindow;
    Widget widget;
    glWindow.setWindowTitle(QObject::tr("GlWindow"));
    qtGlWindow.setWindowTitle(QObject::tr("QtGlWindow"));
    //glWindow.show();
    //qtGlWindow.show();
    widget.show();
    return a.exec();
}
