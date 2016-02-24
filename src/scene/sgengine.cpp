#include <float.h>

#include "glbackend/glselect.h"
#include "glbackend/glfuncs.h"

#include "sgengine.h"
#include "sgfloor.h"
#include "sgbox.h"
#include "sgtransform.h"

#include "base/inifile.h"
#include "base/logger.h"
#include "base/inifile.h"
#include "base/directory.h"


using namespace ps;
using namespace ps::opengl;
using namespace ps::utils;
using namespace ps::dir;
using namespace ps::scene;

#define FOVY 45.0
#define ZNEAR 0.01
#define ZFAR 100.0


SGEngine::SGEngine() {
	m_stkModelView.top().identity();
	m_stkProjection.top().identity();

	//World
	//m_lpWorld = new SGBulletRigidDynamics();
    //m_lpWorld = new SGBulletSoftRigidDynamics();
    //m_vSceneNodes.push_back(m_lpWorld);

	//add header to opaque list
	m_headers = new SGHeaders();
	m_vSceneNodes.push_back(m_headers);

	m_tick = tbb::tick_count::now();
	m_ctFrames = m_ctSampledFrames = 0;


	//Headers to show
	m_idGPUHeader = m_headers->addHeaderLine("gpu", gpuInfo());
	m_idCamHeader = m_headers->addHeaderLine("camera", "");
	m_idAnimationHeader = m_headers->addHeaderLine("animation", "");

	//update all
	updateCameraHeader();
}

SGEngine::~SGEngine() {
	cleanup();
}

void SGEngine::cleanup() {
//	for (U32 i = 0; i < m_vSceneNodes.size(); i++)
	//	SAFE_DELETE(m_vSceneNodes[i]);
	m_vSceneNodes.resize(0);
//	SAFE_DELETE(m_lpWorld);
}

void SGEngine::init() {

    //init
    int argc = 1;
    AnsiStr strPath = GetExePath();
    char* argv = strPath.ptr();
    //glutInit(&argc, &argv);

    //init opengl
    InitGL();
}

void SGEngine::resize(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOVY, (double)w/(double)h, ZNEAR, ZFAR);
    //glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


U32 SGEngine::add(SGNode *aNode) {
	if(aNode == NULL)
		return -1;

	m_vSceneNodes.push_back(aNode);
	return (m_vSceneNodes.size() - 1);
}

/*
U32 SceneGraph::addRigidBody(SGBulletRigidMesh* aRigidBody) {
	m_lpWorld->addRigidBody(aRigidBody);
	return add(aRigidBody);
}

U32 SceneGraph::addSoftBody(SGBulletSoftMesh* aSoftBody) {
	m_lpWorld->addSoftBody(aSoftBody);
	return add(aSoftBody);
}
*/

bool SGEngine::remove(U32 index) {
	if(index >= m_vSceneNodes.size())
		return false;

	m_vSceneNodes.erase(m_vSceneNodes.begin() + index);
	return true;
}

bool SGEngine::remove(const string& name) {
	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if (m_vSceneNodes[i]->name() == name) {
			m_vSceneNodes.erase(m_vSceneNodes.begin() + i);
			return true;
		}
	}

	return false;
}

bool SGEngine::remove(const SGNode* pnode) {
	if(pnode == NULL)
		return false;

	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if (m_vSceneNodes[i] == pnode) {
			m_vSceneNodes.erase(m_vSceneNodes.begin() + i);
			return true;
		}
	}

	return false;
}

U32 SGEngine::addSceneBox(const AABB& box) {
	SGBox* lpBox = new SGBox(box.lower(), box.upper());
	lpBox->setName("scenebox");
	return this->add(lpBox);
}

U32 SGEngine::addFloor(int rows, int cols, float step) {
	SGFloor* pFloor = new SGFloor(rows, cols, step);
	pFloor->setName("floor");
	return this->add(pFloor);
}

SGNode* SGEngine::get(const char* name) const {
	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if (m_vSceneNodes[i]->name() == string(name)) {
			return m_vSceneNodes[i];
		}
	}
	return NULL;
}

SGNode* SGEngine::last() const {
	U32 ctNodes = (U32) m_vSceneNodes.size();
	if (ctNodes > 0)
		return m_vSceneNodes[ctNodes - 1];
	else
		return NULL;
}

void SGEngine::draw() {
	//Stats
	m_ctFrames++;
	tbb::tick_count tkStart = tbb::tick_count::now();
	m_avgFrameTime.addValue((tkStart - m_tick).seconds() * 1000.0);
	m_tick = tkStart;

	double avgFT = m_avgFrameTime.getAverage();
	m_fps = 1000.0 / ((avgFT > 1.0) ? avgFT : 1.0);


	//Frame Count, Frame Time and FPS, Log Count
	char chrMsg[1024];
	sprintf(chrMsg, "ANIMATION FRAME# %08llu, LOGS# %08llu, AVG TIME# %.2f, FPS# %d",
			m_ctFrames,
			m_ctSampledFrames,
			avgFT,
			(int)m_fps);
    TheEngine::Instance().headers()->updateHeaderLine("animation", AnsiStr(chrMsg));


	//Draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Camera
	m_camera.look();

	//Draw All Visible Nodes
	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if (m_vSceneNodes[i]->isVisible())
			m_vSceneNodes[i]->draw();
	}

	//Cull SwapBuffers of the API such as
	//glutSwapBuffers
}

void SGEngine::drawBBoxes() {
	//Draw All Visible Nodes
	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if (m_vSceneNodes[i]->isVisible()) {
			AABB box = m_vSceneNodes[i]->aabb();
			if (box.isValid())
				DrawAABB(box.lower(), box.upper(), vec3f(0, 0, 1), 1.0f);
		}
	}
}

void SGEngine::timestep() {
	//update
	for (U32 i = 0; i < m_vSceneNodes.size(); i++) {
		if(m_vSceneNodes[i]->isAnimate())
			m_vSceneNodes[i]->timestep();
	}
}

mat44f SGEngine::modelviewprojection() const {
	mat44f mtxMVP = m_stkProjection.top() * m_stkModelView.top();
	return mtxMVP;
}

void SGEngine::mousePress(MouseButton button, MouseButtonState state, int x, int y) {
	m_camera.mousePress(button, state, x, y);
	updateCameraHeader();
}

void SGEngine::mouseMove(int x, int y) {
    m_camera.mouseMove(x, y);
    updateCameraHeader();
}

void SGEngine::mouseWheel(MouseWheelDir dir) {
    m_camera.mouseWheel(dir);
    updateCameraHeader();
}

void SGEngine::updateCameraHeader() {
	if (!m_headers)
		return;
	if (!m_headers->isValidID(m_idCamHeader))
		return;

	char chrMsg[1024];
    sprintf(chrMsg, "Camera [Roll=%.1f, Tilt=%.1f, Zoom=%.2f, PanX=%.2f, PanY=%.2f]",
            m_camera.roll(), m_camera.tilt(), m_camera.zoomlevel(),
            m_camera.pan().x, m_camera.pan().y);
	AnsiStr strInfo = AnsiStr(chrMsg);
	m_headers->updateHeaderLine(m_idCamHeader, strInfo);
}

bool SGEngine::screenToWorld(const vec3f &s, vec3f &w) {
	GLdouble ox, oy, oz;
	GLdouble mv[16];
	GLdouble pr[16];
	GLint vp[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, mv);
	glGetDoublev(GL_PROJECTION_MATRIX, pr);
	glGetIntegerv(GL_VIEWPORT, vp);
	if (gluUnProject(s.x, s.y, s.z, mv, pr, vp, &ox, &oy, &oz) == GL_TRUE) {
		w = vec3f(ox, oy, oz);
		return true;
	}

	return false;
}

Ray SGEngine::screenToWorldRay(int x, int y) {

	vec4i vp = viewport();
	int screenWidth = vp[2];
	int screenHeight = vp[3];

	//Select Gizmo First
	vec3f ns(x, screenHeight - y, 0.0f);
	vec3f fs(x, screenHeight - y, 1.0f);
	vec3f nw;
	vec3f fw;
	Ray r;

	if (screenToWorld(ns, nw) && screenToWorld(fs, fw)) {
		vec3f dir = fw - nw;
		r.set(nw, dir.normalized());
	} else {
        vlogerror("Unable to convert screen to world!");
	}

	return r;
}

vec4i SGEngine::viewport() const {
	GLint vp[4];
	glGetIntegerv( GL_VIEWPORT, vp);
	return vec4i(&vp[0]);
}

void SGEngine::update() {
	//glutPostRedisplay();
}

AnsiStr SGEngine::gpuInfo() {
	AnsiStr strVendorName = AnsiStr((const char*)glGetString(GL_VENDOR));
	AnsiStr strRenderer   = AnsiStr((const char*)glGetString(GL_RENDERER));
	AnsiStr strVersion	 = AnsiStr((const char*)glGetString(GL_VERSION));
	AnsiStr strExtensions = AnsiStr((const char*)glGetString(GL_EXTENSIONS));

	int pos;
	if(strVendorName.lfindstr("Intel", pos)) {
			cout << "WARNING: Integrated GPU is being used!" << endl;
            vlogwarn("Non-Discrete GPU selected for rendering!");
	}

	return  AnsiStr("GPU: ") + strVendorName + ", " + strRenderer + ", " + strVersion;
}

void SGEngine::print(const char* switches) const {
	U32 ctVisible = 0;
	for(U32 i=0; i<m_vSceneNodes.size(); i++) {
		if(m_vSceneNodes[i]->isVisible()) ctVisible++;
	}

	//Stats
	printf("scenegraph tree. Node# %d, Visible# %d\n", (int)m_vSceneNodes.size(), ctVisible);
	printf("Name \t\t Visible \t\t Pos\n");
	printf("----------------------------------------------------------------------------\n");
	for(U32 i=0; i<m_vSceneNodes.size(); i++) {
		SGNode* node = m_vSceneNodes[i];
		vec3f pos = node->transform()->getTranslate();
		printf("%s \t\t %d \t\t [%.2f %.2f %.2f]\n", node->name().c_str(), node->isVisible(), pos.x, pos.y, pos.z);
	}

}

bool SGEngine::readConfig(const AnsiStr& strFP) {
	if(!FileExists(strFP)) {
        vlogerror("File %s not found to read scene config.", strFP.cptr());
		return false;
	}

    IniFile* script = new IniFile(strFP, IniFile::fmRead);
	m_camera.setRoll(script->readFloat("camera", "roll"));
	m_camera.setTilt(script->readFloat("camera", "tilt"));
    m_camera.setZoomLevel(script->readFloat("camera", "zoom"));
	m_camera.setPan(script->readVec2f("camera", "pan"));

	SAFE_DELETE(script);

	return true;
}

bool SGEngine::writeConfig(const AnsiStr& strFP) {

    IniFile* script = new IniFile(strFP, IniFile::fmReadWrite);

    script->writeFloat("camera", "roll", m_camera.roll());
    script->writeFloat("camera", "tilt", m_camera.tilt());
    script->writeFloat("camera", "zoom", m_camera.zoomlevel());
    script->writeVec2f("camera", "pan", m_camera.pan());
	SAFE_DELETE(script);

	return true;
}
