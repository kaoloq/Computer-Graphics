#version 330 core

layout(location=0) in vec4 vertexAttr;
layout(location=1) in vec3 colorAttr;
layout(location=2) in vec3 normAttr;
layout(location=3) in vec2 texCoord;



uniform mat4 matrix;
uniform mat3 normal_m;
uniform mat4 modelview;
uniform mat4 model;
uniform mat4 lightmatrix;

out vec3 color;
out vec3 tnorm;
out vec3 eyecoord;
out vec4 FragPosLightSpace;

out vec2 v_texCoord;

vec4 plane=vec4(0,-1,0,0.15);

void main()
{


    tnorm=normalize(normal_m*normAttr);
    eyecoord=vec3(model*vertexAttr);
    FragPosLightSpace=lightmatrix*vec4(eyecoord,1.0f);
    gl_Position=matrix * vertexAttr;
    v_texCoord=texCoord;
    color=colorAttr;
}
