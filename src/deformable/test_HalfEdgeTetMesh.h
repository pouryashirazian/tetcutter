/*
 * test_HalfEdgeTetMesh.h
 *
 *  Created on: Jun 28, 2014
 *      Author: pourya
 */

#ifndef TEST_HALFEDGETETMESH_H_
#define TEST_HALFEDGETETMESH_H_

#include "HalfEdgeTetMesh.h"

using namespace PS::FEM;

/*!
 * east test returns true if successful and false otherwise
 */
class TestHalfEdgeTestMesh {
public:

	static bool tst_report_mesh_info(HalfEdgeTetMesh* pmesh);

	static bool tst_correct_elements(HalfEdgeTetMesh* pmesh);

	static bool tst_unused_mesh_fields(HalfEdgeTetMesh* pmesh);

	static bool tst_connectivity(HalfEdgeTetMesh* pmesh);

	static bool tst_meshFacesAndOrder(HalfEdgeTetMesh* pmesh);

	static bool tst_all(HalfEdgeTetMesh* pmesh);
};


#endif /* TEST_HALFEDGETETMESH_H_ */
