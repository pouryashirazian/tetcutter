/*
 * AppScreen.h
 *
 *  Created on: Dec 12, 2013
 *      Author: pshiraz
 */

#ifndef APPSCREEN_H_
#define APPSCREEN_H_

#include <string>

using namespace std;
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_APP_TITLE "HIFEM ENGINE"
#define FOVY 45.0
#define ZNEAR 0.01
#define ZFAR 100.0

/*!
 * synopsis: Screen for the main 3D App
 */
class AppScreen {
public:
	AppScreen(int argc, char* argv[]);
	AppScreen(int argc, char* argv[], int w, int h, const char* title);
	virtual ~AppScreen();

	virtual void draw();
	virtual void resize(int w, int h);
	virtual void specialKey(int key, int x, int y) = 0;
	virtual void normalKey(unsigned char key, int x, int y) = 0;

	virtual void close();

protected:

	void init(int argc, char* argv[]);

private:
	int m_width;
	int m_height;
	string m_strTitle;
};

//Default handlers for reuse
void def_resize(int w, int h);
void def_initgl();


#endif /* APPSCREEN_H_ */
