#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class GLWidget;
}

class GLWidget : public QOpenGLWidget
        , protected /*QOpenGLExtraFunctions*/QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QImage MatToQ(const cv::Mat &src);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    Ui::GLWidget *ui;
    QOpenGLShaderProgram shaderProgram;
    GLuint VBO, VAO, EBO, texture;

    cv::VideoCapture mVideoCap;
    cv::Mat mMatSrc;
    QTimer *mTimer;
    QImage *mQImage;

};

#endif // GLWIDGET_H
