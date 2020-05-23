#ifndef SPHERE_AND_CONUS_H
#define SPHERE_AND_CONUS_H

#include <QVector>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QtMath>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class Sphere_and_conus:protected QOpenGLFunctions
{
public:
    Sphere_and_conus();

    void drawObj(QOpenGLShaderProgram* m_program, GLenum type);
    void setSphereResX(int res);
    void setSphereResY(int res);



private:
    QVector<QVector3D> sphere_points;
    QVector<uint> indices;
    QVector<QVector3D> sphere_normals;
    QVector<QVector2D> tex_coords;
    QVector3D center;
    uint stackCount, sectorCount;
    uint rad_sphere;


    void initSpherePoints();

    void drawSphere(QOpenGLShaderProgram* m_program);

    QVector3D SphereFun(float u, float v);
};

#endif // SPHERE_AND_CONUS_H
