#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    bool success;
    try
    {
        success = shaderProgram.addShaderFromSourceFile(
                    QOpenGLShader::Vertex, vertexPath);
        if (!success) {
            qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
            return;
        }

        success = shaderProgram.addShaderFromSourceFile(
                    QOpenGLShader::Fragment, fragmentPath);
        if (!success) {
            qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
            return;
        }

        success = shaderProgram.link();
        if(!success) {
            qDebug() << "shaderProgram link failed!" << shaderProgram.log();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}

