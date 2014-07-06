//============================================================================
// Name        : tetcutter.cpp
// Author      : Pourya Shirazian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <functional>
#include "base/FileDirectory.h"
#include "base/Logger.h"
#include "graphics/SceneGraph.h"
#include "graphics/Gizmo.h"
#include "graphics/AppScreen.h"
#include "graphics/selectgl.h"
#include "deformable/AvatarScalpel.h"
#include "deformable/TetSubdivider.h"

using namespace PS;
using namespace PS::SG;
using namespace PS::FEM;
using namespace PS::FILESTRINGUTILS;

using namespace std;

AvatarScalpel* g_lpScalpel = NULL;
CuttableMesh* g_lpTetMesh = NULL;

U32 g_current = 3;
U32 g_cutCase = 0;

//funcs
void resetMesh();
void runTestSubDivide(int current);
void handleElementEvent(HalfEdgeTetMesh::ELEM element, U32 handle, HalfEdgeTetMesh::TopologyEvent event);

void draw() {
	TheSceneGraph::Instance().draw();
	TheGizmoManager::Instance().draw();

	glutSwapBuffers();

}

void timestep() {
	TheSceneGraph::Instance().timestep();
	TheGizmoManager::Instance().timestep();
}

void MousePress(int button, int state, int x, int y)
{
    TheSceneGraph::Instance().mousePress(button, state, x, y);
    TheGizmoManager::Instance().mousePress(button, state, x, y);

	glutPostRedisplay();
}


void MousePassiveMove(int x, int y)
{
}

void MouseMove(int x, int y)
{
	TheSceneGraph::Instance().mouseMove(x, y);
	TheGizmoManager::Instance().mouseMove(x, y);

	glutPostRedisplay();
}

void MouseWheel(int button, int dir, int x, int y)
{
	TheSceneGraph::Instance().mouseWheel(button, dir, x, y);
	glutPostRedisplay();
}

void NormalKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case('a'): {
		if(!g_lpTetMesh) return;

//		U32 i = g_lpTetMesh->getElemToShow();
//		if(g_lpTetMesh->isElemIndex(i))
//			g_lpTetMesh->setElemToShow(--i);
//		else
//			g_lpTetMesh->setElemToShow(0);
	}
	break;

	case('d'): {
		if(!g_lpTetMesh) return;

//		U32 i = g_lpTetMesh->getElemToShow();
//		if(g_lpTetMesh->isElemIndex(i))
//			g_lpTetMesh->setElemToShow(++i);
//		else
//			g_lpTetMesh->setElemToShow(0);
	}
	break;

	case('g'):{
		TheGizmoManager::Instance().setType(gtTranslate);
	}
	break;

	case('h'):{
		LogInfo("Home scalpel");
		//TheGizmoManager::Instance().
	}
	break;

	case('s'):{
		TheGizmoManager::Instance().setType(gtScale);
		break;
	}

	case('r'):{
		TheGizmoManager::Instance().setType(gtRotate);
		break;
	}

	case('x'):{
		TheGizmoManager::Instance().setAxis(axisX);
	}
	break;
	case('y'):{
		TheGizmoManager::Instance().setAxis(axisY);
	}
	break;
	case('z'):{
		TheGizmoManager::Instance().setAxis(axisZ);
	}
	break;

	case('p'): {
		TheSceneGraph::Instance().print();
	}
	break;

	case('['):{
		TheSceneGraph::Instance().camera().incrZoom(0.5f);
	}
	break;
	case(']'):{
		TheSceneGraph::Instance().camera().incrZoom(-0.5f);
	}
	break;

	case(27):
	{
		//Saving Settings and Exit
		LogInfo("Saving settings and exit.");
		glutLeaveMainLoop();
	}
	break;


	}


	//Update Screen
	glutPostRedisplay();
}


void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case(GLUT_KEY_F1):
		{
			if(g_current > 0)
				g_current --;
			runTestSubDivide(g_current);
			break;
		}
		case(GLUT_KEY_F2):
		{
			if(g_current < 3)
				g_current ++;
			runTestSubDivide(g_current);
			break;
		}

		case(GLUT_KEY_F3): {
			g_cutCase = (g_cutCase + 1) % 2;
			g_current = 0;
			char cutcase[6] = {'A', 'B', 'C', 'D', 'E'};

			LogInfoArg2("cut case = %c, current mesh node/face = %c", cutcase[g_cutCase], g_current);
			runTestSubDivide(g_current);
			break;
		}


		case(GLUT_KEY_F4):
		{
			//Set UIAxis
			int axis = (int)TheGizmoManager::Instance().axis();
			axis = (axis + 1) % axisCount;
			TheGizmoManager::Instance().setAxis((GizmoAxis)axis);
			LogInfoArg1("Change haptic axis to %d", TheGizmoManager::Instance().axis());
			break;
		}

		case(GLUT_KEY_F5):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, -0.1);
			else
				inc = vec3f(-0.1, -0.1, -0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}


		case(GLUT_KEY_F6):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, 0.1);
			else
				inc = vec3f(0.1, 0.1, 0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}

		case(GLUT_KEY_F8): {
			bool flag = !TheSceneGraph::Instance().get("floor")->isVisible();
			TheSceneGraph::Instance().get("floor")->setVisible(flag);
			LogInfoArg1("Set floor to %s", flag ? "show" : "hide");
			break;
		}

		case(GLUT_KEY_F9): {
			bool flag = !TheGizmoManager::Instance().isVisible();
			TheGizmoManager::Instance().setVisible(flag);
			LogInfoArg1("Set gizmos to %s", flag ? "show" : "hide");

			break;
		}

		case(GLUT_KEY_F10): {
			resetMesh();
			LogInfo("reset mesh");

			break;
		}

	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {
	SAFE_DELETE(g_lpScalpel);
	SAFE_DELETE(g_lpTetMesh);
}

void handleElementEvent(HalfEdgeTetMesh::ELEM element, U32 handle, HalfEdgeTetMesh::TopologyEvent event) {

	if(event == HalfEdgeTetMesh::teAdded)
		LogInfoArg1("A new element added at index: %d", handle);
	else if(event == HalfEdgeTetMesh::teRemoved)
		LogInfoArg1("A new element added at index: %d", handle);
}

void resetMesh() {
	//remove it from scenegraph
	TheSceneGraph::Instance().remove("tets");
	SAFE_DELETE(g_lpTetMesh);

	//create a scalpel
	//g_lpTetMesh = CuttableMesh::CreateTruthCube(8, 4, 4, 0.5);
	g_lpTetMesh = CuttableMesh::CreateTwoTetra();
	g_lpTetMesh->setName("tets");
	TheSceneGraph::Instance().add(g_lpTetMesh);

	g_lpScalpel->setTissue(g_lpTetMesh);
}

void runTestSubDivide(int current) {
	resetMesh();

	//subdivide tet
	double tEdges[6];
	U8 cutEdgeCode, cutNodeCode = 0;

	if(g_cutCase == 0)
		g_lpTetMesh->getSubD()->generateCaseA(0, current, 0.4, cutEdgeCode, cutNodeCode, tEdges);
	else if(g_cutCase == 1)
		g_lpTetMesh->getSubD()->generateCaseB(0, current, cutEdgeCode, cutNodeCode, tEdges);

	//g_lpTetMesh->getSubD()->subdivide(0, cutEdgeCode, cutNodeCode, tEdges);
}

int main(int argc, char* argv[]) {
	cout << "Cutting tets" << endl;

	//Initialize app
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glutCreateWindow("OpenGL Framework");
	glutDisplayFunc(draw);
	glutReshapeFunc(def_resize);
	glutMouseFunc(MousePress);
	glutPassiveMotionFunc(MousePassiveMove);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(NormalKey);
	glutSpecialFunc(SpecialKey);
	glutCloseFunc(closeApp);
	glutIdleFunc(timestep);

	//init gl
	def_initgl();

	//Build Shaders for drawing the mesh
	AnsiStr strRoot = ExtractOneLevelUp(ExtractFilePath(GetExePath()));
	AnsiStr strShaderRoot = strRoot + "data/shaders/";

	//Load Shaders
	TheShaderManager::Instance().addFromFolder(strShaderRoot.cptr());

	//Ground and Room
	TheSceneGraph::Instance().addFloor(32, 32, 0.5f);
	TheSceneGraph::Instance().addSceneBox(AABB(vec3f(-10, -10, -16), vec3f(10, 10, 16)));

	//Create Scalpel
	g_lpScalpel = new AvatarScalpel();
	TheSceneGraph::Instance().add(g_lpScalpel);

	//Focus gizmo manager on the scalpel
	TheGizmoManager::Instance().setFocusedNode(g_lpScalpel);
	TheGizmoManager::Instance().transform()->translate(vec3f(0, 3, 0));

	//reset cuttable mesh
	resetMesh();

	glutMainLoop();

	return 0;
}
