#ifndef CVCAMERA_H
#define CVCAMERA_H

#include <QWidget>
#include <QMainWindow>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class CVCamera;
}

class CVCamera : public QWidget
{
    Q_OBJECT

public:
    explicit CVCamera(QWidget *parent = nullptr);

    QImage MatImageToQt(const cv::Mat &src);

    ~CVCamera();

private slots:
    void readFarme();
    void on_Open_triggered();
    void on_Stop_triggered();

private:
    Ui::CVCamera *ui;
    cv::VideoCapture cap;
    cv::Mat src_image;
    QTimer *timer;
    QImage *image;
};

#endif // CVCAMERA_H
