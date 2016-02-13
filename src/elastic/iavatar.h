/*
 * IScalpel.h
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#ifndef IAVATAR_H_
#define IAVATAR_H_

#include <functional>
#include <scene/gizmo.h>
#include <scene/sgmesh.h>
#include "elastic/cuttablemesh.h"

#define MAX_SCALPEL_TRAJECTORY_ANGLE  60.0
#define MAX_SCALPEL_TRAJECTORY_NODES 1024

namespace ps {
namespace elastic {

class IAvatar: public SGMesh, public IGizmoListener {
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
    virtual void onStart() override;
    virtual void onStop() override;
    virtual void onReset() override;


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

#endif /* IAVATAR_H_ */
