#include "yuvplayer.h"
#include "ui_yuvplayer.h"
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMouseEvent>

YuvPlayer::YuvPlayer(QWidget* parent) :QOpenGLWidget(parent)
{
    textureUniformY = 0;
    textureUniformU = 0;
    textureUniformV = 0;
    id_y = 0;
    id_u = 0;
    id_v = 0;
    m_pBufYuv420p = NULL;
    m_pVSHader = NULL;
    m_pFSHader = NULL;
    m_pShaderProgram = NULL;
    m_pTextureY = NULL;
    m_pTextureU = NULL;
    m_pTextureV = NULL;
    m_pYuvFile = NULL;
    m_nVideoH = 0;
    m_nVideoW = 0;
}
YuvPlayer::~YuvPlayer()
{
}
void YuvPlayer::PlayOneFrame()
{//�������ܶ�ȡһ��yuvͼ�����ݽ�����ʾ,ÿ����һ�Σ�����ʾһ��ͼƬ
    if (NULL == m_pYuvFile)
    {
        //��yuv��Ƶ�ļ� ע���޸��ļ�·��
       // m_pYuvFile = fopen("F://OpenglYuvDemo//1920_1080.yuv", "rb");
        m_pYuvFile = fopen("D://Qt//akiyo_qcif.yuv", "rb");
        m_nVideoW = 320;
        m_nVideoH = 180;
        //����yuv��Ƶ���ݵķֱ������ÿ���,demo������1080p������ط�Ҫע���ʵ�����ݷֱ��ʶ�Ӧ��
//        m_nVideoW = 1920;
//        m_nVideoH = 1080;
    }
    //�����ڴ��һ֡yuvͼ������,���СΪ�ֱ��ʵ�1.5��
    int nLen = m_nVideoW * m_nVideoH * 3 / 2;
    if (NULL == m_pBufYuv420p)
    {
        m_pBufYuv420p = new unsigned char[nLen];
        qDebug("YuvPlayer::PlayOneFrame new data memory. Len=%d width=%d height=%d\n",
            nLen, m_nVideoW, m_nVideoW);
    }
    //��һ֡yuvͼ������ڴ���
    if (NULL == m_pYuvFile)
    {
        qFatal("read yuv file err.may be path is wrong!\n");
        return;
    }
    fread(m_pBufYuv420p, 1, nLen, m_pYuvFile);
    //ˢ�½���,����paintGL�ӿ�
    update();
    return;
}
void YuvPlayer::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    //�ִ�opengl��Ⱦ����������ɫ�����������������
    //��ɫ��������ʹ��openGL��ɫ����(OpenGL Shading Language, GLSL)��д��һ��С����,
    //       GLSL�ǹ�������OpenGL��ɫ��������,�����GLSL���Ե��﷨��Ҫ���߲����������
    //��ʼ��������ɫ�� ����
    m_pVSHader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    //������ɫ��Դ��
    const char* vsrc = "attribute vec4 vertexIn; \
    attribute vec2 textureIn; \
    varying vec2 textureOut;  \
    void main(void)           \
    {                         \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }";
    //���붥����ɫ������
    bool bCompile = m_pVSHader->compileSourceCode(vsrc);
    if (!bCompile)
    {
    }
    //��ʼ��Ƭ����ɫ�� ����gpu��yuvת����rgb
    m_pFSHader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    //Ƭ����ɫ��Դ��
    const char* fsrc = "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
    { \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
                    0,       -0.39465,  2.03211, \
                    1.13983, -0.58060,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
    }";
    //��glslԴ�����������������ɫ������
    bCompile = m_pFSHader->compileSourceCode(fsrc);
    if (!bCompile)
    {
    }
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
    //������ɫ����������
    m_pShaderProgram = new QOpenGLShaderProgram;
    //��Ƭ����ɫ�����ӵ���������
    m_pShaderProgram->addShader(m_pFSHader);
    //��������ɫ�����ӵ���������
    m_pShaderProgram->addShader(m_pVSHader);
    //������vertexIn��ָ��λ��ATTRIB_VERTEX,�������ڶ�����ɫԴ������������
    m_pShaderProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
    //������textureIn��ָ��λ��ATTRIB_TEXTURE,�������ڶ�����ɫԴ������������
    m_pShaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
    //���������������뵽����ɫ������
    m_pShaderProgram->link();
    //������������
    m_pShaderProgram->bind();
    //��ȡ��ɫ���е����ݱ���tex_y, tex_u, tex_v��λ��,��Щ����������������
    //Ƭ����ɫ��Դ���п��Կ���
    textureUniformY = m_pShaderProgram->uniformLocation("tex_y");
    textureUniformU = m_pShaderProgram->uniformLocation("tex_u");
    textureUniformV = m_pShaderProgram->uniformLocation("tex_v");
    // �������
    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         -1.0f, 1.0f,
         1.0f, 1.0f,
    };
    //��������
    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
    //��������ATTRIB_VERTEX�Ķ������ֵ�Լ���ʽ
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
    //��������ATTRIB_TEXTURE����������ֵ�Լ���ʽ
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
    //����ATTRIB_VERTEX���Ե�����,Ĭ���ǹرյ�
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    //����ATTRIB_TEXTURE���Ե�����,Ĭ���ǹرյ�
    glEnableVertexAttribArray(ATTRIB_TEXTURE);
    //�ֱ𴴽�y,u,v��������
    m_pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_pTextureY->create();
    m_pTextureU->create();
    m_pTextureV->create();
    //��ȡ����y��������������ֵ
    id_y = m_pTextureY->textureId();
    //��ȡ����u��������������ֵ
    id_u = m_pTextureU->textureId();
    //��ȡ����v��������������ֵ
    id_v = m_pTextureV->textureId();
    glClearColor(0.3, 0.3, 0.3, 0.0);//���ñ���ɫ
    //qDebug("addr=%x id_y = %d id_u=%d id_v=%d\n", this, id_y, id_u, id_v);
}
void YuvPlayer::resizeGL(int w, int h)
{
    if (h == 0)// ��ֹ�����
    {
        h = 1;// ������Ϊ1
    }
    //�����ӿ�
    glViewport(0, 0, w, h);
}
void YuvPlayer::paintGL()
{
    //����y��������
    //����������ԪGL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    //ʹ������y������������
    glBindTexture(GL_TEXTURE_2D, id_y);
    //ʹ���ڴ���m_pBufYuv420p���ݴ���������y��������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //����u��������
    glActiveTexture(GL_TEXTURE1);//����������ԪGL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW / 2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p + m_nVideoW * m_nVideoH);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //����v��������
    glActiveTexture(GL_TEXTURE2);//����������ԪGL_TEXTURE2
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW / 2, m_nVideoH / 2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p + m_nVideoW * m_nVideoH * 5 / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //ָ��y����Ҫʹ����ֵ ֻ����0,1,2�ȱ�ʾ������Ԫ������������opengl�����Ի��ĵط�
    //0��Ӧ������ԪGL_TEXTURE0 1��Ӧ������ԪGL_TEXTURE1 2��Ӧ�����ĵ�Ԫ
    glUniform1i(textureUniformY, 0);
    //ָ��u����Ҫʹ����ֵ
    glUniform1i(textureUniformU, 1);
    //ָ��v����Ҫʹ����ֵ
    glUniform1i(textureUniformV, 2);
    //ʹ�ö������鷽ʽ����ͼ��
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    return;
}