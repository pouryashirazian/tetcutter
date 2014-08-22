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
#include "VolMesh.h"
#include "TetSubdivider.h"
#include "base/Vec.h"


using namespace PS::MATH;
using namespace PS::MESH;

namespace PS {

class CuttableMesh : public VolMesh {
public:
	//CutEdge
	class CutEdge {
	public:
		double t;
		vec3d pos;
		vec3d uvw;
		U32 idxNP0;
		U32 idxNP1;


		CutEdge() {
			t = 0;
			idxNP0 = idxNP1 = VolMesh::INVALID_INDEX;
		}

		CutEdge& operator = (const CutEdge& A) {
			t	= A.t;
			pos = A.pos;
			uvw = A.uvw;
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
	CuttableMesh(const VolMesh& volmesh);
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
	int cut(const vector<vec3d>& bladePath0,
			const vector<vec3d>& bladePath1,
			const vector<vec3d>& sweptSurface,
			bool modifyMesh);

	//Access vertex neibors
	vec3d vertexRestPosAt(U32 i) const;
	int findClosestVertex(const vec3d& query, double& dist, vec3d& outP) const;
	int countCompletedCuts() const {return m_ctCompletedCuts;}

	//Access to subdivider
	TetSubdivider* getSubD() const { return m_lpSubD;}

	/*!
	 * splits the mesh parts using the sweep surface.
	 * @param vSweeptSurf
	 * @param dist
	 * @return
	 */
	bool splitParts(const vector<vec3d>& vSweeptSurf, double dist);

	//splitting
	bool getFlagSplitMeshAfterCut() const {return m_flagSplitMeshAfterCut;}
	void setFlagSplitMeshAfterCut(bool flag) { m_flagSplitMeshAfterCut = flag;}

	bool getFlagDetectCutNodes() const {return m_flagDetectCutNodes;}
	void setFlagDetectCutNodes(bool flag) { m_flagDetectCutNodes = flag;}

	bool getFlagDrawSweepSurf() const { return m_flagDrawSweepSurf;}
	void setFlagDrawSweepSurf(bool flag) { m_flagDrawSweepSurf = flag;}
protected:
	void setup();

	//TODO: Sync physics mesh after cut

	//TODO: Sync vbo after synced physics mesh
private:
	TetSubdivider* m_lpSubD;
	int m_ctCompletedCuts;
	bool m_flagSplitMeshAfterCut;
	bool m_flagDetectCutNodes;

	//sweep surfaces
	bool m_flagDrawSweepSurf;
	vector<double> m_vSweepSurfaces;

	//Cut Nodes
	std::map<U32, CutNode > m_mapCutNodes;
	typedef std::map<U32, CutNode >::iterator CUTNODEITER;

	//Cut Edges
	std::map<U32, CutEdge > m_mapCutEdges;
	typedef std::map<U32, CutEdge >::iterator CUTEDGEITER;
};


}





#endif /* CUTTABLEMESH_H_ */
