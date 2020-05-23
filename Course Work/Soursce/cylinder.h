#ifndef CYLINDER_H
#define CYLINDER_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QtMath>
#include <QOpenGLShaderProgram>

enum types {def=1, type_sqrt, type_parab} ;

class Cylinder:protected QOpenGLFunctions
{
public:
    Cylinder(int type,uint sector, uint stack,float base_rad=1.0f,float top_rad=1.0f,float height=5.0f);

    void draw(QOpenGLShaderProgram* m_program,bool clip=false);

private:
    float base_rad;
    float top_rad;
    float height;
    int type_of_cyl;

    QVector<QVector3D> points;
    QVector<QVector2D> tex_coord;
    QVector<QVector3D> normals;
    QVector<uint> indices;

    uint sector_count;
    uint stack_count;
    uint baseIndex;
    uint topIndex;

    QVector<QVector3D> getUnitCircleVert();
    QVector<QVector3D> getSideNormals();
    void initPoints();


    float calcRadiusSqrt(float);
    float calcRadiusPar(float z);
};

#endif // CYLINDER_H
