#include "glwidget.h"
#include "ui_glwidget.h"

#include <iostream>
#include <QDir>

//static GLuint VBO, VAO, EBO;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::GLWidget),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer)
{
    ui->setupUi(this);

}

GLWidget::~GLWidget()
{
    makeCurrent();

    VAO.destroy();
    VBO.destroy();
    EBO.destroy();

    doneCurrent();

//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
    delete ui;
}

void GLWidget::initializeGL(){
    this->initializeOpenGLFunctions();

    bool success = shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Vertex, "./shaders/triangle.vert");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.addShaderFromSourceFile(
                QOpenGLShader::Fragment, "./shaders/triangle.frag");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.link();
    if(!success) {
        qDebug() << "shaderProgram link failed!" << shaderProgram.log();
    }

    //VAO，VBO数据部分
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
//    glBindVertexArray(VAO);

//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  //顶点数据复制到缓冲

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);//告诉程序如何解析顶点数据
//    glEnableVertexAttribArray(0);

//    glBindBuffer(GL_ARRAY_BUFFER, 0);//取消VBO的绑定, glVertexAttribPointer已经把顶点属性关联到顶点缓冲对象了
//    glBindVertexArray(0);   //取消VAO绑定

//    //线框模式，QOpenGLExtraFunctions没这函数, 3_3_Core有
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    QOpenGLVertexArrayObject::Binder vaoBind(&VAO);

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));

    EBO.create();
    EBO.bind();
    EBO.allocate(indices, sizeof(indices));

    int attr = -1;
    attr = shaderProgram.attributeLocation("aPos");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 3);
    shaderProgram.enableAttributeArray(attr);

    VBO.release();
}

void GLWidget::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

//    shaderProgram.bind();
//    glBindVertexArray(VAO);
//    //glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//    shaderProgram.release();

    shaderProgram.bind();

    QOpenGLVertexArrayObject::Binder vaoBind(&VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shaderProgram.release();

}
