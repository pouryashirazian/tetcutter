/*
 *  SGFloor.cpp
 *
 *  Created on: Mar 10, 2013
 *      Author: pourya
 */
#include <algorithm>
#include "SGFloor.h"
#include "selectgl.h"

namespace PS {
namespace SG {

SGFloor::SGFloor(int rows, int cols, float step) {

	vector<float> arrVertices;
	vector<float> arrColors;

	int effectiveRow = ((rows % 2) == 0)? rows + 1: rows;
	int effectiveCol = ((cols % 2) == 0)? cols + 1: cols;
	arrVertices.resize(2 * effectiveRow * 3 + 2 * effectiveCol * 3);
	arrColors.resize(2 * effectiveRow * 3 + 2 * effectiveCol * 3);
	std::fill(arrColors.begin(), arrColors.end(), 0.5f);

	int idxFirst = 0;
	int idxSecond = 3;
	float z = static_cast<float>(cols/2) * step;
	for(int i=-rows/2; i<=rows/2; i++) {
		arrVertices[idxFirst] = static_cast<float>(i) * step;
		arrVertices[idxFirst + 1] = 0.0f;
		arrVertices[idxFirst + 2] = -z;

		arrVertices[idxSecond] = static_cast<float>(i) * step;
		arrVertices[idxSecond + 1] = 0.0f;
		arrVertices[idxSecond + 2] = z;

		//Major Z Axis is Blue
		if( i == 0) {
			arrColors[idxFirst] = 0.0f;
			arrColors[idxFirst + 1] = 0.0f;
			arrColors[idxFirst + 2] = 1.0f;

			arrColors[idxSecond] = 0.0f;
			arrColors[idxSecond + 1] = 0.0f;
			arrColors[idxSecond + 2] = 1.0f;
		}

		idxFirst += 6;
		idxSecond += 6;
	}

	//idxFirst = effectiveRow * 6;
	//idxSecond = effectiveRow * 6 + 3;
	float x = static_cast<float>(rows/2) * step;
	for(int i=-cols/2; i<=cols/2; i++) {
		arrVertices[idxFirst] = -x;
		arrVertices[idxFirst + 1] = 0.0f;
		arrVertices[idxFirst + 2] = static_cast<float>(i) * step;

		arrVertices[idxSecond] = x;
		arrVertices[idxSecond + 1] = 0.0f;
		arrVertices[idxSecond + 2] = static_cast<float>(i) * step;

		//Major X Axis is Red
		if( i == 0) {
			arrColors[idxFirst] = 1.0f;
			arrColors[idxFirst + 1] = 0.0f;
			arrColors[idxFirst + 2] = 0.0f;

			arrColors[idxSecond] = 1.0f;
			arrColors[idxSecond + 1] = 0.0f;
			arrColors[idxSecond + 2] = 0.0f;
		}

		idxFirst += 6;
		idxSecond += 6;
	}

	//Set AABB
	m_aabb.set(vec3f(-0.5f * rows * step, 0.0, -0.5f * cols * step),
			   vec3f(+0.5f * rows * step, 0.0, +0.5f * cols * step));

	Geometry g;
	g.init(3, 3, 2, ftLines);
	g.addVertexAttribs(arrVertices, 3, mbtPosition);
	g.addVertexAttribs(arrColors, 3, mbtColor);
	setup(g);
}

SGFloor::~SGFloor() {
	GLMeshBuffer::cleanup();
}

void SGFloor::draw() {
	glDisable(GL_LIGHTING);

	SGMesh::draw();

	glEnable(GL_LIGHTING);
}

}
}

