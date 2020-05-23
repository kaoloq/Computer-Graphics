#include "axes.h"

Axes::Axes()
    :vert{0.0f,0.0f,0.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,2.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,2.0f},
      col{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f}
{

}

void Axes::drawAxis(QOpenGLShaderProgram* m_program)
{

    initializeOpenGLFunctions();


    m_program->bind();

    m_posAttr = m_program->attributeLocation( "vertexAttr" );
    m_colAttr = m_program->attributeLocation( "colorAttr" );


    m_program->setAttributeArray( m_posAttr, vert, 3 );
    m_program->setAttributeArray( m_colAttr, col, 3 );
    m_program->setAttributeValue(2,{0.0f,1.0f,0.0f});

    m_program->enableAttributeArray( m_posAttr );
    m_program->enableAttributeArray( m_colAttr );

    glDrawArrays( GL_LINES, 0, 6 );

    m_program->disableAttributeArray( m_posAttr);
    m_program->disableAttributeArray( m_colAttr );

    m_program->release();
}

