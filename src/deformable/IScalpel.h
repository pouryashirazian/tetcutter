/*
 * IScalpel.h
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#ifndef ISCALPEL_H_
#define ISCALPEL_H_

#include <functional>
#include <graphics/Gizmo.h>
#include <graphics/SGMesh.h>
#include "deformable/CuttableMesh.h"

#define MAX_SCALPEL_TRAJECTORY_ANGLE  60.0
#define MAX_SCALPEL_TRAJECTORY_NODES 1024

namespace PS {
namespace MESH {

class IAvatar: public SG::SGMesh, public SG::IGizmoListener {
public:
	typedef std::function<void()> OnCutFinished;

	IAvatar();
	IAvatar(CuttableMesh* pmesh);
	virtual ~IAvatar();

	//cleanup
	virtual void clearCutContext() {}

	//Tool
	void setOnCutFinishedEventHandler(OnCutFinished f) {m_fOnCutFinished = f;}
	void setTissue(CuttableMesh* tissue);
	virtual void grip();
	bool isGripActive() const {return m_applyGripper;}

	//Set Tool Active flag
	bool isActive() const {return m_isToolActive;}
	void updateVolMeshInfoHeader() const;

	//From Gizmo Manager
	virtual void mousePress(int button, int state, int x, int y);

protected:
	void init();

protected:
	bool m_isToolActive;
	bool m_applyGripper;
	OnCutFinished m_fOnCutFinished;


	CuttableMesh* m_lpTissue;
};

} /* namespace MESH */
} /* namespace PS */

#endif /* ISCALPEL_H_ */
