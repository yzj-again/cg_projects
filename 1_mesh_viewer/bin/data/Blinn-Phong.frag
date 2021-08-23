#version 410

in vec4 v_normal;  // original normal vector
in vec3 v_position_mv;  // Eye-coordinate position of vertex
in vec4 v_color;

out vec4 FRAG_COLOR;

struct lightData
{
    vec4 ambient;
    vec4 position;
    vec4 diffuse;
    vec4 specular;
};

// these are passed in from OF programmable renderer
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform lightData light;
uniform vec4 mat_ambient;
uniform vec4 mat_diffuse;
uniform float k_shininess;
uniform vec4 mat_specular;
uniform vec4 mat_emissive;


void directionalLight(in lightData light, in vec3 normal, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular)
{
    float nDotL;         // normal . light vector (reverse of light direction)
    float nDotH;         // normal . half vector
    float pf;             // power factor

    vec3 eye = -normalize(v_position_mv);  // the negative of a frag's position is the vector from the frag to the eye
    vec3 halfVector = normalize(vec3(light.position) + eye);

    nDotL = max(0.0, dot(normal, normalize(vec3(light.position))));
    nDotH = max(0.0, dot(normal, halfVector));

    pf = pow(nDotH, k_shininess);

    ambient += light.ambient.rgb;
    diffuse += light.diffuse.rgb * nDotL;
    if (nDotL > 0)
        specular += light.specular.rgb * pf;
    // specular += light.specular.rgb * pf;
}


void main ()
{
    vec3 ambient = vec3(0.0,0.0,0.0);
    vec3 diffuse = vec3(0.0,0.0,0.0);
    vec3 specular = vec3(0.0,0.0,0.0);

    vec3 normal_mv = normalize((normalMatrix * v_normal).xyz);  

    directionalLight(light, normal_mv, ambient, diffuse, specular);

    // with color
    // vec4 localColor = vec4(ambient,1.0) * v_color + vec4(diffuse,1.0) * v_color + vec4(specular,1.0) * mat_specular + mat_emissive;

    // without color
    vec4 localColor = vec4(ambient,1.0) * mat_ambient + vec4(diffuse,1.0) * mat_diffuse + vec4(specular,1.0) * mat_specular + mat_emissive;
    
    FRAG_COLOR = localColor;
}