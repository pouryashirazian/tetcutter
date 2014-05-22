#include "MorphingSphere.h"
#include <algorithm>
#include "base/MathBase.h"

namespace PS {
namespace SG {

MorphingSphere::MorphingSphere() {
	this->setup(2.0f, 8, 8);
}

MorphingSphere::MorphingSphere(float rho, int hseg, int vseg) {
	setup(rho, hseg, vseg);
}

MorphingSphere::~MorphingSphere() {
	GLMeshBuffer::cleanup();
}

void MorphingSphere::setup(float rho, int hseg, int vseg) {

	vector<float> vertices;
	vector<float> normals;
	vector<float> colors;
	vector<U32> indices;

	//Attribs
	vertices.reserve(hseg * vseg * 3);
	normals.reserve(hseg * vseg * 3);
	colors.reserve(hseg * vseg * 4);
	indices.reserve(hseg * vseg);

	float vSegInv = 1.0f / (float)vseg;
	float hSegInv = 1.0f / (float)hseg;

	//Loop Over
	for(int v=0; v<vseg+1; v++) {
		float p = DEGTORAD((float)v * vSegInv * 180.0f);

		for(int h=0; h<hseg; h++) {
			float o = DEGTORAD((float)h * hSegInv * 360.0f);

			vec3f v1;
			v1.x = rho * sin(p) * sin(o);
			v1.z = rho * sin(p) * cos(o);
			v1.y = rho * cos(p);

			vertices.push_back(v1.x);
			vertices.push_back(v1.y);
			vertices.push_back(v1.z);

			colors.push_back(0.7f);
			colors.push_back(0.7f);
			colors.push_back(0.7f);
			colors.push_back(0.7f);


			vec3f n = v1.normalized();
			normals.push_back(n.x);
			normals.push_back(n.y);
			normals.push_back(n.z);
		}
	}

	//Indices
	U32 t[4];
	for(int v=0; v<vseg; v++) {
		for(int h=0; h<hseg; h++) {
			t[0] = h + v*hseg;
			t[1] = t[0] + 1;
			t[2] = h + (v+1)*hseg;
			t[3] = t[2] + 1;

			if(h == hseg-1) {
				t[1] = v*hseg;
				t[3] = (v+1)*hseg;
			}

			//First Triangle
			indices.push_back(t[0]);
			indices.push_back(t[2]);
			indices.push_back(t[1]);

			//Second Triangle
			indices.push_back(t[1]);
			indices.push_back(t[2]);
			indices.push_back(t[3]);

		}
	}

	//Setup Mesh Buffers
	setupVertexAttribs(vertices, 3, mbtPosition);
	setupVertexAttribs(colors, 4, mbtColor);
	setupVertexAttribs(normals, 3, mbtNormal);
	setupIndexBufferObject(indices, ftTriangles);
	//setupVertexAttribs(arrColors, 3, mbtColor);
	//m_ctFaceElements = m_ctVertices;
	m_faceMode = ftLineLoop;
}

}
}
