/*
 * test_HalfEdgeTetMesh.h
 *
 *  Created on: Jun 28, 2014
 *      Author: pourya
 */

#ifndef TEST_HALFEDGETETMESH_H_
#define TEST_HALFEDGETETMESH_H_

#include "CellularMesh.h"

using namespace PS::MESH;

/*!
 * east test returns true if successful and false otherwise
 */
class TestHalfEdgeTestMesh {
public:

	static bool tst_report_mesh_info(CellMesh* pmesh);

	static bool tst_correct_elements(CellMesh* pmesh);

	static bool tst_unused_mesh_fields(CellMesh* pmesh);

	static bool tst_connectivity(CellMesh* pmesh);

	static bool tst_meshFacesAndOrder(CellMesh* pmesh);

	static bool tst_all(CellMesh* pmesh);
};


#endif /* TEST_HALFEDGETETMESH_H_ */
