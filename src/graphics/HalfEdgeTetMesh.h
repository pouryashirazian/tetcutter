/*
 * TetMesh.h
 *
 *  Created on: May 6, 2014
 *      Author: pourya
 */

#ifndef WINGEDEDGETETMESH_H_
#define WINGEDEDGETETMESH_H_

#include "graphics/SGNode.h"

/*!Stories:
 * 1. Iterate over edges
 * 2. Setup by adding vertices and tet elements
 * 3. Store rest positions for all vertices
 * 4. Add vertices and split edges using new vertices
 * 5. Iterate over elements
 *
*/
using namespace PS;
using namespace PS::SG;

namespace PS {
namespace FEM {


//template <typename T>
class HalfEdgeTetMesh : public SGNode {
public:
	static const U32 INVALID_INDEX = -1;

	enum ErrorCodes {
		err_op_failed = -1,
		err_elem_not_found = -2,
		err_face_not_found = -3,
		err_edge_not_found = -4,
		err_node_not_found = -5,
	};

	//elements
	struct ELEM {
		U32 faces[4];
		U32 nodes[4];
		U32 halfedge[6];
		bool posDet;

		ELEM& operator = (const ELEM& A) {
			for(int i=0; i<4; i++) {
				faces[i] = A.faces[i];
				nodes[i] = A.nodes[i];
			}
			for(int i=0; i<6; i++)
				halfedge[i] = A.halfedge[i];
			posDet = A.posDet;
			return (*this);
		}
	};

	//faces
	struct FACE {
		U32 halfedge[3];

		FACE& operator = (const FACE& A) {
			for(int i=0; i<3; i++)
				halfedge[i] = A.halfedge[i];
			return (*this);
		}
	};

	//Key to access faces in a unique order
	struct FaceKey
	{
	    FaceKey(U32 f[3], U32 count) { key = (f[0] * count + f[1]) * count + f[2];}

	    static void order_lo2hi(U32& a, U32& b, U32& c) {
	    	if(a > b)
	    		swap(a, b);
	    	if(b > c)
	    		swap(b, c);
	    	if(a > b)
	    		swap(a, b);
	    }

	    bool operator<(const FaceKey& k) const { return key < k.key; }

	    bool operator>(const FaceKey& k) const { return key > k.key; }

	    U64 key;
	};

	//vertices
	struct NODE {
		vec3d pos;
		vec3d restpos;
		U32 outHE;

		NODE& operator = (const NODE& A) {
			pos = A.pos;
			restpos = A.restpos;
			outHE = A.outHE;
			return (*this);
		}
	};

	//edges
	class HEDGE{
	public:
		//Vertex
		U32 from, to;

		//Face
		U32 face;

		//HEDGES
		U32 prev, next, opposite;


		HEDGE() { init();}
		HEDGE(U32 from_, U32 to_) {
			init();
			from = from_;
			to = to_;
		};

		void init() {
			from = to = face =  INVALID_INDEX;
			prev = next = opposite = INVALID_INDEX;
		}

		HEDGE& operator = (const HEDGE& A) {

			from = A.from;
			to = A.to;
			face = A.face;
			prev = A.prev;
			next = A.next;
			opposite = A.opposite;

			return(*this);
		};
	};

	class EDGE {
	public:
		U32 from, to;
		U32 lface, rface;

		EDGE() { init();}
		EDGE(const HEDGE& e1, const HEDGE& e2) {
			setup(e1, e2);
		}

		void init() {
			from = to = lface = rface = INVALID_INDEX;
		}

		void setup(const HEDGE& e1, const HEDGE& e2) {
			assert(e1.from == e2.to && e1.to == e2.from);
			from = e1.from;
			to = e1.to;
			lface = e1.face;
			rface = e2.face;
		}

		EDGE& operator =(const EDGE& A) {
			from = A.from;
			to = A.to;
			lface = A.lface;
			rface = A.rface;

			return (*this);
		}
	};


public:
	HalfEdgeTetMesh();
	HalfEdgeTetMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements);
	HalfEdgeTetMesh(const vector<double>& vertices, const vector<U32>& elements);
	virtual ~HalfEdgeTetMesh();

	//Build
	bool setup(const vector<double>& vertices, const vector<U32>& elements);
	bool setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements);
	void cleanup();
	void printInfo() const;

	double computeDeterminant(U32 idxNodes[4]) const;
	static double computeElementDeterminant(const vec3d v[4]);

	//add/remove
	int insert_element(const ELEM& e);
	int insert_element(const U32 nodes[4]);
	void remove_element(U32 i);

	//access
	ELEM& elemAt(U32 i);
	FACE& faceAt(U32 i);
	HEDGE& halfedgeAt(U32 i);
	NODE& nodeAt(U32 i);
	EDGE edgeAt(U32 i) const;

	const ELEM& const_elemAt(U32 i) const;
	const FACE& const_faceAt(U32 i) const;
	const HEDGE& const_halfedgeAt(U32 i) const;
	const NODE& const_nodeAt(U32 i) const;

	inline U32 countElements() const { return m_vElements.size();}
	inline U32 countFaces() const {return m_vFaces.size();}
	inline U32 countHalfEdges() const {return m_vHalfEdges.size();}
	inline U32 countEdges() const {return m_vHalfEdges.size() / 2;}
	inline U32 countNodes() const {return m_vNodes.size();}

	//functions
	inline U32 next_hedge(U32 he) const { return m_vHalfEdges[he].next;}
	inline U32 prev_hedge(U32 he) const { return m_vHalfEdges[he].prev;}
	inline U32 opposite_hedge(U32 he) const { return m_vHalfEdges[he].opposite;}

	inline U32 vertex_from_hedge(U32 he) const { return m_vHalfEdges[he].from;}
	inline U32 vertex_to_hedge(U32 he) const {return m_vHalfEdges[he].to;}
	inline U32 halfedge_from_edge(U32 edge, U8 which) const { return edge * 2 + which;}
	inline U32 edge_from_halfedge(U32 he) const { return he / 2;}


	//algorithm
	//splits an edge e at parametric point t
	void displace(double * u);

	/*!
	 * splits the edge in the middle. Adds one new node along the edge and re-connects all half-edges along that.
	 */
	bool split_edge(int idxEdge, double t);

	/*!
	 * cuts an edge completely. Two new nodes are created at the point of cut with no hedges between them.
	 */
	bool cut_edge(int idxEdge, double t, U32* poutIndexNP0 = NULL, U32* poutIndexNP1 = NULL);
	int getFirstRing(int idxNode, vector<U32>& ringNodes) const;
	int getIncomingHalfEdges(int idxNode, vector<U32>& incomingHE) const;
	int getOutgoingHalfEdges(int idxNode, vector<U32>& outgoingHE) const;

	bool checkMeshConnectivity() const;
	bool checkMeshFaceDirections() const;

	//serialize
	bool readVegaFormat(const AnsiStr& strFP);
	bool writeVegaFormat(const AnsiStr& strFP) const;


	//draw
	void draw();
protected:
	vector<ELEM> m_vElements;
	vector<FACE> m_vFaces;
	vector<HEDGE> m_vHalfEdges;
	vector<NODE> m_vNodes;

	//Index control
	inline bool isElemIndex(U32 i) const { return (i < m_vElements.size());}
	inline bool isFaceIndex(U32 i) const { return (i < m_vFaces.size());}
	inline bool isHalfEdgeIndex(U32 i) const { return (i < m_vHalfEdges.size());}
	inline bool isEdgeIndex(U32 i) const { return (i < countEdges());}
	inline bool isNodeIndex(U32 i) const { return (i < m_vNodes.size());}

	AABB computeAABB();
};

}
}

#endif /* TETMESH_H_ */
