#ifndef SGENGINE_H
#define SGENGINE_H

#include "tbb/tick_count.h"
#include "loki/Singleton.h"
#include "loki/Functor.h"
#include "loki/SmartPtr.h"
#include "base/copystack.h"
#include "base/resourcemanager.h"
#include "base/movingaverage.h"

#include "glbackend/glshadermanager.h"
#include "glbackend/gltexturemanager.h"

#include "arcballcamera.h"
#include "sgnode.h"
#include "sgheaders.h"

//#include "SGBulletRigidMesh.h"
//#include "SGBulletRigidDynamics.h"
//#include "SGBulletSoftRigidDynamics.h"


using namespace Loki;
using namespace std;
using namespace ps;
using namespace ps::opengl;
using namespace ps::base;

#define FRAME_TIME_BUFFER_SIZE 32


namespace ps {
namespace scene {

struct LightSource {
	vec4f pos;
	vec4f color;
};

/*!
 * Scene Graph
 */
class SGEngine : public IMouseListener {
public:
    SGEngine();
    virtual ~SGEngine();

    void init();
    void resize(int w, int h);

    //Draws the entire scenegraph for traversing through the list of
    //nodes and calling individual draw methods
    void draw();
    void drawBBoxes();
    void timestep();

    //Nodes
    U32 add(SGNode* aNode);
//    U32 addRigidBody(SGBulletRigidMesh* aRigidBody);
//    U32 addSoftBody(SGBulletSoftMesh* aSoftBody);
//    SGBulletSoftRigidDynamics* world() { return m_lpWorld;}

    U32 addSceneBox(const AABB& box);
    U32 addFloor(int rows, int cols, float step = 1.0f);
    bool remove(U32 index);
    bool remove(const string& name);
    bool remove(const SGNode* pnode);

    U32 count() const {return (U32)m_vSceneNodes.size();}
    SGNode* get(U32 index) const {return m_vSceneNodes[index];}
    SGNode* get(const char* name) const;
    SGNode* last() const;


    //Matrix Stacks
	CopyStack<mat44f>& stkProjection() {return m_stkProjection;}
	CopyStack<mat44f>& stkModelView() {return m_stkModelView;}
	mat44f modelviewprojection() const;

    //Camera
    ArcBallCamera& camera() { return m_camera;}
    
    //Mouse calls
    void mousePress(MouseButton button, MouseButtonState state, int x, int y) override;
    void mouseWheel(MouseWheelDir dir) override;
    void mouseMove(int x, int y) override;
    
    //Coordinate Conversion
    bool screenToWorld(const vec3f& s, vec3f& w);
    Ray  screenToWorldRay(int x, int y);
    vec4i viewport() const;    

    //Headers
    SGHeaders* headers() const {return m_headers;}
    void updateCameraHeader();

    //Modifier
    int getModifier() const {return m_keyModifier;}
    void setModifier(int mod) {m_keyModifier = mod;}

    void update();
    static AnsiStr gpuInfo();

    //print structure
    void print(const char* switches = "-a") const;

    //Save and Load view settings
	bool readConfig(const AnsiStr& strFP = "scene.ini");
	bool writeConfig(const AnsiStr& strFP = "scene.ini");



    //Timing and Profiling services

    //Object Selection

    //Surfaces

    
protected:
    void cleanup();

private:
    int m_keyModifier;

    //SGBulletSoftRigidDynamics* m_lpWorld;
	CopyStack<mat44f> m_stkProjection;
	CopyStack<mat44f> m_stkModelView;
	std::vector<SGNode*> m_vSceneNodes;


    ArcBallCamera m_camera;
    SGHeaders* m_headers;
    int m_idCamHeader;
    int m_idGPUHeader;
    int m_idAnimationHeader;

    //Stats
    MovingAvg<double, FRAME_TIME_BUFFER_SIZE> m_avgFrameTime;
    tbb::tick_count m_tick;
    double m_fps;
    U64 m_ctFrames;
    U64 m_ctSampledFrames;
};

//Singleton Access to scene graph
typedef SingletonHolder<SGEngine, CreateUsingNew, PhoenixSingleton> TheEngine;

}
}

#endif /* SCENEGRAPH_H_ */
