#version 410

in vec4 position;

out vec3 v_position;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;


void main()
{
    v_position = normalize(modelMatrix * position).xyz;
    
    gl_Position = modelViewProjectionMatrix * position;

}