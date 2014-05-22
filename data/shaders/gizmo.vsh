//Phong Vertex Shader
//#version 330 core
#version 120
varying vec3 N;
varying vec3 V;
void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
    N = normalize(gl_NormalMatrix * gl_Normal);
    V = vec3(gl_ModelViewMatrix * gl_Vertex);
}