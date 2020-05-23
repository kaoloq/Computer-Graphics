#ifndef AXES_H
#define AXES_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Axes : protected QOpenGLFunctions
{
public:
    Axes();

    void drawAxis(QOpenGLShaderProgram* m_program);

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLfloat vert[18];
    GLfloat col[18];

};

#endif // AXES_H
