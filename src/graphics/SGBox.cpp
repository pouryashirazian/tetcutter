/*
 * SceneBox.cpp
 *
 *  Created on: Mar 10, 2013
 *      Author: pourya
 */

#include "SGBox.h"
#include "selectgl.h"

namespace PS {
namespace SG {

SGBox::SGBox() {
	setup(vec3f(-4, -4, -4), vec3f(4, 4, 4));
}


SGBox::SGBox(const vec3f& lo, const vec3f& hi) {
	setup(lo, hi);
}

SGBox::~SGBox() {
	GLMeshBuffer::cleanup();
}

void SGBox::setup(const vec3f& lo, const vec3f& hi) {
    m_lo = lo;
    m_hi = hi;
	float l = lo.x; float r = hi.x;
	float b = lo.y; float t = hi.y;
	float n = lo.z; float f = hi.z;

	float vertices [][3] = {{l, b, f}, {l, t, f}, {r, t, f},
							  {r, b, f}, {l, b, n}, {l, t, n},
							  {r, t, n}, {r, b, n}};

	vector<float> arrVertices;
	arrVertices.resize(24 * 3);
	vector<float> arrNormals;
	arrNormals.resize(24 * 3);
	/*
	U32 indices[24] = {0, 3, 2, 1, 4, 5, 6, 7,
					   3, 0, 4, 7, 1, 2, 6, 5,
					   2, 3, 7, 6, 5, 4, 0, 1};

	*/

	U32 indices[24] = {1, 2, 3, 0, 7, 6, 5, 4,
					   7, 4, 0, 3, 5, 6, 2, 1,
					   6, 7, 3, 2, 1, 0, 4, 5};

	vec3f center = (lo + hi) * 0.5f;
	for(int i=0; i<6; i++) {
		for(int j=0; j<4; j++)
		{
			int idxVertex = i*4 + j;

			vec3f v(vertices[indices[idxVertex]]);
			arrVertices[idxVertex * 3] = v.x;
			arrVertices[idxVertex * 3 + 1] = v.y;
			arrVertices[idxVertex * 3 + 2] = v.z;

			vec3f n = (center - v);
			n.normalize();
			arrNormals[idxVertex * 3] = n.x;
			arrNormals[idxVertex * 3 + 1] = n.y;
			arrNormals[idxVertex * 3 + 2] = n.z;
		}
	}

	vec4f red(1,0,0,1);
	vec4f green(0,1,0,1);
	vec4f white(0.89, 0.88, 0.85, 1);
	vec4f gray(0.5, 0.5, 0.5, 1);
	vec4f colorSet[6];
	colorSet[0] = white;
	colorSet[1] = gray;
	colorSet[2] = gray;
	colorSet[3] = white;
	colorSet[4] = green;
	colorSet[5] = red;

	//Setup colors buffer
	vector<float> arrColors;
	arrColors.resize(24 * 4);

	//Face
	for(int i=0; i<6; i++) {
		//Vertex
		for(int j=0; j<4; j++)
		{
			int idxVertex = i*4 + j;
			vec4f color = colorSet[i];
			arrColors[idxVertex * 4] = color.x;
			arrColors[idxVertex * 4 + 1] = color.y;
			arrColors[idxVertex * 4 + 2] = color.z;
			arrColors[idxVertex * 4 + 3] = color.w;
		}
	}

	//Setup Mesh
	setupVertexAttribs(arrVertices, 3, mbtPosition);
	setupVertexAttribs(arrNormals, 3, mbtNormal);
	setupVertexAttribs(arrColors, 4, mbtColor);
	//setupPerVertexColor(vec4f(1,0,0,1), m_ctVertices, 4);
	m_ctFaceElements = m_ctVertices;
	m_faceMode = ftQuads;
}

void SGBox::draw() {
	glDisable(GL_LIGHTING);

	GLMeshBuffer::draw();

	glEnable(GL_LIGHTING);
}
/*

void DrawBox(const vec3f& lo, const vec3f& hi, const vec3f& color, float lineWidth)
{
	float l = lo.x; float r = hi.x;
	float b = lo.y; float t = hi.y;
	float n = lo.z; float f = hi.z;

	GLfloat vertices [][3] = {{l, b, f}, {l, t, f}, {r, t, f},
							  {r, b, f}, {l, b, n}, {l, t, n},
							  {r, t, n}, {r, b, n}};

	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(color.x, color.y, color.z);
		glLineWidth(lineWidth);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);
			glVertex3fv(vertices[0]); glVertex3fv(vertices[3]); glVertex3fv(vertices[2]); glVertex3fv(vertices[1]);
			glVertex3fv(vertices[4]); glVertex3fv(vertices[5]); glVertex3fv(vertices[6]); glVertex3fv(vertices[7]);
			glVertex3fv(vertices[3]); glVertex3fv(vertices[0]); glVertex3fv(vertices[4]); glVertex3fv(vertices[7]);
			glVertex3fv(vertices[1]); glVertex3fv(vertices[2]); glVertex3fv(vertices[6]); glVertex3fv(vertices[5]);
			glVertex3fv(vertices[2]); glVertex3fv(vertices[3]); glVertex3fv(vertices[7]); glVertex3fv(vertices[6]);
			glVertex3fv(vertices[5]); glVertex3fv(vertices[4]); glVertex3fv(vertices[0]); glVertex3fv(vertices[1]);
		glEnd();
	glPopAttrib();
}

*/

}
}
