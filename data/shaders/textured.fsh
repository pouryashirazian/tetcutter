uniform sampler2D sampler2d;

varying vec3 N;
varying vec3 V;
varying vec2 T;


void main(void)
{
	gl_FragColor = texture2D(sampler2d, T);
}
