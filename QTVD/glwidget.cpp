#include "glwidget.h"
#include "ui_glwidget.h"

#include <QDebug>
#include <QTimer>

#include "QDesktopWidget"

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::GLWidget),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer)
{
    ui->setupUi(this);
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, [=]{
            m_nTimeValue += 5;
            update();
    });
    mTimer->start(40);

    int nMin = 0;
    int nMax = 100;
    int nSingleStep = 10;

    pSpinBox1 = ui->spinBox;
    pSpinBox1->setMinimum(nMin);  // 最小值
    pSpinBox1->setMaximum(nMax);  // 最大值
    pSpinBox1->setSingleStep(nSingleStep);  // 步长
    pSlider1 = ui->verticalSlider;
    pSlider1->setOrientation(Qt::Vertical);  // 水平方向
    pSlider1->setMinimum(nMin);  // 最小值
    pSlider1->setMaximum(nMax);  // 最大值
    pSlider1->setSingleStep(nSingleStep);  // 步长
    connect(pSpinBox1, SIGNAL(valueChanged(int)), pSlider1, SLOT(setValue(int)));
    connect(pSlider1, SIGNAL(valueChanged(int)), pSpinBox1, SLOT(setValue(int)));

    pSpinBox2 = ui->spinBox_2;
    pSpinBox2->setMinimum(nMin);  // 最小值
    pSpinBox2->setMaximum(nMax);  // 最大值
    pSpinBox2->setSingleStep(nSingleStep);  // 步长
    pSlider2 = ui->verticalSlider_2;
    pSlider2->setOrientation(Qt::Vertical);  // 水平方向
    pSlider2->setMinimum(nMin);  // 最小值
    pSlider2->setMaximum(nMax);  // 最大值
    pSlider2->setSingleStep(nSingleStep);  // 步长
    connect(pSpinBox2, SIGNAL(valueChanged(int)), pSlider2, SLOT(setValue(int)));
    connect(pSlider2, SIGNAL(valueChanged(int)), pSpinBox2, SLOT(setValue(int)));

    pSpinBox1sw = ui->spinBox_3;
    pSpinBox1sw->setMinimum(nMin);  // 最小值
    pSpinBox1sw->setMaximum(nMax);  // 最大值
    pSpinBox1sw->setSingleStep(nSingleStep);  // 步长
    pSlider1sw = ui->verticalSlider_3;
    pSlider1sw->setOrientation(Qt::Vertical);  // 水平方向
    pSlider1sw->setMinimum(nMin);  // 最小值
    pSlider1sw->setMaximum(nMax);  // 最大值
    pSlider1sw->setSingleStep(nSingleStep);  // 步长
    connect(pSpinBox1sw, SIGNAL(valueChanged(int)), pSlider1sw, SLOT(setValue(int)));
    connect(pSlider1sw, SIGNAL(valueChanged(int)), pSpinBox1sw, SLOT(setValue(int)));


    pSpinBox2sw = ui->spinBox_4;
    pSpinBox2sw->setMinimum(nMin);  // 最小值
    pSpinBox2sw->setMaximum(nMax);  // 最大值
    pSpinBox2sw->setSingleStep(nSingleStep);  // 步长
    pSlider2sw = ui->verticalSlider_4;
    pSlider2sw->setOrientation(Qt::Vertical);  // 水平方向
    pSlider2sw->setMinimum(nMin);  // 最小值
    pSlider2sw->setMaximum(nMax);  // 最大值
    pSlider2sw->setSingleStep(nSingleStep);  // 步长
    connect(pSpinBox2sw, SIGNAL(valueChanged(int)), pSlider2sw, SLOT(setValue(int)));
    connect(pSlider2sw, SIGNAL(valueChanged(int)), pSpinBox2sw, SLOT(setValue(int)));


    pSpinBox1->setValue(50);
    pSpinBox2->setValue(50);

    pSpinBox1sw->setValue(50);
    pSpinBox2sw->setValue(50);

}

GLWidget::~GLWidget()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &FBO1);

    mTimer->stop();
    mVideoCap.release();
}

void GLWidget::initializeGL(){

    int SCR_WIDTH = this->geometry().width();
    int SCR_HEIGHT = this->geometry().height();

    this->initializeOpenGLFunctions();

    createShader();

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

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mVideoCap.open("D:/My Documents/Pictures/Camera Roll/test.mp4");

    // perview transform
    QMatrix4x4 transform;
    transform.rotate(180, QVector3D(0.0f, 0.0f, 0.0f));
    shaderFirst.bind();
    int transformLoc = shaderFirst.uniformLocation("transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.data());
    shaderFirst.release();

    // framebuffer1
    glGenFramebuffers(1, &FBO1);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO1);

    glGenTextures(1, &FBOtexture1);
    glBindTexture(GL_TEXTURE_2D, FBOtexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOtexture1, 0);

    glGenRenderbuffers(1, &RBO1);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO1); // now actually attach it
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // framebuffer2
    glGenFramebuffers(1, &FBO2);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO2);

    glGenTextures(1, &FBOtexture2);
    glBindTexture(GL_TEXTURE_2D, FBOtexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOtexture2, 0);

    glGenRenderbuffers(1, &RBO2);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO2); // now actually attach it
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFBO = FBO1;
    currentFBOtexture = FBO1;

}

void GLWidget::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL(){

    //cout<<currentFrame<<endl;

    // 第一次采样 FBO1 start
    glBindFramebuffer(GL_FRAMEBUFFER, FBO1);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 读取视频帧
    mVideoCap.read(mFrame);
    shaderFirst.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(flag == 0){
        // 设置每n帧获取一次帧
        if (currentFrame % 1 == 0) {
            mMatSrc = mFrame;
            if (currentFrame >= 1000) {
                flag = 1;
            }
            currentFrame++;
        }
        if(mMatSrc.elemSize()>0){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mMatSrc.cols, mMatSrc.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, mMatSrc.data);
        }
        glBindVertexArray(VAO);
        glUniform1i(shaderFirst.uniformLocation("ourTexture"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    shaderFirst.release();

    // FBO2 Start
    glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
    shaderWhitebalance.bind();
    glBindTexture(GL_TEXTURE_2D, FBOtexture1);
    glUniform1i(shaderWhitebalance.uniformLocation("screenTexture"), 0);
    glUniform1f(shaderWhitebalance.uniformLocation("tint"), pSlider1->value());
    glUniform1f(shaderWhitebalance.uniformLocation("temperature"), pSlider2->value());
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderWhitebalance.release();

    glBindFramebuffer(GL_FRAMEBUFFER, FBO1);
    shaderBrightness.bind();
    glBindTexture(GL_TEXTURE_2D, FBOtexture2);
    glUniform1i(shaderBrightness.uniformLocation("screenTexture"), 0);
    glUniform1f(shaderBrightness.uniformLocation("brightness"), pSlider1sw->value());
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderBrightness.release();

    glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
    shaderSaturation.bind();
    glBindTexture(GL_TEXTURE_2D, FBOtexture1);
    glUniform1i(shaderSaturation.uniformLocation("screenTexture"), 0);
    glUniform1f(shaderSaturation.uniformLocation("saturation"), pSlider2sw->value());
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderSaturation.release();

    // default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    shaderLast.bind();
    glBindTexture(GL_TEXTURE_2D, FBOtexture2);
    glUniform1i(shaderLast.uniformLocation("screenTexture"), 0);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shaderLast.release();

}

void GLWidget::createShader(){
    createShaderProgram(shaderFirst,"./shaders/shaderFirst.vert","./shaders/shaderFirst.frag");
    createShaderProgram(shaderLast,"./shaders/shaderLast.vert","./shaders/shaderLast.frag");
    createShaderProgram(shaderWhitebalance,"./shaders/shaderLast.vert","./shaders/shaderWhitebalance.frag");
    createShaderProgram(shaderBrightness,"./shaders/shaderLast.vert","./shaders/shaderBrightness.frag");
    createShaderProgram(shaderSaturation,"./shaders/shaderLast.vert","./shaders/shaderSaturation.frag");
}

void GLWidget::createShaderProgram(
        QOpenGLShaderProgram &shaderProgram,
        const char* vertexPath, const char* fragmentPath){
    bool success = false;
    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath);
    if (!success) {qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();return;}
    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath);
    if (!success) {qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();return;}
    success = shaderProgram.link();
    if(!success) {qDebug() << "shaderProgram link failed!" << shaderProgram.log();}
    //return success;
}
