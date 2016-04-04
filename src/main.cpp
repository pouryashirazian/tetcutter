/*!
 * \brief tetcutter - fast tetrahedral mesh cutting
 * \author Pourya Shirazian
 */
#include <iostream>
#include <functional>
#include <tbb/task_scheduler_init.h>

#include "base/directory.h"
#include "base/logger.h"
#include "base/cmdlineparser.h"
#include "base/inifile.h"

#include "scene/sgengine.h"
#include "scene/gizmo.h"
#include "scene/sgquad.h"

#include "glbackend/glfuncs.h"
#include "glbackend/glselect.h"
#include "glbackend/glscreen.h"

#include "elastic/avatarscalpel.h"
#include "elastic/avatarring.h"
#include "elastic/tetsubdivider.h"
#include "elastic/volmeshsamples.h"
#include "elastic/volmeshio.h"
#include "elastic/volmeshstats.h"

//#include "graphics/SGRenderMask.h"

#include <GLFW/glfw3.h>

using namespace tbb;
using namespace ps;
using namespace ps::utils;
using namespace ps::scene;
using namespace ps::opengl;
using namespace ps::dir;
using namespace ps::elastic;

using namespace std;


//global vars
GLFWwindow* g_lpWindow = NULL;
AvatarScalpel* g_lpScalpel = NULL;
AvatarRing* g_lpRing = NULL;
IAvatar* g_lpAvatar = NULL;

CuttableMesh* g_lpTissue = NULL;
CmdLineParser g_parser;
AnsiStr g_strIniFilePath;
U32 g_current = 3;
U32 g_cutCase = 0;

//funcs
void closeApp();
bool resetMesh();
void cutFinished();
void runTestSubDivide(int current);
void handleElementEvent(CELL element, U32 handle, VolMesh::TopologyEvent event);
void normal_key(unsigned char key, int x, int y);

inline ps::MouseButton glfw_mouse_button_to_ps(int button) {
    switch(button) {
    case(GLFW_MOUSE_BUTTON_LEFT):
        return ps::mbLeft;
    case(GLFW_MOUSE_BUTTON_RIGHT):
        return ps::mbRight;
    case(GLFW_MOUSE_BUTTON_MIDDLE):
        return ps::mbMiddle;
    default:
        return ps::mbLeft;
    }
}

inline ps::MouseButtonState glfw_mouse_button_state_to_ps(int action) {
    switch(action) {
    case(GLFW_PRESS):
        return ps::mbsPressed;
    case(GLFW_RELEASE):
        return ps::mbsReleased;
    default:
        return ps::mbsPressed;
    }
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos = 0.0;
    glfwGetCursorPos(window, &xpos, &ypos);

    int x = (int)xpos;
    int y = (int)ypos;
    ps::MouseButton btn = glfw_mouse_button_to_ps(button);
    ps::MouseButtonState state = glfw_mouse_button_state_to_ps(action);

    TheEngine::Instance().mousePress(btn, state, x, y);
    TheGizmoManager::Instance().mousePress(btn, state, x, y);

    //left key
    if(btn == ps::MouseButton::mbLeft  && state == mbsPressed) {
        //vec3f expand(0.2);
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        Ray ray = TheEngine::Instance().screenToWorldRay(x, y);
        int idxVertex = g_lpTissue->selectNode(ray);

        //select vertex
        if (idxVertex >= 0) {
            vloginfo("Selected Vertex Index = %d ", idxVertex);
            g_lpTissue->setNodeToShow(idxVertex);
        }
    }
}

static void mouse_motion_callback(GLFWwindow* window, double xpos, double ypos)
{
    //double xpos, ypos;
    //glfwGetCursorPos(window, &xpos, &ypos);
    //vloginfoArg2("xpos : %f, ypos: %f\n", xpos, ypos);
    TheEngine::Instance().mouseMove(xpos, ypos);
    TheGizmoManager::Instance().mouseMove(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    TheEngine::Instance().mouseWheel(yoffset > 0 ? MouseWheelDir::mwUp : MouseWheelDir::mwDown);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        closeApp();
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(action != GLFW_PRESS)
        return;

    switch(key)
    {
        case(GLFW_KEY_F1):
        {
            if(g_current > 0)
                g_current --;
            runTestSubDivide(g_current);
            break;
        }
        case(GLFW_KEY_F2):
        {
            if(g_current < 3)
                g_current ++;
            runTestSubDivide(g_current);
            break;
        }

        case(GLFW_KEY_F3): {
            g_cutCase = (g_cutCase + 1) % 2;
            g_current = 0;
            char cutcase[6] = {'A', 'B', 'C', 'D', 'E'};

            vloginfo("cut case = %c, current mesh node/face = %c", cutcase[g_cutCase], g_current);
            runTestSubDivide(g_current);
            break;
        }


        case(GLFW_KEY_F4):
        {
            //Set UIAxis
            int axis = (int)TheGizmoManager::Instance().axis();
            axis = (axis + 1) % axisCount;
            TheGizmoManager::Instance().setAxis((GizmoAxis)axis);
            vloginfo("Change haptic axis to %d", TheGizmoManager::Instance().axis());
            break;
        }

        case(GLFW_KEY_F5):
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


        case(GLFW_KEY_F6):
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

        case(GLFW_KEY_F7):
        {
            g_lpTissue->setFlagSplitMeshAfterCut(!g_lpTissue->getFlagSplitMeshAfterCut());
            vloginfo("Tissue splitting is set to: %d", g_lpTissue->getFlagSplitMeshAfterCut());
            break;
        }

        case(GLFW_KEY_F8): {
            bool flag = !TheEngine::Instance().get("floor")->isVisible();
            TheEngine::Instance().get("floor")->setVisible(flag);
            vloginfo("Set floor to %s", flag ? "show" : "hide");
            break;
        }

        case(GLFW_KEY_F9): {
            bool flag = !TheGizmoManager::Instance().isVisible();
            TheGizmoManager::Instance().setVisible(flag);
            vloginfo("Set gizmos to %s", flag ? "show" : "hide");

            break;
        }

        case(GLFW_KEY_F10): {
            resetMesh();
            vloginfo("reset mesh");

            break;
        }

        case(GLFW_KEY_F11): {
            g_lpRing->grip();
            vloginfo("Gripped apply on scalpel!");
            break;
        }


    default:
        //vloginfo("No special key handled this so I forward it to normal keys");
        double fx, fy;
        glfwGetCursorPos(window, &fx, &fy);

        normal_key(key, (int)fx, (int)fy);
    }

    glfwSwapBuffers(window);
}

void draw() {
    TheEngine::Instance().draw();
	TheGizmoManager::Instance().draw();
}

void timestep() {
    TheEngine::Instance().timestep();
	TheGizmoManager::Instance().timestep();
}

void normal_key(unsigned char key, int x, int y)
{
    key = tolower(key);

	switch(key)
	{

	case('/'): {
		g_lpTissue->setFlagDrawSweepSurf(!g_lpTissue->getFlagDrawSweepSurf());
        vloginfo("Draw sweep surf set to: %d", g_lpTissue->getFlagDrawSweepSurf());
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
            TheEngine::Instance().headers()->updateHeaderLine("cell", strInfo);
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
            TheEngine::Instance().headers()->updateHeaderLine("cell", strInfo);
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
        bool flag = !TheEngine::Instance().get("headers")->isVisible();
        TheEngine::Instance().get("headers")->setVisible(flag);
        vloginfo("Set headers draw to %s", flag ? "show" : "hide");
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

	case ('t'): {
		if (g_lpAvatar == g_lpScalpel) {
			g_lpAvatar = g_lpRing;
			g_lpScalpel->setVisible(false);
		} else {
			g_lpAvatar = g_lpScalpel;
			g_lpRing->setVisible(false);
		}

		g_lpAvatar->setVisible(true);
		g_lpAvatar->setTissue(g_lpTissue);
		g_lpAvatar->setOnCutFinishedEventHandler(cutFinished);
		TheGizmoManager::Instance().setFocusedNode(g_lpAvatar);
		break;
	}

	case('.'):{
		if(g_lpTissue)
			g_lpTissue->setFlagDrawWireFrame(!g_lpTissue->getFlagDrawWireFrame());
        vloginfo("Wireframe mode is %d", g_lpTissue->getFlagDrawWireFrame());
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
        bool flag = !TheEngine::Instance().get("rendermask")->isVisible();
        TheEngine::Instance().get("rendermask")->setVisible(flag);
        vloginfo("Set rendermask to %s", flag ? "show" : "hide");
		break;
	}
	break;

	case('p'): {
        TheEngine::Instance().print();
	}
	break;


	case('['):{
        TheEngine::Instance().camera().incrZoomLevel(0.5f);
	}
	break;
	case(']'):{
        TheEngine::Instance().camera().incrZoomLevel(-0.5f);
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

        vloginfo("Attempt to store at %s. Make sure all the required directories are present!", strVegOutput.cptr());
		if(VolMeshIO::writeVega(g_lpTissue, strVegOutput))
            vloginfo("Stored the mesh at: %s", strVegOutput.cptr());

        vloginfo("Attempt to store at %s. Make sure all the required directories are present!", strObjOutput.cptr());
		if(VolMeshIO::writeObj(g_lpTissue, strObjOutput))
            vloginfo("Stored the mesh at: %s", strObjOutput.cptr());
	}
	break;


	}


	//Update Screen
//	glutPostRedisplay();
}


void closeApp() {
    TheGizmoManager::Instance().writeConfig(g_strIniFilePath);
    TheEngine::Instance().writeConfig(g_strIniFilePath);

	SAFE_DELETE(g_lpScalpel);
	SAFE_DELETE(g_lpRing);
	SAFE_DELETE(g_lpTissue);
}

void handleElementEvent(CELL element, U32 handle, VolMesh::TopologyEvent event) {

	if(event == VolMesh::teAdded)
        vloginfo("A new element added at index: %d", handle);
	else if(event == VolMesh::teRemoved)
        vloginfo("A new element added at index: %d", handle);
}

bool resetMesh() {
	//remove it from scenegraph
    TheEngine::Instance().remove(g_lpTissue);
	SAFE_DELETE(g_lpTissue);

    if(!FileExists(g_strIniFilePath)) {
        vlogerror("ini file not exists! [%s]", g_strIniFilePath.c_str());
        return false;
    }

    vloginfo("reading model config from ini file: [%s]", g_strIniFilePath.c_str());
    IniFile ini(g_strIniFilePath, IniFile::fmRead);
    AnsiStr mdl_type = ini.readString("model", "type");
    AnsiStr mdl_name = ini.readString("model", "name");

    VolMesh* temp = NULL;

    //loading internal model
    if(mdl_type == "internal") {

        int pos = -1;
        if(mdl_name == "one")
            temp = VolMeshSamples::CreateOneTetra();
        else if(mdl_name == "two")
            temp = VolMeshSamples::CreateTwoTetra();
        else if(mdl_name.lfindstr(AnsiStr("cube"), pos)) {

            U32 nx, ny, nz = 0;
            sscanf(mdl_name.cptr(), "cube_%u_%u_%u", &nx, &ny, &nz);
            temp = VolMeshSamples::CreateTruthCube(nx, ny, nz, 0.2);
        }
        else if(mdl_name.lfindstr(AnsiStr("eggshell"), pos)) {

            U32 nx, ny;
            //float radius, thickness;
            sscanf(mdl_name.cptr(), "eggshell_%u_%u", &nx, &ny);
            temp = VolMeshSamples::CreateEggShell(nx, ny);
        }
        else
            temp = VolMeshSamples::CreateOneTetra();
    }
    else if(mdl_type == "file") {
        temp = new VolMesh();
        temp->setFlagFilterOutFlatCells(false);
        temp->setVerbose(g_parser.value_to_int("verbose"));

        if(!FileExists(mdl_name)) {
            AnsiStr data_root_path = ini.readString("system", "data");
            mdl_name = data_root_path + "meshes/veg/" + mdl_name;
            vloginfo("resolved model name to [%s]", mdl_name.cptr());
        }

        vloginfo("Begin to read vega file from: %s", mdl_name.cptr());
        bool res = VolMeshIO::readVega(temp, mdl_name);
        if(!res)
            vlogerror("Unable to load mesh from: %s", mdl_name.cptr());
    }


    vloginfo("Loaded mesh to temp");
	g_lpTissue = new CuttableMesh(*temp);
	g_lpTissue->setFlagSplitMeshAfterCut(true);
	g_lpTissue->setFlagDrawNodes(true);
	g_lpTissue->setFlagDrawWireFrame(false);
    g_lpTissue->setFlagDrawSweepSurf(ini.readBool("visible", "sweepsurf"));
	g_lpTissue->setColor(Color::skin());
    g_lpTissue->setVerbose(g_parser.value_to_int("verbose") != 0);
	g_lpTissue->syncRender();
	SAFE_DELETE(temp);

    TheEngine::Instance().add(g_lpTissue);
    if(g_parser.value_to_int("ringscalpel") == 1)
		g_lpRing->setTissue(g_lpTissue);
	else
		g_lpScalpel->setTissue(g_lpTissue);

	//print stats
	VolMeshStats::printAllStats(g_lpTissue);

    vloginfo("mesh load completed");
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

    if(!g_parser.value_to_int("disjoint"))
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
        TheEngine::Instance().add(vMeshes[i]);

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
	int ctThreads = tbb::task_scheduler_init::default_num_threads();
	tbb::task_scheduler_init init(ctThreads);
 	cout << "started tbb with " << ctThreads << " threads." << endl;

	//parser
    g_parser.addSwitch("--disjoint", "-d", "converts splitted part to disjoint meshes", "0");
    g_parser.addSwitch("--ringscalpel", "-r", "If the switch presents then the ring scalpel will be used");
    g_parser.addSwitch("--verbose", "-v", "prints detailed description.");
    g_parser.addSwitch("--input", "-i", "[filepath] set input file in vega format", "internal");
    //g_parser.addSwitch("--example", "-e", "[one, two, cube, eggshell] set an internal example", "two");
    //g_parser.addSwitch("--gizmo", "-g", "loads a file to set gizmo location and orientation", "gizmo.ini");

	if(g_parser.parse(argc, argv) < 0)
		exit(0);

	//file path
    g_strIniFilePath = AnsiStr(g_parser.value("input").c_str());
    if(FileExists(g_strIniFilePath))
        vloginfo("input file: %s.", g_strIniFilePath.cptr());
    else {
        vlogerror("Unable to find input filepath: [%s]", g_strIniFilePath.cptr());
        exit(1);
    }


    //GLFW LIB
    g_lpWindow = NULL;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        vlogerror("Failed to init glfw");
        exit(EXIT_FAILURE);
    }

    /*
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    */

    g_lpWindow = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "tetcutter - Pourya Shirazian", NULL, NULL);
    if (!g_lpWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(g_lpWindow);
    glfwSwapInterval(1);
    glfwSetKeyCallback(g_lpWindow, key_callback);
    glfwSetCursorPosCallback(g_lpWindow, mouse_motion_callback);
    glfwSetMouseButtonCallback(g_lpWindow, mouse_button_callback);
    glfwSetScrollCallback(g_lpWindow, scroll_callback);

	//init gl
	def_initgl();

	//Build Shaders for drawing the mesh
    IniFile ini(g_strIniFilePath, IniFile::fmRead);

    AnsiStr strRoot = ini.readString("system", "data");
    AnsiStr strShaderRoot = strRoot + "shaders/";
    AnsiStr strTextureRoot = strRoot + "textures/";
    AnsiStr strFontsRoot = strRoot + "fonts/";

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

    vloginfo("Floor show is set to: [%d]", ini.readBool("visible", "floor"));

	//Ground and Room
	SGQuad* floor = new SGQuad(16.0f, 16.0f, TheTexManager::Instance().get("icefloor"));
	floor->setName("floor");
	floor->transform()->translate(vec3f(0, -0.1f, 0));
	floor->transform()->rotate(vec3f(1.0f, 0.0f, 0.0f), 90.0f);
    floor->setVisible(ini.readBool("visible", "floor"));
    TheEngine::Instance().add(floor);

	//Create Scalpel
	g_lpScalpel = new AvatarScalpel();
	g_lpScalpel->setVisible(false);

	g_lpRing = new AvatarRing(TheTexManager::Instance().get("spin"));
	g_lpRing->setVisible(false);
    TheEngine::Instance().add(g_lpScalpel);
    TheEngine::Instance().add(g_lpRing);

    if(g_parser.value_to_int("ringscalpel") == 1) {
		g_lpAvatar = g_lpRing;
		g_lpRing->setVisible(true);
	}
	else {
		g_lpAvatar = g_lpScalpel;
		g_lpScalpel->setVisible(true);
	}

	g_lpAvatar->setTissue(g_lpTissue);
	g_lpAvatar->setOnCutFinishedEventHandler(cutFinished);
	TheGizmoManager::Instance().setFocusedNode(g_lpAvatar);


	//load gizmo manager file
    //TheGizmoManager::Instance().readConfig(g_strIniFilePath);
    TheEngine::Instance().readConfig(g_strIniFilePath);
    TheEngine::Instance().headers()->addHeaderLine("cell", "info");
    TheEngine::Instance().print();

	//reset cuttable mesh
    resetMesh();


    //mainloop
    while (!glfwWindowShouldClose(g_lpWindow))
    {
        //setup projection matrix
        int width = DEFAULT_WIDTH;
        int height = DEFAULT_HEIGHT;
        glfwGetFramebufferSize(g_lpWindow, &width, &height);
        def_resize(width, height);

        //draw frame
        draw();

        timestep();

        glfwSwapBuffers(g_lpWindow);
        glfwPollEvents();
    }

    //destroy window
    glfwDestroyWindow(g_lpWindow);
    glfwTerminate();

    exit(EXIT_SUCCESS);


	return 0;
}
