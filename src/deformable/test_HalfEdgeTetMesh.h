/*
 * test_HalfEdgeTetMesh.h
 *
 *  Created on: Jun 28, 2014
 *      Author: pourya
 */

#ifndef TEST_HALFEDGETETMESH_H_
#define TEST_HALFEDGETETMESH_H_

#include "VolMesh.h"

using namespace PS::MESH;

/*!
 * east test returns true if successful and false otherwise
 */
class TestVolMesh {
public:

	static bool tst_report_mesh_info(VolMesh* pmesh);

	static bool tst_correct_elements(VolMesh* pmesh);

	static bool tst_unused_mesh_fields(VolMesh* pmesh);

	static bool tst_connectivity(VolMesh* pmesh);

	static bool tst_all(VolMesh* pmesh);
};


#endif /* TEST_HALFEDGETETMESH_H_ */
