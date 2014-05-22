/*
 * AppScreen.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: pshiraz
 */
#include "AppScreen.h"
#include "selectgl.h"
#include <iostream>
#include <cstdio>

using namespace std;

void ForwarderDraw(AppScreen* scr) {
	scr->draw();
}

AppScreen::AppScreen(int argc, char* argv[]) {
	m_width = DEFAULT_WIDTH;
	m_height = DEFAULT_HEIGHT;
	m_strTitle = string(DEFAULT_APP_TITLE);

	this->init(argc, argv);
}


AppScreen::AppScreen(int argc, char* argv[], int w, int h, const char* title) {

	m_width = w;
	m_height = h;
	m_strTitle = string(title);

	this->init(argc, argv);
}

AppScreen::~AppScreen() {

}

void AppScreen::close() {

}

void AppScreen::init(int argc, char* argv[]) {

	//Init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(m_width, m_height);
	glutCreateWindow(m_strTitle.c_str());

	//glutDisplayFunc (&AppScreen::draw);
	//glutReshapeFunc (Resize);
	//	glutMouseFunc(MousePress);
	//	glutPassiveMotionFunc(MousePassiveMove);
	//	glutMotionFunc(MouseMove);
	//	glutMouseWheelFunc(MouseWheel);

	//glutKeyboardFunc (NormalKey);
	//glutSpecialFunc (SpecialKey);
	//glutCloseFunc (Close);
}

void AppScreen::draw() {

}

void AppScreen::resize(int w, int h) {

}


//Global Default Functions: Reusable in many projects

void def_resize(int w, int h) {
    glViewport(0, 0, w, h);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, (double)w/(double)h, ZNEAR, ZFAR);
	//glOrtho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void def_initGL() {
	//Setup Shading Environment
	static const GLfloat lightColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat lightPos[4] = { 0.0f, 9.0f, 0.0f, 1.0f };
    
	//Setup Light0 Position and Color
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
	//Turn on Light 0
	glEnable(GL_LIGHT0);
	//Enable Lighting
	glEnable(GL_LIGHTING);
    
	//Enable features we want to use from OpenGL
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
    
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
	glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
	//glClearColor(1.0, 1.0, 1.0, 1.0);
    
	//Compiling shaders
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		//Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
}
