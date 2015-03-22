//Phong Vertex Shader
varying vec3 N;
varying vec3 V;
uniform vec3 camPos;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
   	N = gl_Normal;
   	if(dot(camPos, N) < 0.0)
   		N = N * -1.0;
   
    N = normalize(gl_NormalMatrix * N);
    V = vec3(gl_ModelViewMatrix * gl_Vertex);
}