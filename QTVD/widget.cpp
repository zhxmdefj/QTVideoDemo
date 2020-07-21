#include "widget.h"
#include "ui_widget.h"
#include "ivideotread.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    IVideoTread* videoThread = new IVideoTread;
    connect(videoThread, SIGNAL(sgImage(QImage)),this, SLOT(slImage(QImage)));
    videoThread->openCamera();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::slImage(QImage img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}
