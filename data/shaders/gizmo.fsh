//#version 330 core
//#version 120
varying vec3 N;
varying vec3 V;

//layout(location = 2) uniform vec4 color;
uniform vec4 color;

void main(void)
{
    vec3 L = normalize(gl_LightSource[0].position.xyz - V);
    vec3 E = normalize(-V);
    vec3 R = normalize(-reflect(L, N));
    
    vec4 Ia = 0.5 * gl_LightSource[0].ambient * color;
    vec4 Id = (gl_LightSource[0].diffuse * color) * max(dot(N, L), 0.0);
    vec4 Is = (gl_LightSource[0].specular * vec4(0.8, 0.8, 0.8, 0.8) + 0.2 * color) * pow(max(dot(R, E), 0.0), 32.0);
  
    gl_FragColor = gl_FrontLightModelProduct.sceneColor + Ia + Id + Is;
}