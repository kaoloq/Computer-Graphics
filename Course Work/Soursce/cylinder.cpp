#include "cylinder.h"

Cylinder::Cylinder(int type,uint sector, uint stack,float base_rad, float top_rad,float height):base_rad(base_rad),top_rad(top_rad),height(height)
{
    this->stack_count=stack;
    this->sector_count=sector;
    this->type_of_cyl=type;
    initPoints();
}

void Cylinder::draw(QOpenGLShaderProgram *m_program,bool clip)
{
    initializeOpenGLFunctions();

    m_program->bind();
    m_program->setUniformValue("clip",clip);
    m_program->setAttributeArray(0, points.data());
    m_program->setAttributeArray(2,normals.data());
    m_program->setAttributeArray(3,tex_coord.data());
    m_program->setAttributeValue("colorAttr",{0.1f,0.8f,0.1f,0.1f});



    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(2);
    m_program->enableAttributeArray(3);



  //  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
   // glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,indices.data());

    m_program->disableAttributeArray( 0);
    m_program->disableAttributeArray(2);
    m_program->disableAttributeArray(3);
    m_program->release();
}

QVector<QVector3D> Cylinder::getUnitCircleVert()
{

    float sectorStep = 2 * M_PI / sector_count;
    float sectorAngle;  // radian

    QVector<QVector3D> unitCircleVertices;
    for(uint i = 0; i <= sector_count; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back({cosf(sectorAngle),sinf(sectorAngle),0});   // z
    }
    return unitCircleVertices;
}

QVector<QVector3D> Cylinder::getSideNormals()
{

    float sectorStep = 2 * M_PI / sector_count;
    float sectorAngle;  // radian

    // compute the normal vector at 0 degree first
    // tanA = (baseRadius-topRadius) / height
    float zAngle = atan2(base_rad - top_rad, height);
    float x0 = cos(zAngle);     // nx
    float y0 = sin(zAngle);               // ny
    float z0 = 0;     // nz

    // rotate (x0,y0,z0) per sector angle
    QVector<QVector3D> normals;
    for(uint i = 0; i <= sector_count; ++i)
    {
        sectorAngle = i * sectorStep;
        normals.push_back({cosf(sectorAngle)*x0 - sinf(sectorAngle)*y0,z0,sinf(sectorAngle)*x0 + cosf(sectorAngle)*y0});   // nx

    }

    return normals;
}

void Cylinder::initPoints()
{
    // clear memory of prev arrays
    points.clear();
    normals.clear();
    tex_coord.clear();
    indices.clear();
                              // vertex position
    //float s, t;                                     // texCoord
    float radius,z;                                   // radius for each stack

    // get normals for cylinder sides
    QVector<QVector3D> sideNormals = getSideNormals();
    QVector<QVector3D> unitCircleVerices=getUnitCircleVert();
    // put vertices of side cylinder to array by scaling unit circle
    for(uint i = 0; i <= stack_count; ++i)
    {
        z = -(height * 0.5f) + (float)i / stack_count * height;      // vertex position z
        switch (type_of_cyl) {
        case def:
            radius =  (base_rad + (float)i / stack_count * (top_rad - base_rad));
            break;
        case type_sqrt:
             radius=calcRadiusSqrt(z);
            break;
        case type_parab:
            radius=calcRadiusPar(z);
            break;
        }
             // lerp

        float t = 1.0f - (float)i / stack_count;   // top-to-bottom

        for(uint j = 0; j <= sector_count; ++j)
        {
            QVector3D p=unitCircleVerices[j];

            points.append({p.x() * radius,z, p.y() * radius});
            normals.append(sideNormals[j]); // normal
            tex_coord.append({(float)j / sector_count, t});
        }
    }

    // remember where the base.top vertices start
    unsigned int baseVertexIndex = (unsigned int)points.size();

    // put vertices of base of cylinder
    z = -height * 0.5f;
    switch (type_of_cyl) {
    case def:
        radius =  base_rad;
        break;
    case type_sqrt:
         radius=calcRadiusSqrt(z);
        break;
    case type_parab:
        radius=calcRadiusPar(z);
        break;
    }
    points.append({0.0f,z,0.0f});
    normals.append({0.0f,-1.0f,0.0f});
    tex_coord.append({0.0f,0.0f});


    for(uint i = 0; i < sector_count; ++i)
    {
        QVector3D p=unitCircleVerices[i];

        points.append({p.x() * radius, z, p.y() * radius});
        normals.append({0.0f,-1.0f,0.0f});
        tex_coord.append({0.0f,0.0f});
    }

    // remember where the base vertices start
    unsigned int topVertexIndex = (unsigned int)points.size();

    // put vertices of top of cylinder
    z = height * 0.5f;
    switch (type_of_cyl) {
    case def:
        radius = top_rad;
        break;
    case type_sqrt:
         radius=calcRadiusSqrt(z);
        break;
    case type_parab:
        radius=calcRadiusPar(z);
        break;
    }
    points.append({0.0f,z,0.0f});
    normals.append({0.0f,1.0f,0.0f});
    tex_coord.append({0.0f,0.0f});

    for(uint i = 0; i < sector_count; ++i)
    {
        QVector3D p=unitCircleVerices[i];

        points.append({p.x() * radius, z, p.y() * radius});
        normals.append({0.0f,1.0f,0.0f});
       tex_coord.append({0.0f,0.0f});
    }

    // put indices for sides
    unsigned int k1, k2;
    for(uint i = 0; i < stack_count; ++i)
    {
        k1 = i * (sector_count + 1);     // bebinning of current stack
        k2 = k1 + sector_count + 1;      // beginning of next stack

        for(uint j = 0; j < sector_count; ++j, ++k1, ++k2)
        {
            // 2 trianles per sector
            indices.append(k1);
            indices.append(k1+1);
            indices.append(k2);

            indices.append(k2);
            indices.append(k1+1);
            indices.append(k2+1);
        }
    }

    // remember where the base indices start
    baseIndex = (unsigned int)indices.size();

    // put indices for base
    for(uint i = 0, k = baseVertexIndex + 1; i < sector_count; ++i, ++k)
    {
        if(i < (sector_count - 1)){
            indices.append(baseVertexIndex);
            indices.append(k+1);
            indices.append(k);
        }
        else{    // last triangle
            indices.append(baseVertexIndex);
            indices.append(baseVertexIndex+1);
            indices.append(k);
        }
    }

    // remember where the base indices start
    topIndex = (unsigned int)indices.size();

    for(uint i = 0, k = topVertexIndex + 1; i < sector_count; ++i, ++k)
    {
        if(i < (sector_count - 1)){
            indices.append(topVertexIndex);
            indices.append(k);
            indices.append(k+1);
        }
        else{
            indices.append(topVertexIndex);
            indices.append(k);
            indices.append(topVertexIndex+1);
        }
    }

    // generate interleaved vertex array as well
}

float Cylinder::calcRadiusSqrt(float z)
{
    return (float)sqrt(-z+height/2+0.02);
}

float Cylinder::calcRadiusPar(float z)
{
    return ((float)qPow(z,2)+height)/6.0f;
}
