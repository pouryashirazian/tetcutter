/*
 * IScalpel.cpp
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#include <deformable/IScalpel.h>
#include "base/Logger.h"
#include "graphics/SceneGraph.h"

namespace PS {
namespace MESH {

IAvatar::IAvatar(): IGizmoListener() {
	init();
}

IAvatar::IAvatar(CuttableMesh* pmesh): IGizmoListener() {
	init();
	m_lpTissue = pmesh;
}

IAvatar::~IAvatar() {
	// TODO Auto-generated destructor stub
	SGMesh::cleanup();
	clearCutContext();
}

void IAvatar::init() {
	setName("scalpel");
	m_fOnCutFinished = NULL;
	m_lpTissue = NULL;
	m_isToolActive = false;
	m_applyGripper = false;

	//Add a header
	TheSceneGraph::Instance().headers()->addHeaderLine("scalpel", "scalpel");
	TheSceneGraph::Instance().headers()->addHeaderLine("volmesh", "volmesh");
}

void IAvatar::grip() {
	m_applyGripper = true;
}

void IAvatar::setTissue(CuttableMesh* tissue) {
	m_lpTissue = tissue;
}

void IAvatar::mousePress(int button, int state, int x, int y) {
	if (button == ArcBallCamera::mbRight) {
		LogInfo("Right clicked cleared cut context!");
		clearCutContext();
		return;
	}

	if (button != ArcBallCamera::mbLeft)
		return;

	//Down = Start
	if (state == 0) {
		if (m_lpTissue) {
			m_isToolActive = true;
			TheSceneGraph::Instance().headers()->updateHeaderLine("scalpel",
					"scalpel: start cutting");
		}
	} else {
		//Up = Stop
		if (m_lpTissue) {
			m_isToolActive = false;
			TheSceneGraph::Instance().headers()->updateHeaderLine("scalpel",
					"scalpel: end cutting");
		}
	}
}

void IAvatar::updateVolMeshInfoHeader() const {

	if(m_lpTissue == NULL)
		return;

	char chrMsg[1024];
	sprintf(chrMsg, "VolMesh [Nodes# %u, Edges# %u, Faces# %u, Cells# %u]",
				m_lpTissue->countNodes(),
				m_lpTissue->countEdges(),
				m_lpTissue->countFaces(),
				m_lpTissue->countCells());

	TheSceneGraph::Instance().headers()->updateHeaderLine("volmesh", AnsiStr(chrMsg));
}

} /* namespace MESH */
} /* namespace PS */
