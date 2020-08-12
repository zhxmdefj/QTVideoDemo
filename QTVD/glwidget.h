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
#include <QSlider>
#include <QSpinBox>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace std;

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
    QOpenGLShaderProgram shaderFirst;
    QOpenGLShaderProgram shaderLast;
    QOpenGLShaderProgram shaderWhitebalance;
    QOpenGLShaderProgram shaderBrightness;
    QOpenGLShaderProgram shaderSaturation;
    GLuint VBO, VAO, EBO, texture, quadVAO, quadVBO;
    GLuint FBO1, FBO2;
    GLuint FBOtexture1, FBOtexture2;
    GLuint currentFBO;
    GLuint currentFBOtexture;
    QOpenGLShaderProgram currentShader;
    GLuint RBO1, RBO2;

    cv::VideoCapture mVideoCap;
    cv::Mat mMatSrc;
    cv::Mat mFrame;
    QTimer *mTimer;
    QImage *mQImage;

    QSlider *pSlider1;
    QSpinBox *pSpinBox1;
    QSlider *pSlider2;
    QSpinBox *pSpinBox2;

    QSlider *pSlider1sw;
    QSpinBox *pSpinBox1sw;
    QSlider *pSlider2sw;
    QSpinBox *pSpinBox2sw;

    int flag = 0;
    long currentFrame = 0;
    long totalFrameNumber = 0;

    int m_nTimeValue = 0;

    void createShader();
    void createShaderProgram(QOpenGLShaderProgram &shaderProgram,
                             const char* vertexPath, const char* fragmentPath);

};

#endif // GLWIDGET_H
