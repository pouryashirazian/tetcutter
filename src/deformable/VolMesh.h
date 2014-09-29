/*
 * TetMesh.h
 *
 *  Created on: May 6, 2014
 *      Author: pourya
 */

#ifndef VOLMESH_H
#define VOLMESH_H

#include <base/Vec.h>
#include <base/Color.h>
#include "graphics/SGNode.h"
#include "VolMeshEntities.h"
#include <functional>
#include <set>

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
namespace MESH {

//To track changes made to the topology of the mesh the following events are generated:
// 1. Node added/Removed
// 2. Edge added/Removed
// 3. Face added/Removed
// 4. Element added/Removed



//template <typename T>
class VolMesh : public SGNode {
public:
	static const U32 INVALID_INDEX = -1;
	enum TopologyEvent {teAdded, teRemoved, teUpdated};

	enum ErrorCodes {
		err_op_failed = -1,
		err_elem_not_found = -2,
		err_face_not_found = -3,
		err_edge_not_found = -4,
		err_node_not_found = -5,
	};


	typedef std::function<void(NODE, U32 handle, TopologyEvent event)> OnNodeEvent;
	typedef std::function<void(EDGE, U32 handle, TopologyEvent event)> OnEdgeEvent;
	typedef std::function<void(FACE, U32 handle, TopologyEvent event)> OnFaceEvent;
	typedef std::function<void(CELL, U32 handle, TopologyEvent event)> OnCellEvent;
public:
	VolMesh();
	VolMesh(const VolMesh& other);
	VolMesh(U32 ctVertices, double* vertices, U32 ctElements, U32* elements);
	VolMesh(const vector<double>& vertices, const vector<U32>& elements);
	virtual ~VolMesh();

	//Topology events
	void setOnNodeEventCallback(OnNodeEvent f);
	void setOnEdgeEventCallback(OnEdgeEvent f);
	void setOnFaceEventCallback(OnFaceEvent f);
	void setOnElemEventCallback(OnCellEvent f);

	//Build
	bool setup(const vector<double>& vertices, const vector<U32>& elements);
	bool setup(U32 ctVertices, const double* vertices, U32 ctElements, const U32* elements);
	void cleanup();

	//Stats
	void printNodeInfo() const;
	void printEdgeInfo() const;
	void printFaceInfo() const;
	void printCellInfo() const;
	void printInfo() const;

	//Determinant
	double computeCellDeterminant(U32 idxCell) const;
	double computeCellVolume(U32 idxCell) const;
	vec3d  computeCellCentroid(U32 idxCell) const;


	double computeAspectRatio(U32 idxCell) const;
	double computeInscribedRadius(U32 idxCell) const;
	double computeCircumscribedRadius(U32 idxCell) const ;

	static double ComputeCircumscribedRadius(const vec3d v[4]);
	static double ComputeCellDeterminant(const vec3d v[4]);
	static double ComputeCellVolume(const vec3d v[4]);

	//Index control
	inline bool isCellIndex(U32 i) const { return (i < m_vCells.size());}
	inline bool isFaceIndex(U32 i) const { return (i < m_vFaces.size());}
	inline bool isEdgeIndex(U32 i) const { return (i < m_vEdges.size());}
	inline bool isNodeIndex(U32 i) const { return (i < m_vNodes.size());}

	//access
	bool getFaceNodes(U32 idxFace, U32 (&nodes)[3]) const;
	bool isNodeOfCell(U32 idxNode, U32 idxCell) const;
	bool isEdgeOfCell(U32 idxEdge, U32 idxCell) const;

	CELL& cellAt(U32 i);
	FACE& faceAt(U32 i);
	NODE& nodeAt(U32 i);
	EDGE& edgeAt(U32 i);

	const CELL& const_cellAt(U32 i) const;
	const FACE& const_faceAt(U32 i) const;
	const NODE& const_nodeAt(U32 i) const;
	const EDGE& const_edgeAt(U32 i) const;

	inline U32 countCells() const { return m_vCells.size();}
	inline U32 countFaces() const {return m_vFaces.size();}
	inline U32 countEdges() const {return m_vEdges.size();}
	inline U32 countNodes() const {return m_vNodes.size();}

	//edge-wise funcs
	bool edge_exists(U32 from, U32 to);
	U32 edge_handle(U32 from, U32 to);

	U32 edge_from_node(U32 idxEdge) const;
	U32 edge_to_node(U32 idxEdge) const;

	U32 get_node_neighbors(U32 idxNode, vector<U32>& nbors) const;


	//displace all nodes by a displacement array
	void displace(U32 countDegreesOfFreedom, const double * u);


	//topology modifiers

	//insertions
	U32 insert_node(const NODE& n);
	U32 insert_edge(const EDGE& e);
	U32 insert_face(U32 nodes[3]);
	bool insert_cell(const CELL& cell);
	bool insert_cell(U32 nodes[4]);

	//setters
	void set_edge(U32 idxEdge, U32 from, U32 to);
	void set_face(U32 idxFace, U32 edges[3]);

	//mesh disjoint parts
	int get_disjoint_parts(vector<vector<U32>>& cellgroups);
	void printParts();

	//schedule a cell removal at the next GC
	void schedule_remove_cell(U32 idxCell);

	//remove single cell, face, edge, node
	void remove_cell(U32 idxCell);
	void remove_face(U32 idxFace);
	void remove_edge(U32 idxEdge);
	void remove_node(U32 idxNode);

	//remove batch of cells, faces, edges and nodes
	template <class ContainerT>
	void remove_cells(const ContainerT& cells);

	template <class ContainerT>
	void remove_faces(const ContainerT& faces);

	template <class ContainerT>
	void remove_edges(const ContainerT& edges);

	template <class ContainerT>
	void remove_nodes(const ContainerT& nodes);



	//erases all objects marked removed
	void garbage_collection();


	/*!
	 * cuts an edge completely. Two new nodes are created at the point of cut with no hedges between them.
	 */
	bool cut_edge(int idxEdge, double distance, U32* poutIndexNP0 = NULL, U32* poutIndexNP1 = NULL);


	//algorithmic functions
	int getNodeIncidentEdges(U32 idxNode, vector<U32>& incidentEdges) const;
	int getNodeIncidentNodes(U32 idxNode, vector<U32>& incidentNodes) const;
	bool getCellFacesExpensive(U32 idxCell, U32 (&faces)[4]);
	bool getCellEdgesExpensive(U32 idxCell, U32 (&edges)[6]);

	//show individual entities
	void setElemToShow(U32 elem = INVALID_INDEX);
	U32 getElemToShow() const {return m_elemToShow;}
	void setNodeToShow(U32 idxNode = INVALID_INDEX);
	U32 getNodeToShow() const {return m_nodeToShow;}

	//flag on what to show
	void setDrawWireFrame(bool drawWireFrame) { m_drawWireFrameMesh = drawWireFrame;}
	bool getDrawWireFrame() const {return m_drawWireFrameMesh;}
	void setDrawNodes(bool drawNodes) { m_drawNodes = drawNodes;}
	bool getDrawNodes() const {return m_drawNodes;}

	//set base color
	Color getColor() const {return m_color;}
	void setColor(const Color& c) { m_color = c;}


	//draw
	void draw();
	void drawElement(U32 i) const;

	//aabb
	AABB computeAABB();


	//selects a node using a ray intersection test
	int selectNode(const Ray& ray) const;

	bool verbose() const { return m_verbose;}
	void setVerbose(bool b) { m_verbose = b;}

protected:
	void init();
	inline bool insertEdgeIndexToMap(U32 from, U32 to, U32 idxEdge);
	inline bool removeEdgeIndexFromMap(U32 from, U32 to);


	inline EdgeKey computeEdgeKey(U32 idxEdge) const;

	inline bool face_exists_by_edges(U32 edges[3]) const;
	inline bool face_exists_by_nodes(U32 nodes[3]);

	U32 face_handle_by_edges(U32 edges[3]) const;
	U32 face_handle_by_nodes(U32 nodes[3]);

	//incident entities
	template <class ContainerT>
	int get_incident_cells(const ContainerT& in_faces, set<U32>& out_cells) const;

	template <class ContainerT>
	int get_incident_faces(const ContainerT& in_edges, set<U32>& out_faces) const;

	template <class ContainerT>
	int get_incident_edges(const ContainerT& in_nodes, set<U32>& out_edges) const;


	bool test_cell_topology(U32 idxCell);
	bool test_cells_topology();
	bool test_incident_edges();
	bool test_incident_faces() const;
	bool test_incident_cells() const;

	bool test_incidents();

	AABB computeNodalAABB() const;
protected:
	//remove core functions
	void remove_cell_core(U32 idxCell);
	void remove_face_core(U32 idxFace);
	void remove_edge_core(U32 idxEdge);
	void remove_node_core(U32 idxNode);

protected:
	U32 m_elemToShow;
	U32 m_nodeToShow;
	bool m_verbose;
	bool m_drawWireFrameMesh;
	bool m_drawNodes;
	Color m_color;

	//topology events
	OnNodeEvent m_fOnNodeEvent;
	OnEdgeEvent m_fOnEdgeEvent;
	OnFaceEvent m_fOnFaceEvent;
	OnCellEvent m_fOnElementEvent;

	//containers
	vector<CELL> m_vCells;
	vector<FACE> m_vFaces;
	vector<EDGE> m_vEdges;
	vector<NODE> m_vNodes;

	//marked cells to be deleted at the next GC
	vector<U32> m_pendingToDeleteCells;

	//top-down access
	vector< vector<U32> > m_incident_edges_per_node;
	vector< vector<U32> > m_incident_faces_per_edge;
	vector< vector<U32> > m_incident_cells_per_face;

	//maps a half-edge from-to pair to the corresponding hedge handle
	std::map< EdgeKey, U32 > m_mapEdgesIndex;

	//iterators
	typedef std::map< EdgeKey, U32 >::iterator MAPHEDGEINDEXITER;
	typedef std::map< EdgeKey, U32 >::const_iterator MAPHEDGEINDEXCONSTITER;
};

}
}

#endif /* TETMESH_H_ */
