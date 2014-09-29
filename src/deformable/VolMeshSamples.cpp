/*
 * VolMeshSamples.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: hupf2020
 */

#include "VolMeshSamples.h"
#include "base/FlatArray.h"
#include "base/Logger.h"

using namespace PS;
using namespace PS::MESH;

VolMesh* VolMeshSamples::CreateOneTetra() {
	vector<double> vertices;
	vector<U32> elements;

	vec3d points[4];
	points[0] = vec3d(-1, 0, 0);
	points[1] = vec3d(0, 0, -2);
	points[2] = vec3d(1, 0, 0);
	points[3] = vec3d(0, 2, -1);

	vertices.resize(4 * 3);
	for (int i = 0; i < 4; i++) {
		points[i].store(&vertices[i * 3]);
	}

	elements.resize(4);
	elements[0] = 0;
	elements[1] = 1;
	elements[2] = 2;
	elements[3] = 3;

	VolMesh* tet = new VolMesh(vertices, elements);
	tet->setName("onetet");
	return tet;
}

VolMesh* VolMeshSamples::CreateTwoTetra() {
	vector<double> vFlatVertices;
	vector<U32> vFlatElements;

	{
		vector<vec3d> vertices;
		vertices.push_back(vec3d(-1, 0, 0));
		vertices.push_back(vec3d(1, 0, 0));
		vertices.push_back(vec3d(0, 0, -1));
		vertices.push_back(vec3d(0, 0, 1));
		vertices.push_back(vec3d(0, 2, 0));
		FlattenVec3<double>(vertices, vFlatVertices);
	}

	{
		vector< Vec4<U32> > elements;
		elements.push_back( Vec4<U32>(0, 2, 3, 4) );
		elements.push_back( Vec4<U32>(1, 2, 3, 4) );
		FlattenVec4<U32>(elements, vFlatElements);
	}

	VolMesh* tet = new VolMesh(vFlatVertices, vFlatElements);
	tet->setName("twotets");
	return tet;
}

VolMesh* VolMeshSamples::CreateTruthCube(int nx, int ny, int nz, double cellsize) {
	if(nx < 2 || ny < 2 || nz < 2) {
		LogError("Invalid input param to create a truth cube");
		return NULL;
	}

	vector< vec3d > vertices;
	vector< vec4u32 > elements;

	vec3d start = vec3d(- (double)(nx)/2.0, 0, - (double)(nz)/2.0) * (cellsize);

	//add all nodes
	for(int i=0; i < nx; i++) {
		for(int j=0; j < ny; j++) {
			for(int k=0; k < nz; k++) {
				vec3d v = start + vec3d(i, j, k) * cellsize;
				vertices.push_back(v);
			}
		}
	}

	//corner defs
	enum CellCorners {LBN = 0, LBF = 1, LTN = 2, LTF = 3, RBN = 4, RBF = 5, RTN = 6, RTF = 7};
	U32 cn[8];
	//add all elements
	for(int i=0; i < nx-1; i++) {
		for(int j=0; j < ny-1; j++) {
			for(int k=0; k < nz-1; k++) {
				//collect cell nodes
				cn[LBN] = i * ny * nz + j * nz + k;
				cn[LBF] = i * ny * nz + j * nz + k + 1;

				cn[LTN] = i * ny * nz + (j+1) * nz + k;
				cn[LTF] = i * ny * nz + (j+1) * nz + k + 1;

				cn[RBN] = (i+1) * ny * nz + j * nz + k;
				cn[RBF] = (i+1) * ny * nz + j * nz + k + 1;

				cn[RTN] = (i+1) * ny * nz + (j+1) * nz + k;
				cn[RTF] = (i+1) * ny * nz + (j+1) * nz + k + 1;

				//add elements
				elements.push_back(vec4u32(cn[LBN], cn[LTN], cn[RBN], cn[LBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LBF], cn[RBN]));
				elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LTF], cn[LBF]));
				elements.push_back(vec4u32(cn[RTN], cn[RBN], cn[LBF], cn[RBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LBF], cn[LTF], cn[RBF]));
				elements.push_back(vec4u32(cn[RTN], cn[LTF], cn[RTF], cn[RBF]));
			}
		}
	}

	//build the final mesh
	vector<double> vFlatVertices;
	vector<U32> vFlatElements;
	FlattenVec3<double>(vertices, vFlatVertices);
	FlattenVec4<U32>(elements, vFlatElements);


	AnsiStr strName = printToAStr("truthcube_%dx%dx%d", nx, ny, nz);
	VolMesh* cube = new VolMesh(vFlatVertices, vFlatElements);
	cube->setName(strName.cptr());
	return cube;
}

VolMesh* VolMeshSamples::CreateEggShell(int hseg, int vseg, double radius, double shelltickness) {
	if(hseg <= 0 || vseg <= 0) {
		LogErrorArg2("Invalid eggshell segments param: %u, %u", hseg, vseg);
		return NULL;
	}

	if(shelltickness >= radius) {
		LogErrorArg2("Invalid eggshell radius and thickness params: %.3f, %.3f", radius, shelltickness);
		return NULL;
	}

	vector< vec3d > vertices;
	vector< vec4u32 > elements;

	float vSegInv = 1.0f / (float)vseg;
	float hSegInv = 1.0f / (float)hseg;

	//Loop Over
	for(int v=0; v<vseg+1; v++) {
		double p = DEGTORAD(v * vSegInv * 180.0);

		for(int h=0; h<hseg; h++) {
			double o = DEGTORAD(h * hSegInv * 360.0);

			//vertex
			vec3d v1;
			v1.x = radius * sin(p) * sin(o);
			v1.z = radius * sin(p) * cos(o);
			v1.y = radius * cos(p);

			//Add Vertex
			vertices.push_back(v1);
		}
	}

	//layer 2
	U32 layeroffset = vertices.size();
	radius -= shelltickness;
	for(int v=0; v<vseg+1; v++) {
		double p = DEGTORAD(v * vSegInv * 180.0);

		for(int h=0; h<hseg; h++) {
			double o = DEGTORAD(h * hSegInv * 360.0);

			//vertex
			vec3d v1;
			v1.x = radius * sin(p) * sin(o);
			v1.z = radius * sin(p) * cos(o);
			v1.y = radius * cos(p);

			//Add Vertex
			vertices.push_back(v1);
		}
	}


	//Indices
	enum CellCorners {LBN = 0, LBF = 1, LTN = 2, LTF = 3, RBN = 4, RBF = 5, RTN = 6, RTF = 7};
	U32 cn[8];

	//Add all elements
	for(int v=0; v<vseg; v++) {
		for(int h=0; h<hseg; h++) {

			//top
			cn[LTN] = h + v*hseg;
			cn[LTF] = cn[LTN] + 1;
			cn[RTN] = h + (v+1)*hseg;
			cn[RTF] = cn[RTN] + 1;

			if(h == hseg-1) {
				cn[LTF] = v*hseg;
				cn[RTF] = (v+1)*hseg;
			}

			//bottom
			cn[LBN] = layeroffset + h + v*hseg;
			cn[LBF] = layeroffset + cn[LTN] + 1;
			cn[RBN] = layeroffset + h + (v+1)*hseg;
			cn[RBF] = layeroffset + cn[RTN] + 1;

			if(h == hseg-1) {
				cn[LBF] = layeroffset + v*hseg;
				cn[RBF] = layeroffset + (v+1)*hseg;
			}

			//add elements
			elements.push_back(vec4u32(cn[LBN], cn[LTN], cn[RBN], cn[LBF]));
			elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LBF], cn[RBN]));
			elements.push_back(vec4u32(cn[RTN], cn[LTN], cn[LTF], cn[LBF]));
			elements.push_back(vec4u32(cn[RTN], cn[RBN], cn[LBF], cn[RBF]));
			elements.push_back(vec4u32(cn[RTN], cn[LBF], cn[LTF], cn[RBF]));
			elements.push_back(vec4u32(cn[RTN], cn[LTF], cn[RTF], cn[RBF]));
		}
	}

	//remove slivers
	U32 i = 0;
	while(i < elements.size()) {
		//fetch vertices
		vec4u32 e = elements[i];
		vec3d v[4];
		for(U32 j=0; j < COUNT_CELL_NODES; j++)
			v[j] = vertices[e[j]];

		if(VolMesh::ComputeCellVolume(v) < 0.0001)
			elements.erase(elements.begin() + i);
		else
			i++;
	}

	//build the final mesh
	vector<double> vFlatVertices;
	vector<U32> vFlatElements;
	FlattenVec3<double>(vertices, vFlatVertices);
	FlattenVec4<U32>(elements, vFlatElements);

	AnsiStr strName = printToAStr("eggshell_h%d_v%d", hseg, vseg);
	VolMesh* eggshell = new VolMesh(vFlatVertices, vFlatElements);
	eggshell->setName(strName.cptr());
	return eggshell;
}
