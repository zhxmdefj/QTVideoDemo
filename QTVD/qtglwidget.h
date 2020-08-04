#ifndef QTGLWIDGET_H
#define QTGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

namespace Ui {
class QtGLwidget;
}

class QtGLwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit QtGLwidget(QWidget *parent = nullptr);
    ~QtGLwidget();

protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;

private:
    Ui::QtGLwidget *ui;

    QOpenGLShaderProgram shaderProgram;
    QOpenGLBuffer VBO, EBO;
    QOpenGLVertexArrayObject VAO;
    QOpenGLTexture *texture = nullptr;
};

#endif // QTGLWIDGET_H
