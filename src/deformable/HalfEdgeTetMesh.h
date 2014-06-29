/*
 * TetMesh.h
 *
 *  Created on: May 6, 2014
 *      Author: pourya
 */

#ifndef WINGEDEDGETETMESH_H_
#define WINGEDEDGETETMESH_H_

#include "graphics/SGNode.h"
#include <functional>

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
using namespace std;

namespace PS {
namespace FEM {

//To track changes made to the topology of the mesh the following events are generated:
// 1. Node added/Removed
// 2. Edge added/Removed
// 3. Face added/Removed
// 4. Element added/Removed

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
		FaceKey(U64 k) { this->key = k; }
	    FaceKey(U32 node_handles[3], U32 count) {
	    	key = (node_handles[0] * count + node_handles[1]) * count + node_handles[2];
	    }

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


	enum TopologyEvent {teAdded, teRemoved};
	typedef std::function<void(HalfEdgeTetMesh::NODE, U32 handle, TopologyEvent event)> OnNodeEvent;
	typedef std::function<void(HalfEdgeTetMesh::EDGE, U32 handle, TopologyEvent event)> OnEdgeEvent;
	typedef std::function<void(HalfEdgeTetMesh::FACE, U32 handle, TopologyEvent event)> OnFaceEvent;
	typedef std::function<void(HalfEdgeTetMesh::ELEM, U32 handle, TopologyEvent event)> OnElementEvent;
public:
	HalfEdgeTetMesh();
	HalfEdgeTetMesh(const HalfEdgeTetMesh& other);
	HalfEdgeTetMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements);
	HalfEdgeTetMesh(const vector<double>& vertices, const vector<U32>& elements);
	virtual ~HalfEdgeTetMesh();

	//Create one tet
	static HalfEdgeTetMesh* CreateOneTet();

	//Topology events
	void setOnNodeEventCallback(OnNodeEvent f);
	void setOnEdgeEventCallback(OnEdgeEvent f);
	void setOnFaceEventCallback(OnFaceEvent f);
	void setOnElemEventCallback(OnElementEvent f);

	//Build
	bool setup(const vector<double>& vertices, const vector<U32>& elements);
	bool setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements);
	void cleanup();
	void printInfo() const;

	double computeDeterminant(U32 idxNodes[4]) const;
	static double ComputeElementDeterminant(const vec3d v[4]);

	//Index control
	inline bool isElemIndex(U32 i) const { return (i < m_vElements.size());}
	inline bool isFaceIndex(U32 i) const { return (i < m_vFaces.size());}
	inline bool isHalfEdgeIndex(U32 i) const { return (i < m_vHalfEdges.size());}
	inline bool isEdgeIndex(U32 i) const { return (i < countEdges());}
	inline bool isNodeIndex(U32 i) const { return (i < m_vNodes.size());}

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


	//topology modifiers
	int insert_element(const ELEM& e);
	int insert_element(U32 nodes[4]);
	void remove_element(U32 i);

	/*!
	 * splits the edge in the middle. Adds one new node along the edge and re-connects all half-edges along that.
	 */
	bool split_edge(int idxEdge, double t);

	/*!
	 * cuts an edge completely. Two new nodes are created at the point of cut with no hedges between them.
	 */
	bool cut_edge(int idxEdge, double distance, U32* poutIndexNP0 = NULL, U32* poutIndexNP1 = NULL);


	//algorithmic functions useful for many computational geometry projects
	int getFirstRing(int idxNode, vector<U32>& ringNodes) const;
	int getIncomingHalfEdges(int idxNode, vector<U32>& incomingHE) const;
	int getOutgoingHalfEdges(int idxNode, vector<U32>& outgoingHE) const;

	//checking
	void setElemToShow(U32 elem = INVALID_INDEX);
	U32 getElemToShow() const {return m_elemToShow;}
	bool checkMeshConnectivity() const;
	bool checkMeshFaceDirections() const;

	//serialize
	bool readVegaFormat(const AnsiStr& strFP);
	bool writeVegaFormat(const AnsiStr& strFP) const;


	//draw
	void draw();
	void drawElement(U32 i) const;

	//aabb
	AABB computeAABB();

private:
	void init();
	inline int insertHEdgeIndexToMap(U32 from, U32 to, U32 idxHE);
	inline int removeHEdgeIndexFromMap(U32 from, U32 to);
	inline bool halfedge_exists(U32 from, U32 to) const;
	U32 halfedge_handle(U32 from, U32 to);

protected:
	U32 m_elemToShow;

	//topology events
	OnNodeEvent m_fOnNodeEvent;
	OnEdgeEvent m_fOnEdgeEvent;
	OnFaceEvent m_fOnFaceEvent;
	OnElementEvent m_fOnElementEvent;

	//containers
	vector<ELEM> m_vElements;
	vector<FACE> m_vFaces;
	vector<HEDGE> m_vHalfEdges;
	vector<NODE> m_vNodes;

	//maps a facekey to the corresponding face handle
	std::map< FaceKey, U32 > m_mapFaces;

	//maps a half-edge from-to pair to the corresponding hedge handle
	std::map< pair<U32, U32>, U32 > m_mapHalfEdgesIndex;

	//iterator for half-edge index iter
	typedef std::map< pair<U32, U32>, U32 >::iterator MAPHEDGEINDEXITER;
	typedef std::map< pair<U32, U32>, U32 >::iterator MAPHEDGEINDEXCONSTITER;

	//iterator for face index iter
	typedef std::map< FaceKey, U32 >::iterator MAPFACEITER;

};

}
}

#endif /* TETMESH_H_ */
