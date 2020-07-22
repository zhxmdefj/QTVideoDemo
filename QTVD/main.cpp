#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    w.play("D:/My Documents/Pictures/Camera Roll/test.mp4");

    return a.exec();
}
