/*
 * SGRenderMask.cpp
 *
 *  Created on: Aug 31, 2014
 *      Author: pourya
 */

#include "SGRenderMask.h"
#include "selectgl.h"

namespace PS {
namespace SG {

SGRenderMask::SGRenderMask() {
	m_lpTex = NULL;
}

SGRenderMask::SGRenderMask(GLTexture* lpTex) {
	m_lpTex = lpTex;
	setup();
}

void SGRenderMask::setup() {
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	float w = vp[2];
	float h = vp[3];

    Geometry g;
    g.init();
    g.addVertex(vec3f(0, 0, 0));
    g.addVertex(vec3f(w, 0, 0));
    g.addVertex(vec3f(w, h, 0));
    g.addVertex(vec3f(0, h, 0));


    g.addTexCoord(vec2f(0, 0));
    g.addTexCoord(vec2f(1, 0));
    g.addTexCoord(vec2f(1, 1));
    g.addTexCoord(vec2f(0, 1));
    g.addTriangle(vec3u32(0, 3, 2));
    g.addTriangle(vec3u32(0, 2, 1));
    g.addPerVertexColor(vec4f(1,1,1,1));
    SGMesh::setup(g);

    setName("rendermask");
}

SGRenderMask::~SGRenderMask() {
	SGMesh::cleanup();
}

void SGRenderMask::draw() {
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	//Make ortho projection
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, vp[2], vp[3], 0, -1, 1);

	//goto origin
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	if(m_lpTex)
		m_lpTex->bind();

	GLMeshBuffer::draw();

	if(m_lpTex)
		m_lpTex->unbind();


	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//important to change the state back to modelview
	glMatrixMode(GL_MODELVIEW);
}
}
}
