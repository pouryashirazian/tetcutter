/*!
 * \brief tetcutter - fast tetrahedral mesh cutting
 * \author Pourya Shirazian
 */
#include <iostream>
#include <functional>
#include "base/FileDirectory.h"
#include "base/Logger.h"
#include "base/CmdLineParser.h"

#include "graphics/SceneGraph.h"
#include "graphics/Gizmo.h"
#include "graphics/AppScreen.h"
#include "graphics/selectgl.h"
#include "graphics/SGQuad.h"
#include "graphics/SGRenderMask.h"
#include "deformable/AvatarScalpel.h"
#include "deformable/AvatarRing.h"
#include "deformable/TetSubdivider.h"
#include "deformable/VolMeshSamples.h"
#include "deformable/VolMeshIO.h"
#include "deformable/VolMeshStats.h"

using namespace PS;
using namespace PS::SG;
using namespace PS::MESH;
using namespace PS::FILESTRINGUTILS;

using namespace std;

AvatarScalpel* g_lpScalpel = NULL;
AvatarRing* g_lpRing = NULL;
IAvatar* g_lpAvatar = NULL;

CuttableMesh* g_lpTissue = NULL;
CmdLineParser g_parser;
AnsiStr g_strFilePath;
U32 g_current = 3;
U32 g_cutCase = 0;

//funcs
void resetMesh();
void runTestSubDivide(int current);
void handleElementEvent(CELL element, U32 handle, VolMesh::TopologyEvent event);

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

    //left key
    if(button == GLUT_LEFT_BUTTON && state == 0) {
		vec3f expand(0.2);
		Ray ray = TheSceneGraph::Instance().screenToWorldRay(x, y);
		int idxVertex = g_lpTissue->selectNode(ray);

		//select vertex
		if (idxVertex >= 0) {
			LogInfoArg1("Selected Vertex Index = %d ", idxVertex);
			g_lpTissue->setNodeToShow(idxVertex);
		}
    }


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

void SaveAsObj() {

}

void NormalKey(unsigned char key, int x, int y)
{
	switch(key)
	{

	case('/'): {
		g_lpTissue->setFlagDrawSweepSurf(!g_lpTissue->getFlagDrawSweepSurf());
		LogInfoArg1("Draw sweep surf set to: %d", g_lpTissue->getFlagDrawSweepSurf());
	}
	break;
	case('a'): {
		if(!g_lpTissue) return;

		U32 i = g_lpTissue->getElemToShow();
		if(g_lpTissue->isCellIndex(i))
			g_lpTissue->setElemToShow(--i);
		else
			g_lpTissue->setElemToShow(0);

		if(g_lpTissue->isCellIndex(i)) {
			const CELL& cell = g_lpTissue->const_cellAt(i);
			AnsiStr strInfo = printToAStr("cell [%u], nodes [%u, %u, %u, %u]",
										  i, cell.nodes[0], cell.nodes[1], cell.nodes[2], cell.nodes[3]);
			TheSceneGraph::Instance().headers()->updateHeaderLine("cell", strInfo);
		}
	}
	break;

	case('d'): {
		if(!g_lpTissue) return;

		U32 i = g_lpTissue->getElemToShow();
		if(g_lpTissue->isCellIndex(i))
			g_lpTissue->setElemToShow(++i);
		else
			g_lpTissue->setElemToShow(0);

		if(g_lpTissue->isCellIndex(i)) {
			const CELL& cell = g_lpTissue->const_cellAt(i);
			AnsiStr strInfo = printToAStr("cell [%u], nodes [%u, %u, %u, %u]",
										  i, cell.nodes[0], cell.nodes[1], cell.nodes[2], cell.nodes[3]);
			TheSceneGraph::Instance().headers()->updateHeaderLine("cell", strInfo);
		}
	}
	break;

	case('e'): {
		if(!g_lpTissue) return;

		U32 ctElems = g_lpTissue->countCells();
		U32 idxElem = -1;
		printf("Insert element index to show: [0:%u]\n", ctElems-1);
		scanf("%u", &idxElem);
		if(g_lpTissue->isCellIndex(idxElem))
			g_lpTissue->setElemToShow(idxElem);
		else
			g_lpTissue->setElemToShow();
	}
	break;

	case('g'):{
		TheGizmoManager::Instance().setType(gtTranslate);
	}
	break;

	case('h'):{
		bool flag = !TheSceneGraph::Instance().get("headers")->isVisible();
		TheSceneGraph::Instance().get("headers")->setVisible(flag);
		LogInfoArg1("Set headers draw to %s", flag ? "show" : "hide");
		break;
	}

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

	case('m'): {
		bool flag = !TheSceneGraph::Instance().get("rendermask")->isVisible();
		TheSceneGraph::Instance().get("rendermask")->setVisible(flag);
		LogInfoArg1("Set rendermask to %s", flag ? "show" : "hide");
		break;
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
	case('w'):{
		AnsiStr strRoot = ExtractOneLevelUp(ExtractFilePath(GetExePath()));
		AnsiStr strOutput = strRoot + "data/output/";
		AnsiStr strVegOutput = strOutput + printToAStr("%s_cuts%d.veg",
									  g_lpTissue->name().c_str(),
									  g_lpTissue->countCompletedCuts());

		AnsiStr strObjOutput = strOutput + printToAStr("%s_cuts%d.obj",
									  g_lpTissue->name().c_str(),
									  g_lpTissue->countCompletedCuts());

		LogInfoArg1("Attempt to store at %s. Make sure all the required directories are present!", strVegOutput.cptr());
		if(VolMeshIO::writeVega(g_lpTissue, strVegOutput))
			LogInfoArg1("Stored the mesh at: %s", strVegOutput.cptr());

		LogInfoArg1("Attempt to store at %s. Make sure all the required directories are present!", strObjOutput.cptr());
		if(VolMeshIO::writeObj(g_lpTissue, strObjOutput))
			LogInfoArg1("Stored the mesh at: %s", strObjOutput.cptr());
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

		case(GLUT_KEY_F7):
		{
			g_lpTissue->setFlagSplitMeshAfterCut(!g_lpTissue->getFlagSplitMeshAfterCut());
			LogInfoArg1("Tissue splitting is set to: %d", g_lpTissue->getFlagSplitMeshAfterCut());
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

		case(GLUT_KEY_F11): {
			g_lpRing->grip();
			LogInfo("Gripped apply on scalpel!");
			break;
		}

		case(GLUT_KEY_F12): {
			LogInfo("Apply transform to mesh and then reset transform");
			g_lpTissue->applyTransformToMeshThenResetTransform();

			if(FileExists(g_strFilePath)) {
				bool res = VolMeshIO::writeVega(g_lpTissue, g_strFilePath);
				if(res) {
					LogInfoArg1("Modified mesh is stored to: %s", g_strFilePath.cptr());
				}
			}
			break;
		}
	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {
	TheGizmoManager::Instance().writeConfig();

	SAFE_DELETE(g_lpScalpel);
	SAFE_DELETE(g_lpRing);
	SAFE_DELETE(g_lpTissue);
}

void handleElementEvent(CELL element, U32 handle, VolMesh::TopologyEvent event) {

	if(event == VolMesh::teAdded)
		LogInfoArg1("A new element added at index: %d", handle);
	else if(event == VolMesh::teRemoved)
		LogInfoArg1("A new element added at index: %d", handle);
}

void resetMesh() {
	//remove it from scenegraph
	TheSceneGraph::Instance().remove(g_lpTissue);
	SAFE_DELETE(g_lpTissue);

	VolMesh* temp = NULL;
	if(FileExists(g_strFilePath)) {
		temp = new PS::MESH::VolMesh();
		temp->setVerbose(g_parser.value<int>("verbose"));
		LogInfoArg1("Begin to read vega file from: %s", g_strFilePath.cptr());
		bool res = PS::MESH::VolMeshIO::readVega(temp, g_strFilePath);
		if(!res)
			LogErrorArg1("Unable to load mesh from: %s", g_strFilePath.cptr());

//		U32 ctRemoved = temp->removeZeroVolumeCells();
//		if(ctRemoved > 0)
//			LogInfoArg1("Managed to remove %u flat cells in the mesh", ctRemoved);
	}
	else {
		AnsiStr strExample = g_parser.value<AnsiStr>("example");
		int pos = -1;
		if(strExample == "one")
			temp = PS::MESH::VolMeshSamples::CreateOneTetra();
		else if(strExample == "two")
			temp = PS::MESH::VolMeshSamples::CreateTwoTetra();
		else if(strExample.lfindstr(AnsiStr("cube"), pos)) {

			U32 nx, ny, nz = 0;
			sscanf(strExample.cptr(), "cube_%u_%u_%u", &nx, &ny, &nz);
			temp = PS::MESH::VolMeshSamples::CreateTruthCube(nx, ny, nz, 0.2);
		}
		else if(strExample.lfindstr(AnsiStr("eggshell"), pos)) {

			U32 nx, ny;
			//float radius, thickness;
			sscanf(strExample.cptr(), "eggshell_%u_%u", &nx, &ny);
			temp = PS::MESH::VolMeshSamples::CreateEggShell(nx, ny);
		}
		else
			temp = PS::MESH::VolMeshSamples::CreateOneTetra();
	}


	LogInfo("Loaded mesh to temp");
	g_lpTissue = new CuttableMesh(*temp);
	g_lpTissue->setFlagSplitMeshAfterCut(true);
	g_lpTissue->setFlagDrawNodes(false);
	g_lpTissue->setFlagDrawWireFrame(false);
	g_lpTissue->setVerbose(g_parser.value<int>("verbose") != 0);
	SAFE_DELETE(temp);

	TheSceneGraph::Instance().add(g_lpTissue);
	if(g_parser.value<int>("ringscalpel") == 1)
		g_lpRing->setTissue(g_lpTissue);
	else
		g_lpScalpel->setTissue(g_lpTissue);

	//print stats
	VolMeshStats::printAllStats(g_lpTissue);
	LogInfo("Loaded mesh completed");

	//rotate mesh
	//	vec3d translate(-2.03281307, -3.78926992, -1.11631393);
	//	vec3d scale(0.018766);
	//	VolMeshIO::fitmesh(g_lpTissue, scale, translate);
	//	TheGizmoManager::Instance().setFocusedNode(g_lpTissue);
	//	TheGizmoManager::Instance().cmdRotate(vec3f(1,0,0), -90.0f);
}

void runTestSubDivide(int current) {
	resetMesh();

	if(g_lpTissue->countCells() > 1)
		return;

	//subdivide tet
	U8 cutEdgeCode, cutNodeCode = 0;

	if(g_cutCase == 0)
		g_lpTissue->getSubD()->generateCaseA(g_lpTissue, 0, current, 0.4, cutEdgeCode, cutNodeCode);
	else if(g_cutCase == 1)
		g_lpTissue->getSubD()->generateCaseB(g_lpTissue, 0, current, cutEdgeCode, cutNodeCode);
	g_lpTissue->garbage_collection();
}

void cutFinished() {
	if(!g_parser.value<int>("disjoint"))
		return;


	vector<CuttableMesh*> vMeshes;
	g_lpTissue->convertDisjointPartsToMeshes(vMeshes);

	if(vMeshes.size() == 0)
		return;

	U32 ctMaxCells = 0;
	U32 idxMaxCell = 0;
	for(U32 i=0; i < vMeshes.size(); i++) {
		vMeshes[i]->computeAABB();
		vMeshes[i]->setElemToShow(0);
		TheSceneGraph::Instance().add(vMeshes[i]);

		if(vMeshes[i]->countCells() > ctMaxCells) {
			ctMaxCells = vMeshes[i]->countCells();
			idxMaxCell = i;
		}
	}

	g_lpTissue = vMeshes[idxMaxCell];

	g_lpTissue->setElemToShow();
	if(g_lpAvatar)
		g_lpAvatar->setTissue(g_lpTissue);
}

int main(int argc, char* argv[]) {
 	cout << "startup" << endl;

	//parser
 	g_parser.add_toggle("disjoint", "converts splitted part to disjoint meshes");
 	g_parser.add_toggle("ringscalpel", "If the switch presents then the ring scalpel will be used");
 	g_parser.add_toggle("verbose", "prints detailed description.");
 	g_parser.add_option("input", "[filepath] set input file in vega format", Value(AnsiStr("internal")));
	g_parser.add_option("example", "[one, two, cube, eggshell] set an internal example", Value(AnsiStr("two")));
	g_parser.add_option("gizmo", "loads a file to set gizmo location and orientation", Value(AnsiStr("gizmo.ini")));

	if(g_parser.parse(argc, argv) < 0)
		exit(0);

	//file path
	g_strFilePath = ExtractFilePath(GetExePath()) + g_parser.value<AnsiStr>("input");
	if(FileExists(g_strFilePath))
		LogInfoArg1("input file: %s.", g_strFilePath.cptr());
	else
		g_strFilePath = "";

	//Initialize appidxEdges
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
	AnsiStr strLeftPial = strRoot + "data/meshes/obj/brain/pial_Full_obj/lh.pial.obj";
	AnsiStr strRightPial = strRoot + "data/meshes/obj/brain/pial_Full_obj/rh.pial.obj";
	AnsiStr strShaderRoot = strRoot + "data/shaders/";
	AnsiStr strTextureRoot = strRoot + "data/textures/";

	//convert mesh
// 	AnsiStr strNodesFP = strRoot + AnsiStr("data/meshes/matlab/nodes.txt");
// 	AnsiStr strFacesFP = strRoot + AnsiStr("data/meshes/matlab/faces.txt");
// 	AnsiStr strCellsFP = strRoot + AnsiStr("data/meshes/matlab/cells.txt");
// 	VolMeshIO::convertMatlabTextToVega(strNodesFP, strFacesFP, strCellsFP);


	//Load Shaders
	TheShaderManager::Instance().addFromFolder(strShaderRoot.cptr());

	//Load Textures
	TheTexManager::Instance().add(strTextureRoot + "wood.png");
	TheTexManager::Instance().add(strTextureRoot + "spin.png");
	TheTexManager::Instance().add(strTextureRoot + "icefloor.png");
//	TheTexManager::Instance().add(strTextureRoot + "rendermask.png");
//	TheTexManager::Instance().add(strTextureRoot + "maskalpha.png");
//	TheTexManager::Instance().add(strTextureRoot + "maskalphafilled.png");

	//Ground and Room
	SGQuad* floor = new SGQuad(16.0f, 16.0f, TheTexManager::Instance().get("icefloor"));
	floor->setName("floor");
	floor->transform()->translate(vec3f(0, -0.1f, 0));
	floor->transform()->rotate(vec3f(1.0f, 0.0f, 0.0f), 90.0f);
	TheSceneGraph::Instance().add(floor);

	AnsiStr strGreyMatter = strRoot + "data/meshes/veg/brain/graymatter.veg";
	AnsiStr strWhiteMatter = strRoot + "data/meshes/veg/brain/whitematter.veg";
	{
		VolMesh* temp = new VolMesh();
		bool res = VolMeshIO::readVega(temp, strGreyMatter);
		if(res) {
			CuttableMesh* pmesh = new CuttableMesh(*temp);
			pmesh->setColor(Color::grey());
			pmesh->setName("graymatter");
			TheSceneGraph::Instance().add(pmesh);
		}
		SAFE_DELETE(temp);
	}
	{
		VolMesh* temp = new VolMesh();
		bool res = VolMeshIO::readVega(temp, strWhiteMatter);
		if(res) {
			CuttableMesh* pmesh = new CuttableMesh(*temp);
			pmesh->setColor(Color::white());
			pmesh->setName("whitematter");
			TheSceneGraph::Instance().add(pmesh);
		}
		SAFE_DELETE(temp);
	}


	//TheSceneGraph::Instance().addFloor(32, 32, 0.5f);
	TheSceneGraph::Instance().addSceneBox(AABB(vec3f(-10, -10, -16), vec3f(10, 10, 16)));

	//Create Scalpel
	g_lpScalpel = new AvatarScalpel();
	g_lpRing = new AvatarRing(TheTexManager::Instance().get("spin"));
	//g_lpRing = new AvatarRing(NULL);
	g_lpScalpel->setOnCutFinishedEventHandler(cutFinished);
	g_lpRing->setOnCutFinishedEventHandler(cutFinished);

	if(g_parser.value<int>("ringscalpel")) {
		//g_lpRing->setWireFrameMode(true);
		g_lpAvatar = g_lpRing;
		TheSceneGraph::Instance().add(g_lpRing);
		TheGizmoManager::Instance().setFocusedNode(g_lpRing);
	}
	else {
		g_lpAvatar = g_lpScalpel;
		TheSceneGraph::Instance().add(g_lpScalpel);
		TheGizmoManager::Instance().setFocusedNode(g_lpScalpel);
	}


	//load gizmo manager file
	AnsiStr strGizmoFP = g_parser.value<AnsiStr>("gizmo");
	TheGizmoManager::Instance().readConfig(strGizmoFP);

	//reset cuttable mesh
	resetMesh();

//	SGRenderMask* renderMask = new SGRenderMask(TheTexManager::Instance().get("maskalpha"));
//	renderMask->setName("rendermask");
//	TheSceneGraph::Instance().add(renderMask);


	TheSceneGraph::Instance().headers()->addHeaderLine("cell", "info");
	TheSceneGraph::Instance().print();


	glutMainLoop();

	return 0;
}
