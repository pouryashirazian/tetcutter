/*
 * IScalpel.cpp
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#include "base/logger.h"
#include "elastic/iavatar.h"

#include "scene/sgengine.h"

namespace ps {
namespace elastic {

IAvatar::IAvatar(): IGizmoListener() {
	init();
}

IAvatar::IAvatar(CuttableMesh* pmesh): IGizmoListener() {
	init();
	m_lpTissue = pmesh;
}

IAvatar::~IAvatar() {
	// TODO Auto-generated destructor stub
    //SGMesh::clearAllBuffers();
	clearCutContext();
}

void IAvatar::init() {
	setName("scalpel");
	m_fOnCutFinished = NULL;
	m_lpTissue = NULL;
	m_isToolActive = false;
	m_applyGripper = false;

	//Add a header
    TheEngine::Instance().headers()->addHeaderLine("scalpel", "scalpel");
    TheEngine::Instance().headers()->addHeaderLine("volmesh", "volmesh");
}

void IAvatar::grip() {
	m_applyGripper = true;
}

void IAvatar::setTissue(CuttableMesh* tissue) {
	m_lpTissue = tissue;
	if(m_lpTissue)
		updateVolMeshInfoHeader();
}

void IAvatar::onStart() {
    if (m_lpTissue) {
        m_isToolActive = true;
        TheEngine::Instance().headers()->updateHeaderLine("scalpel",
                "scalpel: start cutting");

        LogInfo("Start cutting");
    }
}

void IAvatar::onStop() {
    if (m_lpTissue) {
        m_isToolActive = false;

        //count disjoint parts
        vector<vector<U32> > parts;
        U32 ctParts = m_lpTissue->get_disjoint_parts(parts);
        AnsiStr strMsg = printToAStr("scalpel: finished cut %u. disjoint parts#%u",
                                     (U32)m_lpTissue->countCompletedCuts(),
                                     ctParts);
        LogInfo("Finished cutting");

        TheEngine::Instance().headers()->updateHeaderLine("scalpel", strMsg);
    }
}

void IAvatar::onReset() {
    clearCutContext();
}
/*
void IAvatar::mousePress(ps::MouseButton button, MouseButtonState state, int x, int y) {
    if (button == MouseButton::mbRight) {
		LogInfo("Right clicked cleared cut context!");
		clearCutContext();
		return;
	}

    if (button != MouseButton::mbLeft)
		return;

	//Down = Start
    if (state == mbsDown) {
		if (m_lpTissue) {
			m_isToolActive = true;
            TheEngine::Instance().headers()->updateHeaderLine("scalpel",
					"scalpel: start cutting");

            LogInfo("Start cutting");
		}
	} else {
		//Up = Stop
		if (m_lpTissue) {
			m_isToolActive = false;

			//count disjoint parts
			vector<vector<U32> > parts;
			U32 ctParts = m_lpTissue->get_disjoint_parts(parts);
			AnsiStr strMsg = printToAStr("scalpel: finished cut %u. disjoint parts#%u",
										 (U32)m_lpTissue->countCompletedCuts(),
										 ctParts);
            LogInfo("Finished cutting");

            TheEngine::Instance().headers()->updateHeaderLine("scalpel", strMsg);
		}
	}
}
*/

void IAvatar::updateVolMeshInfoHeader() const {

	if(m_lpTissue == NULL)
		return;

	char chrMsg[MAX_STRING_BUFFER_LEN];
	sprintf(chrMsg, "VolMesh [Nodes# %u, Edges# %u, Faces# %u, Cells# %u]",
				m_lpTissue->countNodes(),
				m_lpTissue->countEdges(),
				m_lpTissue->countFaces(),
				m_lpTissue->countCells());

    TheEngine::Instance().headers()->updateHeaderLine("volmesh", AnsiStr(chrMsg));
}

} /* namespace MESH */
} /* namespace PS */
