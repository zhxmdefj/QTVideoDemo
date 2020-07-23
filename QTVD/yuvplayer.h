#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QFile>
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
class YuvPlayer :public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    YuvPlayer(QWidget* parent = nullptr);
    ~YuvPlayer();
    void PlayOneFrame();
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private:
    GLuint textureUniformY; //y��������λ��
    GLuint textureUniformU; //u��������λ��
    GLuint textureUniformV; //v��������λ��
    GLuint id_y; //y�������ID
    GLuint id_u; //u�������ID
    GLuint id_v; //v�������ID
    QOpenGLTexture* m_pTextureY;  //y�������
    QOpenGLTexture* m_pTextureU;  //u�������
    QOpenGLTexture* m_pTextureV;  //v�������
    QOpenGLShader* m_pVSHader;  //������ɫ���������
    QOpenGLShader* m_pFSHader;  //Ƭ����ɫ������
    QOpenGLShaderProgram* m_pShaderProgram; //��ɫ����������
    int m_nVideoW; //��Ƶ�ֱ��ʿ�
    int m_nVideoH; //��Ƶ�ֱ��ʸ�
    unsigned char* m_pBufYuv420p;
    FILE* m_pYuvFile;
};

#endif // YUVPLAYER_H
