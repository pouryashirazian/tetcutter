/*
 * SGHeaders.cpp
 *
 *  Created on: Mar 8, 2014
 *      Author: pourya
 */

#include "base/Logger.h"
#include "SGHeaders.h"
#include "selectgl.h"
#include "GLFuncs.h"

namespace PS {
namespace SG {

SGHeaders::SGHeaders():SGNode() {
	setName("headers");
}

SGHeaders::~SGHeaders() {
	cleanup();
}

void SGHeaders::cleanup() {
	m_vHeaders.resize(0);
	m_hashHeaders.cleanup();
}

void SGHeaders::draw() {
	for(U32 i=0; i<m_vHeaders.size(); i++)
		DrawText(m_vHeaders[i].cptr(), 10, 20 + i * 15);
}

int SGHeaders::addHeaderLine(const AnsiStr& title, const AnsiStr& strInfo) {
	if(m_hashHeaders.has(strInfo.cptr())) {
		LogInfoArg1("Header: %s is already present.", title.cptr());
		return -1;
	}

	m_vHeaders.push_back(strInfo);
	int id = (int)m_vHeaders.size() - 1;
	m_hashHeaders.add(id, title.cptr());
	return id;
}

int SGHeaders::getHeaderId(const char* title) {
	return m_hashHeaders.get(title);
}

bool SGHeaders::removeHeaderLine(const char* title) {
	int id = m_hashHeaders.get(title);
	if(id >= 0) {
		m_vHeaders.erase(m_vHeaders.begin() + id);
		m_hashHeaders.remove(title);
	}
	return (id >= 0);
}

void SGHeaders::removeAllHeaders() {
	cleanup();
}

bool SGHeaders::updateHeaderLine(int id, const AnsiStr& strInfo) {
	if(id < 0 || id >= m_vHeaders.size())
		return false;

	m_vHeaders[id] = strInfo;
	return true;
}

bool SGHeaders::updateHeaderLine(const AnsiStr& title, const AnsiStr& strInfo) {
	return updateHeaderLine(getHeaderId(title.cptr()), strInfo);
}

void SGHeaders::DrawText(const char* chrText, int x, int y) {
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	//Make ortho projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, vp[2], vp[3], 0, -1, 1);

	//goto origin
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//draw text
	float clFont[] = { 0, 0, 1, 1 };
	// lighting and color mask
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    //disable lighting for proper text color
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glColor4fv(clFont);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*chrText)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *chrText);
        ++chrText;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//important to change the state back to modelview
	glMatrixMode(GL_MODELVIEW);
}

}
}
