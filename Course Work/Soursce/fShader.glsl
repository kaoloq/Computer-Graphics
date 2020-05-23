varying vec4 color;
in vec3 eyecoord;
in vec3 tnorm;
in vec4 FragPosLightSpace;
in vec2 v_texCoord;

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
uniform sampler2D shadowMap;

float unpack (vec4 colour)
{
    const vec4 bitShifts = vec4(1.0 / (256.0 * 256.0 * 256.0),
                                1.0 / (256.0 * 256.0),
                                1.0 / 256.0,
                                1);
    return dot(colour , bitShifts);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture2D(shadowMap, projCoords.xy);
    closestDepth=unpack(closestDepth);
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{    
    vec3 n = normalize( tnorm );
    vec3 s = normalize( vec3(l.position) - eyecoord );
    vec3 v = normalize(vec3(-eyecoord));
    vec3 r = reflect( -s, n );


    vec3 ambient=l.la*material.ka;

    float sDotN=max(dot(s,tnorm),0.0f);

    vec3 diffuse=l.ld*material.kd*sDotN;

    vec3 spec=l.ls*material.ks*pow(max( dot(r,v), 0.0 ), material.Shininess);

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec4 color_t=texture2D(texture,v_texCoord);

    gl_FragColor=color_t*vec4(ambient+spec+diffuse,1.0f);
}
