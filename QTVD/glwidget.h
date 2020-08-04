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

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    Ui::GLWidget *ui;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLBuffer VBO, EBO;
    QOpenGLVertexArrayObject VAO;

};

#endif // GLWIDGET_H
