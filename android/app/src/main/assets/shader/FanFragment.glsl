#version 300 es
precision mediump float;

in float Shade;

// one colour per part, uploaded with glUniform3fv before each draw call
uniform vec3 PARTCOLOR;

out vec4 FragColor;

void main()
{
    FragColor = vec4(PARTCOLOR * Shade, 1.0);
}
