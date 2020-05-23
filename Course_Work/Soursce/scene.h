#ifndef SCENE_H
#define SCENE_H

#include <QtGui/QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <vector>
#include <QMatrix4x4>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QKeyEvent>
#include <QCursor>
#include <QOpenGLTexture>
#include <QImage>
#include <QSet>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

#include "axes.h"
#include "sphere_and_conus.h"
#include "camera.h"
#include "cylinder.h"

struct Light{
    QVector4D light_pos;
    QVector3D la;
    QVector3D ld;
    QVector3D ls;
};

struct Material{
    QVector3D ka;
    QVector3D kd;
    QVector3D ks;
    float Shininess;
};

class Scene : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    Scene(QWidget* parent=0);

protected:

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    void initTextures();
    QOpenGLTexture* initTexture(const char* nof);
    QOpenGLTexture* initTexture(QImage img);

    void drawTorus(QOpenGLShaderProgram* program,
                   double r = 0.04, double c = 0.3,
                   int rSeg = 30, int cSeg = 30);

    void drawStair(QOpenGLShaderProgram* m_program);

    QOpenGLShaderProgram* def_sh;
    QOpenGLShaderProgram* fbo_sh;


    GLuint m_frame;

    Axes* a;
    Sphere_and_conus* objs;
    Cylinder* hand;

    QOpenGLTexture* angle_tex;
    QOpenGLTexture* cube_tex;
    QOpenGLTexture* floor_tex;

    QPointF start;

    bool mouse_flag;
    bool light_flag;
    float l_angle;
    QVector4D l_pos;

    GLenum type;

    Light l;
    Material glnc,mat;

    QSet<int> keys;

    Camera* cam;

    QOpenGLFramebufferObject* fbo;
    void drawScene(QOpenGLShaderProgram* m_program, bool from_light=false);
    void drawFB(QOpenGLShaderProgram* m_program);
};


#endif // SCENE_H
