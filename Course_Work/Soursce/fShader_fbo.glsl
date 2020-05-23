varying vec4 color;
in vec3 eyecoord;
in vec3 tnorm;
//in vec2 v_texCoord;

struct LightInfo{
    vec4 position;
    vec3 la; //amb color
    vec3 ld;
    vec3 ls;
};

uniform LightInfo l;

struct MaterialInfo{
    vec3 ka; //amb str
    vec3 kd;
    vec3 ks;
    float Shininess;
};

uniform MaterialInfo material;
uniform sampler2D texture;

vec4 pack( float depth )
{
    const vec4 bitSh = vec4( 256.0 * 256.0 * 256.0,
                             256.0 * 256.0,
                             256.0,
                             1.0 );
    const vec4 bitMsk = vec4( 0.0,
                              1.0 / 256.0,
                              1.0 / 256.0,
                              1.0 / 256.0 );
    vec4 comp = fract( depth * bitSh );
    comp -= comp.xxyz * bitMsk;
    return comp;
}

void main()
{
    float normalizedZ = gl_FragCoord.z / gl_FragCoord.w;
    normalizedZ = ( normalizedZ + 1.0 ) / 2.0;
    gl_FragColor = pack( normalizedZ );
}
