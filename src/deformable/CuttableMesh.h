/*
 * TopologyModifier.h
 *
 *  Created on: Apr 12, 2014
 *      Author: pourya
 */

#ifndef CUTTABLEMESH_H_
#define CUTTABLEMESH_H_

//#include "vegafem/include/tetMesh.h"
#include "graphics/SGMesh.h"
#include "HalfEdgeTetMesh.h"
#include "TetSubdivider.h"
#include "base/Vec.h"


using namespace PS::MATH;
using namespace PS::FEM;

namespace PS {

class CuttableMesh : public SGNode /*, public TetMesh */ {
public:
	//CutEdge
	class CutEdge {
	public:
		vec3d pos;
		vec3d uvw;
		vec3d e0;
		vec3d e1;
		double t;
		U32 from;
		U32 to;
		U32 idxNP0;
		U32 idxNP1;

		CutEdge() {
			t = 0;
			from = to = idxNP0 = idxNP1 = HalfEdgeTetMesh::INVALID_INDEX;
		}

		CutEdge& operator = (const CutEdge& A) {
			pos = A.pos;
			uvw = A.uvw;
			e0 	= A.e0;
			e1 	= A.e1;
			t	= A.t;
			from = A.from;
			to = A.to;
			idxNP0 = A.idxNP0;
			idxNP1 = A.idxNP1;
			return (*this);
		}
	};

	//CutNode
	struct CutNode {
		vec3d pos;
		U32 idxNode;

		CutNode& operator = (const CutNode& A) {
			pos = A.pos;
			idxNode = A.idxNode;
			return (*this);
		}
	};

public:
	CuttableMesh(const HalfEdgeTetMesh& hemesh);
	CuttableMesh(const vector<double>& vertices, const vector<U32>& elements);
	CuttableMesh(int ctVertices, double* vertices, int ctElements, int* elements);
	virtual ~CuttableMesh();

	//distances
	double pointLineDistance(const vec3d& v1, const vec3d& v2, const vec3d& p);
	double pointLineDistance(const vec3d& v1, const vec3d& v2,
							 const double len2, const vec3d& p, double* outT = NULL);


	//draw
	void draw();

	//cutting
	void clearCutContext();
	int cut(const vector<vec3d>& bladePath0, const vector<vec3d>& bladePath1,
			vec3d sweptSurface[4], bool modifyMesh);

	//apply displacements
	void displace(double * u);

	//Access vertex neibors
	U32 countVertices() const;
	vec3d vertexRestPosAt(U32 i) const;
	vec3d vertexAt(U32 i) const;
	int findClosestVertex(const vec3d& query, double& dist, vec3d& outP) const;
	int countCompletedCuts() const {return m_ctCompletedCuts;}

	//Access to subdivider
	TetSubdivider* getSubD() const { return m_lpSubD;}

	//create a tetrahedra
	static CuttableMesh* CreateOneTetra();
	static CuttableMesh* CreateTwoTetra();

protected:
	void setup(int ctVertices, double* vertices, int ctElements, int* elements);

	//TODO: Sync physics mesh after cut

	//TODO: Sync vbo after synced physics mesh
private:
	HalfEdgeTetMesh* m_lpHEMesh;
	TetSubdivider* m_lpSubD;
	int m_ctCompletedCuts;

	//Cut Nodes
	std::map<U32, CutNode > m_mapCutNodes;
	typedef std::map<U32, CutNode >::iterator CUTNODEITER;

	//Cut Edges
	std::map<U32, CutEdge > m_mapCutEdges;
	typedef std::map<U32, CutEdge >::iterator CUTEDGEITER;
};


}





#endif /* CUTTABLEMESH_H_ */
