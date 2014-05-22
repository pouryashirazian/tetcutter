#ifndef PS_GLSURFACE_H
#define PS_GLSURFACE_H

#include "base/MathBase.h"
//#include <GL/glew.h>

#define DEFAULT_SURFACE_WIDTH 512
#define DEFAULT_SURFACE_HEIGHT 512

/*!
* When programming for GPGPU there are times when you need to pass the result one rendering step to
* the next step. GLSurface is an interface for rendering to a texture
*/
namespace PS {
namespace GL {


class GLSurface
{
public:
	GLSurface();
	GLSurface(U32 w, U32 h);
	~GLSurface();

	void cleanup();

	//Attach surface to the render buffer target of the pipeline
	void attach();

	//Detach surface from the render buffer target
	void detach();

	//Draw a triangle in orthographic projects for testing the surface
	void testDrawTriangle();

	/*!
	*  Draws the output texture as a quad
	*/
	void drawAsQuad();


	U32 texture() const { return m_glTex[0];}
	U32 renderbuffer() const {return m_glRBO;}
	U32 width() const {return m_width;}
	U32 height() const {return m_height;}
private:
	void init(U32 w, U32 h);
	void initTextures(U32 w, U32 h);

private:
	U32 m_glTex[2];
	U32 m_glFBO[2];
	U32 m_glRBO;

	U32 m_width;
	U32 m_height;
};

}
}
#endif

