//Phong Vertex Shader
varying vec3 V;
varying float F;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    V = vec3(gl_ModelViewMatrix * gl_Vertex);
    F = gl_Vertex.w;
}