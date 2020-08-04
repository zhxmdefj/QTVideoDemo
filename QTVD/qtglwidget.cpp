#include "qtglwidget.h"
#include "ui_qtglwidget.h"

QtGLwidget::QtGLwidget(QWidget *parent) :
    QOpenGLWidget(parent),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer),
    ui(new Ui::QtGLwidget)
{
    ui->setupUi(this);
}

QtGLwidget::~QtGLwidget()
{
    makeCurrent();

    VBO.destroy();
    EBO.destroy();
    VAO.destroy();

    delete texture;

    doneCurrent();

    delete ui;
}

void QtGLwidget::initializeGL(){
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


    //VAO，VBO data
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    QOpenGLVertexArrayObject::Binder vaoBind(&VAO);

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));

    EBO.create();
    EBO.bind();
    EBO.allocate(indices, sizeof(indices));

    // position attribute
    int attr = -1;
    attr = shaderProgram.attributeLocation("aPos");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray(attr);
    // color attribute
    attr = shaderProgram.attributeLocation("aColor");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray(attr);
    // texture coord attribute
    attr = shaderProgram.attributeLocation("aTexCoord");
    shaderProgram.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 6, 2, sizeof(GLfloat) * 8);
    shaderProgram.enableAttributeArray(attr);

    texture = new QOpenGLTexture(QImage("./resources/container.jpg"), QOpenGLTexture::GenerateMipMaps); //直接生成绑定一个2d纹理, 并生成多级纹理MipMaps
    if(!texture->isCreated()){
        qDebug() << "Failed to load texture";
    }
    // set the texture wrapping parameters
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);// 等于glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    texture->setMinificationFilter(QOpenGLTexture::Linear);   //等价于glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    shaderProgram.bind();
    shaderProgram.setUniformValue("ourTexture", 0);
    shaderProgram.release();

    VBO.release();

}

void QtGLwidget::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}

void QtGLwidget::paintGL(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram.bind();
    {
        glActiveTexture(GL_TEXTURE0);
        texture->bind();

        // render container
        QOpenGLVertexArrayObject::Binder vaoBind(&VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        texture->release();
    }
    shaderProgram.release();
}
