/*
 * Gizmo.h
 *
 *  Created on: Dec 28, 2013
 *      Author: pourya
 */

#ifndef GIZMO_H_
#define GIZMO_H_

#include "SGMesh.h"
#include "IMouseListener.h"
#include <loki/Singleton.h>
#include "ArcBallCamera.h"

using namespace PS;
using namespace PS::GL;

/*!
 * Create Affine widgets for controlling transformations or Avatar for interaction with model.
 */

namespace PS {
namespace SG {

enum GizmoAxis {axisX, axisY, axisZ, axisFree, axisCount};
enum GizmoType {gtTranslate, gtRotate, gtScale, gtAvatar, gtCount};

//Interface for Gizmo listeners
class IGizmoListener : public IMouseListener {
public:
	IGizmoListener();
	virtual ~IGizmoListener();
	virtual void onTranslate(const vec3f& delta, const vec3f& pos) {}
	virtual void onScale(const vec3f& delta, const vec3f& current) {}
	virtual void onRotate(const quatf& delta) {}

	bool registerListener();
	void unregisterListener();
protected:
	int m_id;
};

//Interface for all affine Gizmo Widgets
class GizmoInterface : public SGMesh {
public:
	GizmoInterface() {m_axis = axisFree;}
	virtual ~GizmoInterface() {}

    //Axis
    GizmoAxis axis() const {return m_axis;}
    void setAxis(GizmoAxis axis) {m_axis = axis;}
    virtual int setAxis(const Ray& r);
    
    vec4f axisColor(GizmoAxis a);
protected:
	GizmoAxis m_axis;
};

//Translate
class GizmoTranslate : public GizmoInterface {
public:
	GizmoTranslate();

	void draw();
    int intersect(const Ray& r);
protected:
	void setup();

private:
	SGMesh m_x;
	SGMesh m_y;
	SGMesh m_z;
};

//Scale
class GizmoScale : public GizmoInterface {
public:
	GizmoScale();
    
    void draw();
    int intersect(const Ray& r);
protected:
	void setup();

private:
	SGMesh m_x;
	SGMesh m_y;
	SGMesh m_z;
};

//Rotate
class GizmoRotate : public GizmoInterface {
public:
	GizmoRotate();

    void draw();
    int intersect(const Ray& r);
    int setAxis(const Ray& r);

    void rotate(GizmoAxis axis, float degree);
protected:
	void setup();

private:
	SGMesh m_x;
	SGMesh m_y;
	SGMesh m_z;
	SGMesh m_xs;
	SGMesh m_ys;
	SGMesh m_zs;

};

//Avatar
class GizmoAvatar : public GizmoInterface {
public:
	GizmoAvatar();

protected:
	void setup();
};

/*!
 * Gizmo Manager controls the UI Widgets for sketching, transforming, cutting and poking of
 * deformable models.
 */
class GizmoManager : public SGNode, public IMouseListener {
public:
	GizmoManager();
	virtual ~GizmoManager();

	void draw();
	void timestep();
    int intersect(const Ray& r);

    //Get
    GizmoType gizmoType() const {return m_gizmoType;}
    GizmoAxis axis() const {return m_gizmoAxis;}
    
    //Mouse
    void mousePress(int button, int state, int x, int y);
    void mouseMove(int x, int y);
    void mouseWheel(int button, int dir, int x, int y);

    
    //Set Gizmo Type and Axis
    void setAxis(const Ray& r);
    void setAxis(GizmoAxis axis);
	void setType(GizmoType gtype);
    
	//Return current Gizmo Type
	GizmoInterface* current() const {return m_lpGizmoCurrent;}

	//Set Node
	void setFocusedNode(SGNode* node);

	//Clients
	int registerClient(IGizmoListener* client);
	void unregisterClient(int id);

	static string AxisToStr(GizmoAxis axis);

	void cmdTranslate(const vec3f& increment);
	void cmdScale(const vec3f& increment);
	void cmdRotate(const vec3f& axis, float degreeIncrement);

protected:

private:
	GizmoTranslate* m_lpGizmoTranslate;
	GizmoScale* m_lpGizmoScale;
	GizmoRotate* m_lpGizmoRotate;
	GizmoAvatar* m_lpGizmoAvatar;

	GizmoInterface* m_lpGizmoCurrent;
	GizmoType m_gizmoType;
    GizmoAxis m_gizmoAxis;


    //Mouse State
    vec2i m_pressedPos;
    PS::ArcBallCamera::MouseButton m_button;
    PS::ArcBallCamera::ButtonState m_buttonState;

    //Register SGNode
    SGNode* m_lpFocusedNode;

    //Registered clients
    vector<IGizmoListener*> m_clients;
};

//Singleton for UI Gizmo Manager
typedef SingletonHolder<GizmoManager, CreateUsingNew, PhoenixSingleton> TheGizmoManager;



}
}


#endif /* GIZMO_H_ */
