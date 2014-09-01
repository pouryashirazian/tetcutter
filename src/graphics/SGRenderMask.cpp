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

	SGQuad::setup()
}

SGRenderMask::~SGRenderMask() {

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

	int hw = vp[2] / 2;
	int hh = vp[3] / 2;

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_BLEND);

	glColor4f(0,0,0, 1);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, hh);
		glVertex2i(hw, hh);
		glVertex2i(hw, 0);
	glEnd();


//	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//important to change the state back to modelview
	glMatrixMode(GL_MODELVIEW);
}
}
}
