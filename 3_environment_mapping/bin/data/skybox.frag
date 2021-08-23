#version 410

in vec3 v_position;

out vec4 FRAG_COLOR;

// these are passed in from OF programmable renderer
uniform samplerCube envMap;

void main ()
{
    FRAG_COLOR = texture(envMap, v_position);
}