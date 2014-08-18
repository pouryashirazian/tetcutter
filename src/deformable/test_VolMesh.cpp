/*
 * test_HalfEdgeTetMesh.cpp
 *
 *  Created on: Jun 28, 2014
 *      Author: pourya
 */


#include "test_VolMesh.h"
#include "base/Logger.h"
#include <map>

using namespace std;
using namespace PS;

bool TestVolMesh::tst_report_mesh_info(VolMesh* pmesh) {
	if(pmesh == NULL)
		return false;

	printf("============================mesh stats begin===========================\n");
	printf("elements# %u, face# %u, edges# %u, nodes# %u\n",
			pmesh->countCells(), pmesh->countFaces(),
			pmesh->countEdges(), pmesh->countNodes());
	pmesh->printInfo();
	printf("============================mesh stats end=============================\n");
	LogInfoArg1("PASS: %s", __FUNCTION__);
	return true;
}

bool TestVolMesh::tst_correct_elements(VolMesh* pmesh) {
	if(pmesh == NULL)
		return false;

	U32 ctErrors = 0;
	for(U32 i = 0; i < pmesh->countCells(); i++) {
		const CELL& cell = pmesh->const_cellAt(i);

		map<U32, U32> mapElementNodes;
		map<U32, U32> mapElementEdges;

		map<U32, U32> mapUniqueTest;

		//check nodes
		mapUniqueTest.clear();
		for(U32 j=0; j < 4; j++) {
			if(!pmesh->isNodeIndex(!cell.nodes[j])) {
				LogErrorArg2("Invalid node index found for element %u, node %u", i, j);
				ctErrors++;
			}

			//map node to element
			mapElementNodes[cell.nodes[j]] = i;

			if(mapUniqueTest.find(cell.nodes[j]) == mapUniqueTest.end())
				mapUniqueTest[ cell.nodes[j] ] = j;
			else {
				LogErrorArg2("Duplicate node found for element %u, at node %u", i, j);
				ctErrors++;
			}
		}

		//check edges
		mapUniqueTest.clear();
		for(U32 j = 0; j < 6; j++) {

			if(!pmesh->isEdgeIndex(!cell.edges[j])) {
				LogErrorArg2("Invalid edge index found for element %u, edge %u", i, j);
				ctErrors++;
			}

			U32 idxEdge = cell.edges[j];
			const EDGE& edge = pmesh->const_edgeAt(idxEdge);

			//edge
			{
				//map edges to element
				mapElementEdges[ idxEdge ] = i;

				if(mapUniqueTest.find(idxEdge) == mapUniqueTest.end())
					mapUniqueTest[ idxEdge ] = j;
				else {
					LogErrorArg2("Duplicate edge found for element %u, at edge %u", i, idxEdge);
					ctErrors++;
				}
			}

			//check that edge from and to nodes are in this element
			if(mapElementNodes.find(edge.from) == mapElementNodes.end() ) {
				LogErrorArg3("Invalid from node in element %u, edge %u, from %u", i, idxEdge, edge.from);
				ctErrors++;
			}

			if(mapElementNodes.find(edge.to) == mapElementNodes.end()) {
				LogErrorArg3("Invalid to node in element %u, edge %u, to %u", i, idxEdge, edge.to);
				ctErrors++;
			}
		}

		//check faces
		mapUniqueTest.clear();
		for(U32 j=0; j < 4; j++) {
			if(!pmesh->isFaceIndex(!cell.faces[j])) {
				LogErrorArg2("Invalid face index found for element %u, face %u", i, cell.faces[j]);
				ctErrors++;
			}

			//check edges of this face for inclusion
			const FACE& face = pmesh->const_faceAt(cell.faces[j]);
			for(U32 k=0; k < 3; k++) {
				if(mapElementEdges.find( face.edges[k] ) == mapElementEdges.end()) {

					LogErrorArg3("Invalid edge of a face found in: element %u, face %u, edge %u", i, cell.faces[j], face.edges[k]);
					ctErrors++;
				}
			}


			if(mapUniqueTest.find(cell.faces[j]) == mapUniqueTest.end())
				mapUniqueTest[ cell.faces[j] ] = j;
			else {
				LogErrorArg2("Duplicate face found for element %u, at face %u", i, cell.faces[j]);
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

bool TestVolMesh::tst_unused_mesh_fields(VolMesh* pmesh) {

	if(pmesh == NULL)
		return false;

	vector<U32> vUsedNodes;
	vector<U32> vUsedFaces;
	vector<U32> vUsedEdges;
	vUsedNodes.resize(pmesh->countNodes());
	vUsedFaces.resize(pmesh->countFaces());
	vUsedEdges.resize(pmesh->countEdges());

	//Fill arrays
	std::fill(vUsedNodes.begin(), vUsedNodes.end(), 0);
	std::fill(vUsedFaces.begin(), vUsedFaces.end(), 0);
	std::fill(vUsedEdges.begin(), vUsedEdges.end(), 0);

	U32 ctErrors = 0;
	for(U32 i = 0; i < pmesh->countCells(); i++) {
		const CELL tet = pmesh->const_cellAt(i);

		//faces
		for(U32 j=0; j<4; j++) {
			assert( pmesh->isFaceIndex(tet.faces[j]) );
			vUsedFaces[ tet.faces[j] ] ++;

			const FACE& face = pmesh->const_faceAt(tet.faces[j]);

			for(U32 k=0; k < 3; k++)
				vUsedEdges[ face.edges[k] ] ++;
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

	//edges
	U32 minEdgeUsage = GetMaxLimit<U32>();
	U32 maxEdgeUsage = 0;
	U32 countUnusedHedges = 0;
	for(U32 i=0; i < vUsedEdges.size(); i++) {

		minEdgeUsage = MATHMIN(minEdgeUsage, vUsedEdges[i]);
		maxEdgeUsage = MATHMAX(maxEdgeUsage, vUsedEdges[i]);

		if(vUsedEdges[i] == 0)
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



	printf("Face Usage. Min: %u, Max: %u\n", minFaceUsage, maxFaceUsage);
	if(minFaceUsage == 0) {
		printf(">>list of %u unused faces will follow:\n", countUnusedFaces);
		for(U32 i=0; i < vUsedFaces.size(); i++) {
			if(vUsedFaces[i] == 0) {
				const FACE& face = pmesh->const_faceAt(i);

				U32 edges[3];
				edges[0] = face.edges[0];
				edges[1] = face.edges[1];
				edges[2] = face.edges[2];

				U32 nodes[3];
				pmesh->getFaceNodes(i, nodes);

				printf(">>FACE %u = Nodes [%u, %u, %u], Edges [%u, %u, %u].\n",
						i, nodes[0], nodes[1], nodes[2], edges[0], edges[1], edges[2]);
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

bool TestVolMesh::tst_connectivity(VolMesh* pmesh) {
	return true;
}



bool TestVolMesh::tst_all(VolMesh* pmesh) {

	U32 idxTest = 0;
	const U32 maxTest = 4;
	printf("============================begin mesh tests===========================\n");
	printf("Test %u of %u\n", ++idxTest, maxTest);
	//tst_report_mesh_info(pmesh);

	printf("Test %u of %u\n", ++idxTest, maxTest);
	tst_correct_elements(pmesh);

	printf("Test %u of %u\n", ++idxTest, maxTest);
	tst_unused_mesh_fields(pmesh);

	printf("Test %u of %u\n", ++idxTest, maxTest);
	tst_connectivity(pmesh);

//	printf("Test %u of %u\n", ++idxTest, maxTest);
//	assert(tst_meshFacesAndOrder(pmesh));
	printf("============================end mesh tests=============================\n");

	return true;
}


