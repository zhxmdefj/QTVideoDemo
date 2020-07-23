#include "widget.h"
#include "yuvplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    YuvPlayer yuvW;
    w.show();
    w.play("D:/My Documents/Pictures/Camera Roll/test.mp4");
    
    return a.exec();
}
