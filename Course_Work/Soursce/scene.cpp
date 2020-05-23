#include "scene.h"
#include <QtMath>

Scene::Scene(QWidget* parent)
    :QOpenGLWidget(parent)
{
    a=new Axes;
    objs=new Sphere_and_conus;
    hand=new Cylinder(def,30,30,0.08,0.01,5.9);

    type=GL_POLYGON;

    cam=new Camera;

    l_angle=0.0f;
    l_pos={5*cosf(l_angle/20),3,5*sinf(l_angle/20),0.0f};
    l={l_pos,{0.35f,0.35f,0.35f},{1.0f,1.0f,1.0f},{1.9f,1.9f,1.9f}};
    glnc={{1.7f,1.7f,1.7f},{0.9f,0.9f,0.9f},{5.0f,5.0f,6.0f},256.0f};
    mat={{1.1f,1.1f,1.1f},{0.5f,0.5f,0.5f},{0.3f,0.3f,0.3f},8.0f};
}



void Scene::initializeGL(){
    initializeOpenGLFunctions();



    def_sh=new QOpenGLShaderProgram;
    def_sh->addShaderFromSourceFile(QOpenGLShader::Vertex,":/vShader.glsl");
    def_sh->addShaderFromSourceFile(QOpenGLShader::Fragment,":/fShader.glsl");
    def_sh->link();

    fbo_sh=new QOpenGLShaderProgram;
    fbo_sh->addShaderFromSourceFile(QOpenGLShader::Vertex,":/vShader_fbo.glsl");
    fbo_sh->addShaderFromSourceFile(QOpenGLShader::Fragment,":/fShader_fbo.glsl");
    fbo_sh->link();

    initTextures();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Depth);
    format.setSamples(16);



}

void Scene::resizeGL(int w, int h){
    glViewport(0,0,w,h);

}

void Scene::paintGL(){


    l_pos={10*cosf(l_angle/20),6,10*sinf(l_angle/20),0.0f};
    l.light_pos=l_pos;
    cam->moveCam(&keys);


 //   glBindBuffer(GL_FRAMEBUFFER,0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, width(), height());

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    def_sh->bind();

//    glBindTexture(GL_TEXTURE_2D,fbo->texture());
    def_sh->setUniformValue("shadowMap",0);
    def_sh->release();
    drawScene(def_sh);



    if(light_flag)l_angle++;
    ++m_frame;
    update();
}




void Scene::mousePressEvent(QMouseEvent *event){
    start=QPointF(event->x(),event->y());
    if(event->button()==Qt::RightButton){
        this->setCursor(Qt::BlankCursor);
        mouse_flag=true;
    }
    if(event->button()==Qt::LeftButton){
        light_flag=true;
    }
    if(event->button()==Qt::MidButton){
        fbo->toImage().save("screen.png");
        qDebug()<<event->button();
    }
}

void Scene::mouseReleaseEvent(QMouseEvent *event){
    if(event->button()==Qt::RightButton){
        QCursor a;
        a.setPos(QWidget::mapToGlobal({width()/2,height()/2}));
        setCursor(a);
        mouse_flag=false;
        this->unsetCursor();
    }
    if(event->button()==Qt::LeftButton){
        light_flag=false;
    }
}



void Scene::drawScene(QOpenGLShaderProgram* m_program, bool from_light)
{
    QMatrix4x4 model, view, projection, lightmatr;
    Material m=glnc;
    if(from_light){
        projection.ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
        view.lookAt({l.light_pos.x(),l.light_pos.y(),l.light_pos.z()},{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f});
    }else{
        projection.perspective(70.0f, 2300.0f/1080.0f, 0.1f, 100.0f);
        view=cam->getMatrix();
        lightmatr.ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
        lightmatr.lookAt({l.light_pos.x(),l.light_pos.y(),l.light_pos.z()},{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f});
        m_program->setUniformValue("lightmatrix",lightmatr);

    }

    //matrix.rotate(100.0f * m_frame / 300, 0, 1, 0);

    QVector<QVector3D> points;
    QVector<QVector3D> up_face={
        QVector3D(-1.0f,  1.0f,  1.0f)*0.85+QVector3D(0.0f,0.3f,0.0f), // v20
        QVector3D( 1.0f,  1.0f,  1.0f)*0.85+QVector3D(0.0f,0.3f,0.0f), // v21
        QVector3D(-1.0f,  1.0f, -1.0f)*0.85+QVector3D(0.0f,0.3f,0.0f), // v22
        QVector3D( 1.0f,  1.0f, -1.0f)*0.85+QVector3D(0.0f,0.3f,0.0f)
    };  // v23;
    QVector<QVector2D> tex_coords;
    QVector<QVector3D> normals;
    QVector<GLushort> indices;

    QVector<QVector3D> clipped;
    QVector<QVector3D> clipped_normals;
    QVector<QVector2D> clipped_tex_coords;

    QVector<QVector3D> floor;
    QVector<QVector3D> floor_normals;
    QVector<QVector2D> floor_tex_coords;

    points={

        QVector3D(-1.0f, -1.0f,  1.0f),  // v0
        QVector3D( 1.0f, -1.0f,  1.0f), // v1
        QVector3D(-1.0f,  1.0f,  1.0f),  // v2-
        QVector3D( 1.0f,  1.0f,  1.0f), // v3-

        // Vertex data for face 1
        QVector3D( 1.0f, -1.0f,  1.0f), // v4
        QVector3D( 1.0f, -1.0f, -1.0f),  // v5
        QVector3D( 1.0f,  1.0f,  1.0f),  // v6-
        QVector3D( 1.0f,  1.0f, -1.0f), // v7-

        // Vertex data for face 2
        QVector3D( 1.0f, -1.0f, -1.0f), // v8
        QVector3D(-1.0f, -1.0f, -1.0f),  // v9
        QVector3D( 1.0f,  1.0f, -1.0f), // v10-
        QVector3D(-1.0f,  1.0f, -1.0f), // v11-

        // Vertex data for face 3
        QVector3D(-1.0f, -1.0f, -1.0f), // v12
        QVector3D(-1.0f, -1.0f,  1.0f),  // v13
        QVector3D(-1.0f,  1.0f, -1.0f), // v14-
        QVector3D(-1.0f,  1.0f,  1.0f),  // v15-

        // Vertex data for face 4
        QVector3D(-1.0f, -1.0f, -1.0f), // v16
        QVector3D( 1.0f, -1.0f, -1.0f), // v17
        QVector3D(-1.0f, -1.0f,  1.0f), // v18
        QVector3D( 1.0f, -1.0f,  1.0f), // v19


     };

    points.append(up_face);

    clipped={
        up_face[0], up_face[1],points[3],points[2],
        up_face[1], up_face[3], points[7], points[3],
        up_face[3], up_face[2], points[11], points[7],
        up_face[2], up_face[0], points[2], points[11]
    };



    tex_coords={
        QVector2D(0.0f, 0.0f),  // v0
        QVector2D(0.33f, 0.0f), // v1
        QVector2D(0.0f, 0.5f),  // v2
        QVector2D(0.33f, 0.5f), // v3

        // Vertex data for face 1
        QVector2D( 0.0f, 0.5f), // v4
        QVector2D(0.33f, 0.5f), // v5
        QVector2D(0.0f, 1.0f),  // v6
        QVector2D(0.33f, 1.0f), // v7

        // Vertex data for face 2
        QVector2D(0.66f, 0.5f), // v8
        QVector2D(1.0f, 0.5f),  // v9
        QVector2D(0.66f, 1.0f), // v10
        QVector2D(1.0f, 1.0f),  // v11

        // Vertex data for face 3
        QVector2D(0.66f, 0.0f), // v12
        QVector2D(1.0f, 0.0f),  // v13
        QVector2D(0.66f, 0.5f), // v14
        QVector2D(1.0f, 0.5f),  // v15

        // Vertex data for face 4
        QVector2D(0.33f, 0.0f), // v16
        QVector2D(0.66f, 0.0f), // v17
        QVector2D(0.33f, 0.5f), // v18
        QVector2D(0.66f, 0.5f), // v19

        // Vertex data for face 5
        QVector2D(0.33f, 0.5f), // v20
        QVector2D(0.66f, 0.5f), // v21
        QVector2D(0.33f, 1.0f), // v22
        QVector2D(0.66f, 1.0f)


    };

    indices={
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
       12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
       16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
       20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
   };

    normals={
        QVector3D(0.0f,0.0f,  1.0f),  // v0
        QVector3D(0.0f,0.0f,  1.0f),  // v0
        QVector3D(0.0f,0.0f,  1.0f),  // v0
        QVector3D(0.0f,0.0f,  1.0f),  // v0

        QVector3D( 1.0f, 0.0f,  0.0f), // v4
        QVector3D( 1.0f, 0.0f, 0.0f),  // v5
        QVector3D( 1.0f,  0.0f,  0.0f),  // v6-
        QVector3D( 1.0f,  0.0f, 0.0f), // v7-

        QVector3D( 0.0f, 0.0f, -1.0f), // v8
        QVector3D( 0.0f, 0.0f, -1.0f), // v8
        QVector3D( 0.0f, 0.0f, -1.0f), // v8
        QVector3D( 0.0f, 0.0f, -1.0f), // v8

        // Vertex data for face 3
        QVector3D(-1.0f, 0.0f, 0.0f), // v12
        QVector3D(-1.0f, 0.0f,  0.0f),  // v13
        QVector3D(-1.0f, 0.0f, 0.0f), // v14-
        QVector3D(-1.0f, 0.0f,  0.0f),  // v15-

        QVector3D(0.0, -1.0f, 0.0f), // v16
        QVector3D( 0.0f, -1.0f, 0.0f), // v17
        QVector3D(0.0f, -1.0f,  0.0f), // v18
        QVector3D( 0.0f, -1.0f,  0.0f), // v19

        QVector3D(0.0f, 1.0f, 0.0f), // v16
        QVector3D(0.0f, 1.0f, 0.0f), // v16
        QVector3D(0.0f, 1.0f, 0.0f), // v16
        QVector3D(0.0f, 1.0f, 0.0f), // v16


    };


    floor={
        {1.0f,0.0f,1.0f},
        {1.0f,0.0f,-1.0f},
        {-1.0,0.0f,-1.0f},
        {-1.0f,0.0f,1.0f}
    };
    floor_normals={
        {0.0f,1.0f,0.0f},
        {0.0f,1.0f,0.0f},
        {0.0f,1.0f,0.0f},
        {0.0f,1.0f,0.0f}
    };

    floor_tex_coords={
        {0.0f,1.0f},
        {1.0f,1.0f},
        {1.0f,0.0f},
        {0.0f,0.0f}
    };
    clipped_normals=clipped;
    clipped_tex_coords={
        {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},
        {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},
        {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},
        {0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f},
    };

    int start_clipped=points.size();
    points.append(clipped);
    normals.append(clipped_normals);
    tex_coords.append(clipped_tex_coords);

    int start_floor=points.size();
    points.append(floor);
    normals.append(floor_normals);
    tex_coords.append(floor_tex_coords);

    m_program->bind();
    m_program->setAttributeArray(0, points.data());
    m_program->setAttributeArray(2,normals.data());
    m_program->setAttributeArray(3,tex_coords.data());

    m_program->setAttributeValue(1,QVector3D{0.1f,0.8f,0.1f});

    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(2);
    m_program->enableAttributeArray(3);


    m_program->setUniformValue("l.position",l.light_pos);
    m_program->setUniformValue("l.la",l.la);
    m_program->setUniformValue("l.ld",l.ld);
    m_program->setUniformValue("l.ls",l.ls);
    m_program->setUniformValue("material.ka",m.ka);
    m_program->setUniformValue("material.kd",m.kd);
    m_program->setUniformValue("material.ks",m.ks);
    m_program->setUniformValue("material.Shininess",m.Shininess);



    QVector3D size(2.0f,2.0f,2.0f);
    model.setToIdentity();
    QMatrix4x4 sc,sc1,matr;
    sc.scale({0.5f,0.2f,1.8f});



    for(int i=0; i<4; i++){



        m_program->setUniformValue("matrix",projection*view*sc*model);
        m_program->setUniformValue("modelview",view*sc*model);
        m_program->setUniformValue("normal_m",(sc*model).normalMatrix());
        m_program->setUniformValue("model",(sc*model));
     /*   handler_tex->bind();
        def_sh->setUniformValue("texture",0);*/


        glFrontFace(GL_CW);
        cube_tex->bind();
        m_program->setUniformValue("texture",0);
        glDrawElements(GL_TRIANGLE_STRIP,indices.size(),GL_UNSIGNED_SHORT,indices.data());
        glFrontFace(GL_CCW);
        angle_tex->bind();
        m_program->setUniformValue("texture",0);
        glDrawArrays(GL_QUADS,start_clipped,clipped.size());

        model.translate(size.x(),size.y(),0);
    }

    matr.translate(3*size.x()+0.5,size.y()-0.5,size.z()-0.2);
    matr.scale(3.0f,0.2f,3.6f);

    m_program->setUniformValue("matrix",projection*view*sc1*matr);
    m_program->setUniformValue("modelview",view*sc1*matr);
    m_program->setUniformValue("normal_m",(sc1*matr).normalMatrix());
     m_program->setUniformValue("model",(sc*model));
    /*   handler_tex->bind();
    def_sh->setUniformValue("texture",0);*/

    glFrontFace(GL_CW);
    cube_tex->bind();
    m_program->setUniformValue("texture",0);
    glDrawElements(GL_TRIANGLE_STRIP,indices.size(),GL_UNSIGNED_SHORT,indices.data());
    glFrontFace(GL_CCW);
    angle_tex->bind();
    m_program->setUniformValue("texture",0);
    glDrawArrays(GL_QUADS,start_clipped,clipped.size());

    model.translate(0.0f,0.0f,size.z());
    model.rotate(180.0f,{0.0f,1.0f,0.0f});
    model.translate(size.x(),0.0f,0.0f);
    for(int i=0; i<4; i++){



        m_program->setUniformValue("matrix",projection*view*sc*model);
        m_program->setUniformValue("modelview",view*sc*model);
        m_program->setUniformValue("normal_m",(sc*model).normalMatrix());
         m_program->setUniformValue("model",(sc*model));
        /*   handler_tex->bind();
        def_sh->setUniformValue("texture",0);*/
        glFrontFace(GL_CW);
        cube_tex->bind();
        m_program->setUniformValue("texture",0);
        glDrawElements(GL_TRIANGLE_STRIP,indices.size(),GL_UNSIGNED_SHORT,indices.data());
        glFrontFace(GL_CCW);
        angle_tex->bind();
        m_program->setUniformValue("texture",0);
        glDrawArrays(GL_QUADS,start_clipped,clipped.size());

        model.translate(size.x(),size.y(),0);
    }

    model.setToIdentity();
    model.scale(20.0f,3.0f,4.0f);
    model.translate(0.5f,0.0f,0.0f);
    m_program->setUniformValue("matrix",projection*view*sc*model);
    m_program->setUniformValue("modelview",view*sc*model);
    m_program->setUniformValue("normal_m",(sc*model).normalMatrix());
     m_program->setUniformValue("model",(sc*model));
     glFrontFace(GL_CW);
     floor_tex->bind();
     m_program->setUniformValue("texture",0);
     glDrawArrays(GL_QUADS,start_floor,floor.size());

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(2);
    m_program->disableAttributeArray(3);
    m_program->release();
}


void Scene::mouseMoveEvent(QMouseEvent *event){
    start.setX(event->x()-start.x());
    start.setY(start.y()-event->y());

    if(mouse_flag){
        this->cam->changeYawAndPitch(start.x(),start.y());
    }


    start=event->pos();
    update();
}

void Scene::keyPressEvent(QKeyEvent *event){
    keys.insert(event->key());

}

void Scene::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()==false)keys.remove(event->key());

}


QOpenGLTexture* Scene::initTexture(const char *nof){
    QOpenGLTexture * texture = new QOpenGLTexture(QImage(nof).mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

    return texture;
}

QOpenGLTexture *Scene::initTexture(QImage img)
{
    QOpenGLTexture * texture = new QOpenGLTexture(img.mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

    return texture;
}

void Scene::initTextures(){
    angle_tex=initTexture(":/angle.bmp");
    cube_tex=initTexture(":/cube.bmp");
    floor_tex=initTexture(":/floor.bmp");
}
