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
TetSubdivider* g_lpSubdivider = NULL;
U32 g_current = 3;
U32 g_cutMode = 0;

//funcs
void subdivide(int current);
void handleElementEvent(HalfEdgeTetMesh::ELEM element, U32 handle, HalfEdgeTetMesh::TopologyEvent event);

void draw() {
	TheSceneGraph::Instance().draw();
	TheGizmoManager::Instance().draw();

	glutSwapBuffers();

}

void timestep() {
	TheSceneGraph::Instance().timestep();
}

void MousePress(int button, int state, int x, int y)
{
    TheGizmoManager::Instance().mousePress(button, state, x, y);
    TheSceneGraph::Instance().mousePress(button, state, x, y);

    //Update selection
	glutPostRedisplay();
}


void MousePassiveMove(int x, int y)
{
}

void MouseMove(int x, int y)
{
	TheGizmoManager::Instance().mouseMove(x, y);
	TheSceneGraph::Instance().mouseMove(x, y);

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

void subdivide(int current) {

	//remove it from scenegraph
	TheSceneGraph::Instance().remove("tets");
	SAFE_DELETE(g_lpSubdivider);
	SAFE_DELETE(g_lpTetMesh);

	//create
//	g_lpTetMesh = HalfEdgeTetMesh::CreateOneTet();
//	g_lpTetMesh->setOnElemEventCallback(handleElementEvent);
//	g_lpTetMesh->setName("tets");
//	TheSceneGraph::Instance().add(g_lpTetMesh);
//
//	g_lpSubdivider = new TetSubdivider(g_lpTetMesh);

	//subdivide tet
	double tEdges[6];
	U8 cutEdgeCode, cutNodeCode = 0;

	if(g_cutMode == 0)
		g_lpSubdivider->generateCaseA(0, current, 0.4, cutEdgeCode, cutNodeCode, tEdges);
	else if(g_cutMode == 1)
		g_lpSubdivider->generateCaseB(0, current, cutEdgeCode, cutNodeCode, tEdges);
	g_lpSubdivider->subdivide(0, cutEdgeCode, cutNodeCode, tEdges);
}

void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case(GLUT_KEY_F1):
		{
			if(g_current > 0)
				g_current --;
			subdivide(g_current);
			break;
		}
		case(GLUT_KEY_F2):
		{
			if(g_current < 3)
				g_current ++;
			subdivide(g_current);
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

		case(GLUT_KEY_F7): {
			g_cutMode = (g_cutMode + 1) % 2;
			g_current = 0;

			LogInfoArg2("cut mode = %d, current selection = %d", g_cutMode, g_current);
			subdivide(g_current);
			break;
		}
	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {


	SAFE_DELETE(g_lpScalpel);
	SAFE_DELETE(g_lpSubdivider);
	SAFE_DELETE(g_lpTetMesh);
}

void handleElementEvent(HalfEdgeTetMesh::ELEM element, U32 handle, HalfEdgeTetMesh::TopologyEvent event) {

	if(event == HalfEdgeTetMesh::teAdded)
		LogInfoArg1("A new element added at index: %d", handle);
	else if(event == HalfEdgeTetMesh::teRemoved)
		LogInfoArg1("A new element added at index: %d", handle);
}

int main(int argc, char* argv[]) {
	cout << "Cutting tets" << endl; // prints !!!Hello World!!!

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

	//create a scalpel
	g_lpTetMesh = CuttableMesh::CreateOneTetra();
	g_lpTetMesh->setName("tets");
	TheSceneGraph::Instance().add(g_lpTetMesh);



	//TheGizmoManager::Instance().transform()->translate(vec3f(0,3,0));

	//g_lpSubdivider = new TetSubdivider(g_lpTetMesh);
	g_lpScalpel = new AvatarScalpel(g_lpTetMesh);
	TheGizmoManager::Instance().setNode(g_lpScalpel);
	TheSceneGraph::Instance().add(g_lpScalpel);

	//call subdivide
	//subdivide(g_current);

	glutMainLoop();

	return 0;
}
