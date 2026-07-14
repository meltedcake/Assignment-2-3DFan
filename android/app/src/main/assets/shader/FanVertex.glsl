#version 300 es

// one vbo, two sub-regions: positions at location 0, grey shade at location 1
layout(location = 0) in vec3  VertexPosition;
layout(location = 1) in float VertexShade;

uniform mat4 MODELVIEWPROJECTIONMATRIX;

out float Shade;

void main()
{
    Shade       = VertexShade;
    gl_Position = MODELVIEWPROJECTIONMATRIX * vec4(VertexPosition, 1.0);
}
