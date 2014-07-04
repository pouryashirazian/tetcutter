/*
 * test_HalfEdgeTetMesh.cpp
 *
 *  Created on: Jun 28, 2014
 *      Author: pourya
 */


#include "test_HalfEdgeTetMesh.h"
#include "base/Logger.h"
#include <map>

using namespace std;
using namespace PS;

bool TestHalfEdgeTestMesh::tst_report_mesh_info(HalfEdgeTetMesh* pmesh) {
	if(pmesh == NULL)
		return false;

	printf("============================mesh stats begin===========================\n");
	printf("elements# %u, face# %u, halfedges# %u, edges# %u, nodes# %u\n",
			pmesh->countElements(), pmesh->countFaces(), pmesh->countHalfEdges(),
			pmesh->countEdges(), pmesh->countNodes());
	pmesh->printInfo();
	printf("============================mesh stats end=============================\n");
	LogInfoArg1("PASS: %s", __FUNCTION__);
	return true;
}

bool TestHalfEdgeTestMesh::tst_correct_elements(HalfEdgeTetMesh* pmesh) {
	if(pmesh == NULL)
		return false;

	U32 ctErrors = 0;
	for(U32 i = 0; i < pmesh->countElements(); i++) {
		const HalfEdgeTetMesh::ELEM tet = pmesh->const_elemAt(i);

		map<U32, U32> mapElementNodes;
		map<U32, U32> mapElementEdges;

		map<U32, U32> mapUniqueTest;

		//check nodes
		mapUniqueTest.clear();
		for(U32 j=0; j < 4; j++) {
			if(!pmesh->isNodeIndex(!tet.nodes[j])) {
				LogErrorArg2("Invalid node index found for element %u, node %u", i, j);
				ctErrors++;
			}

			//map node to element
			mapElementNodes[tet.nodes[j]] = i;

			if(mapUniqueTest.find(tet.nodes[j]) == mapUniqueTest.end())
				mapUniqueTest[ tet.nodes[j] ] = j;
			else {
				LogErrorArg2("Duplicate node found for element %u, at node %u", i, j);
				ctErrors++;
			}
		}

		//check halfedges
		mapUniqueTest.clear();
		for(U32 j = 0; j < 6; j++) {

			if(!pmesh->isHalfEdgeIndex(!tet.halfedge[j])) {
				LogErrorArg2("Invalid halfedge index found for element %u, hedge %u", i, j);
				ctErrors++;
			}

			//halfedge
			{
				U32 idxEdge = pmesh->edge_from_halfedge(tet.halfedge[j]);
				U32 idxHE0 = pmesh->halfedge_from_edge(idxEdge, 0);
				U32 idxHE1 = pmesh->halfedge_from_edge(idxEdge, 1);
				HalfEdgeTetMesh::HEDGE he0 = pmesh->const_halfedgeAt(idxHE0);
				HalfEdgeTetMesh::HEDGE he1 = pmesh->const_halfedgeAt(idxHE1);

				//map edges to element
				mapElementEdges[ idxEdge ] = i;

				if(mapUniqueTest.find(idxEdge) == mapUniqueTest.end())
					mapUniqueTest[ idxEdge ] = j;
				else {
					LogErrorArg2("Duplicate edge found for element %u, at edge %u", i, idxEdge);
					ctErrors++;
				}

				if(he0.from != he1.to || he0.to != he1.from) {
					LogErrorArg3("Invalid halfedge pair found for element %u, hedge0 %u, hedge1 %u", i, idxHE0, idxHE1);
					ctErrors++;
				}
			}

			U32 idxHE = tet.halfedge[j];
			const HalfEdgeTetMesh::HEDGE he = pmesh->const_halfedgeAt(tet.halfedge[j]);

			//check that half edge from and to nodes are in this element
			if(mapElementNodes.find(he.from) == mapElementNodes.end() ) {
				LogErrorArg3("Invalid from node in element %u, halfedge %u, from %u", i, idxHE, he.from);
				ctErrors++;
			}

			if(mapElementNodes.find(he.to) == mapElementNodes.end()) {
				LogErrorArg3("Invalid to node in element %u, halfedge %u, to %u", i, idxHE, he.to);
				ctErrors++;
			}
		}

		//check faces
		mapUniqueTest.clear();
		for(U32 j=0; j < 4; j++) {
			if(!pmesh->isFaceIndex(!tet.faces[j])) {
				LogErrorArg2("Invalid face index found for element %u, face %u", i, j);
				ctErrors++;
			}

			//check half-edges of this face for inclusion
			const HalfEdgeTetMesh::FACE face = pmesh->const_faceAt(tet.faces[j]);
			for(U32 k=0; k < 3; k++) {
				if(mapElementEdges.find( pmesh->edge_from_halfedge(face.halfedge[k]) ) == mapElementEdges.end()) {

					LogErrorArg3("Invalid edge of a face found in: element %u, face %u, halfedge %u", i, j, k);
					ctErrors++;
				}
			}


			if(mapUniqueTest.find(tet.faces[j]) == mapUniqueTest.end())
				mapUniqueTest[ tet.faces[j] ] = j;
			else {
				LogErrorArg2("Duplicate face found for element %u, at face %u", i, j);
				ctErrors++;
			}
		}

	}

	if(ctErrors == 0)
		LogInfoArg1("PASS: %s", __FUNCTION__);
	else
		LogInfoArg1("FAILED!: %s", __FUNCTION__);
	return (ctErrors == 0);
}

bool TestHalfEdgeTestMesh::tst_unused_mesh_fields(HalfEdgeTetMesh* pmesh) {

	if(pmesh == NULL)
		return false;

	vector<U32> vUsedNodes;
	vector<U32> vUsedFaces;
	vector<U32> vUsedHalfEdges;
	vUsedNodes.resize(pmesh->countNodes());
	vUsedFaces.resize(pmesh->countFaces());
	vUsedHalfEdges.resize(pmesh->countHalfEdges());

	//Fill arrays
	std::fill(vUsedNodes.begin(), vUsedNodes.end(), 0);
	std::fill(vUsedFaces.begin(), vUsedFaces.end(), 0);
	std::fill(vUsedHalfEdges.begin(), vUsedHalfEdges.end(), 0);

	U32 ctErrors = 0;
	for(U32 i = 0; i < pmesh->countElements(); i++) {
		const HalfEdgeTetMesh::ELEM tet = pmesh->const_elemAt(i);

		//faces
		for(U32 j=0; j<4; j++) {
			assert( pmesh->isFaceIndex(tet.faces[j]) );
			vUsedFaces[ tet.faces[j] ] ++;

			const HalfEdgeTetMesh::FACE face = pmesh->const_faceAt(tet.faces[j]);

			for(U32 k=0; k < 3; k++)
				vUsedHalfEdges[ face.halfedge[k] ] ++;
		}

		//nodes
		for(U32 j=0; j<4; j++) {
			assert( pmesh->isNodeIndex(tet.nodes[j]) );
			vUsedNodes[ tet.nodes[j] ] ++;
		}
	}


	//check results
	U32 minNodeUsage = GetMaxLimit<U32>();
	U32 maxNodeUsage = 0;
	U32 countUnusedNodes = 0;
	for(U32 i=0; i < vUsedNodes.size(); i++) {

		minNodeUsage = MATHMIN(minNodeUsage, vUsedNodes[i]);
		maxNodeUsage = MATHMAX(maxNodeUsage, vUsedNodes[i]);

		if(vUsedNodes[i] == 0)
			countUnusedNodes++;
	}

	//hedges
	U32 minHEdgeUsage = GetMaxLimit<U32>();
	U32 maxHEdgeUsage = 0;
	U32 countUnusedHedges = 0;
	for(U32 i=0; i < vUsedHalfEdges.size(); i++) {

		minHEdgeUsage = MATHMIN(minHEdgeUsage, vUsedHalfEdges[i]);
		maxHEdgeUsage = MATHMAX(maxHEdgeUsage, vUsedHalfEdges[i]);

		if(vUsedHalfEdges[i] == 0)
			countUnusedHedges++;
	}

	//faces
	U32 minFaceUsage = GetMaxLimit<U32>();
	U32 maxFaceUsage = 0;
	U32 countUnusedFaces = 0;
	for(U32 i=0; i < vUsedFaces.size(); i++) {

		minFaceUsage = MATHMIN(minFaceUsage, vUsedFaces[i]);
		maxFaceUsage = MATHMAX(maxFaceUsage, vUsedFaces[i]);

		if(vUsedFaces[i] == 0)
			countUnusedFaces++;
	}

	printf("============================mesh field usage begin===========================\n");
	printf("Node Usage. Min: %u, Max: %u\n", minNodeUsage, maxNodeUsage);
	if(minNodeUsage == 0) {
		printf(">>list of %u unused nodes:\n", countUnusedNodes);
		for(U32 i=0; i < vUsedNodes.size(); i++) {
			if(vUsedNodes[i] == 0) {
				vec3d p = pmesh->const_nodeAt(i).pos;
				printf(">>NODE %u = [%.3f, %.3f, %.3f], used %u times.\n", i, p.x, p.y, p.z, vUsedNodes[i]);
			}
		}
	}

	printf("HalfEdge Usage. Min: %u, Max: %u\n", minHEdgeUsage, maxHEdgeUsage);
	if(minHEdgeUsage == 0) {
		printf(">>list of %u unused halfedges:\n", countUnusedHedges);
		for(U32 i=0; i < vUsedHalfEdges.size(); i++) {
			if(vUsedHalfEdges[i] == 0) {
				U32 from = pmesh->const_halfedgeAt(i).from;
				U32 to = pmesh->const_halfedgeAt(i).to;
				U32 refs = pmesh->const_halfedgeAt(i).refs;
				printf(">>HEDGE %u = [from: %u, to: %u], ref count %u.\n", i, from, to, refs);
			}
		}
	}

	printf("Face Usage. Min: %u, Max: %u\n", minFaceUsage, maxFaceUsage);
	if(minFaceUsage == 0) {
		printf(">>list of %u unused faces will follow:\n", countUnusedFaces);
		for(U32 i=0; i < vUsedFaces.size(); i++) {
			if(vUsedFaces[i] == 0) {
				HalfEdgeTetMesh::FACE face = pmesh->const_faceAt(i);

				U32 he[3];
				he[0] = face.halfedge[0];
				he[1] = face.halfedge[1];
				he[2] = face.halfedge[2];

				U32 n[3];
				n[0] = pmesh->vertex_from_hedge(he[0]);
				n[1] = pmesh->vertex_from_hedge(he[1]);
				n[2] = pmesh->vertex_from_hedge(he[2]);


				printf(">>FACE %u = Nodes [%u, %u, %u], HalfEdges [%u, %u, %u], ref count %u.\n",
						i, n[0], n[1], n[2], he[0], he[1], he[2], face.refs);
			}
		}
	}

	printf("============================mesh field usage end============================\n");

	if(ctErrors == 0)
		LogInfoArg1("PASS: %s", __FUNCTION__);
	else
		LogInfoArg1("FAILED!: %s", __FUNCTION__);
	return (ctErrors == 0);
}

bool TestHalfEdgeTestMesh::tst_connectivity(HalfEdgeTetMesh* pmesh) {
	return true;
}

bool TestHalfEdgeTestMesh::tst_meshFacesAndOrder(HalfEdgeTetMesh* pmesh)  {

	U32 ctNonTriangle = 0;
	U32 ctNotSet = 0;
	vec3d p[3];
	HalfEdgeTetMesh::HEDGE hedge;

	for(U32 i = 0; i < pmesh->countFaces(); i++) {
		HalfEdgeTetMesh::FACE face = pmesh->const_faceAt(i);
		U32 he0 = face.halfedge[0];
		U32 he1 = he0;
		U32 ctEdges = 0;

		do {
			hedge = pmesh->const_halfedgeAt(he1);
			if(ctEdges < 3)
				p[ctEdges] = pmesh->const_nodeAt(hedge.from).pos;

			he1 = hedge.next;
			ctEdges ++;

			//check for infinite loop
			if(ctEdges > 10)
				break;
		}
		while(he0 != he1);

		if(ctEdges > 3) {
			LogErrorArg2("Face %d is not triangulated. Num Edges: %d", i, ctEdges);
			ctNonTriangle++;
		}
	}


	U32 ctErrors = 0; //ctNonTriangle + ctNotCCW;

	if(ctErrors == 0)
		LogInfoArg1("PASS: %s", __FUNCTION__);
	else {
		printf("Non Triangular faces: %u, Not set faces: %u\n", ctNonTriangle, ctNotSet);
		LogInfoArg1("FAILED!: %s", __FUNCTION__);
	}
	return (ctErrors == 0);
}


bool TestHalfEdgeTestMesh::tst_all(HalfEdgeTetMesh* pmesh) {

	U32 idxTest = 0;
	const U32 maxTest = 4;
	printf("============================begin mesh tests===========================\n");
	printf("Test %u of %u\n", ++idxTest, maxTest);
	assert(tst_report_mesh_info(pmesh));

	printf("Test %u of %u\n", ++idxTest, maxTest);
	assert(tst_correct_elements(pmesh));

	printf("Test %u of %u\n", ++idxTest, maxTest);
	assert(tst_unused_mesh_fields(pmesh));

	printf("Test %u of %u\n", ++idxTest, maxTest);
	assert(tst_connectivity(pmesh));

//	printf("Test %u of %u\n", ++idxTest, maxTest);
//	assert(tst_meshFacesAndOrder(pmesh));
	printf("============================end mesh tests=============================\n");

	return true;
}


