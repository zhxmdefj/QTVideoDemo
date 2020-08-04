#include "cvcamera.h"
#include "glwidget.h"
#include "qtglwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CVCamera window;
    GLWidget glWindow;
    QtGLwidget qtGlWindow;
    glWindow.setWindowTitle(QObject::tr("GlWindow"));
    qtGlWindow.setWindowTitle(QObject::tr("QtGlWindow"));
    glWindow.show();
    qtGlWindow.show();
    return a.exec();
}
