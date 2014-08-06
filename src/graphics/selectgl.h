/*
 * selectgl.h
 *
 *  Created on: Dec 14, 2013
 *      Author: pourya
 */

#ifndef SELECTGL_H_
#define SELECTGL_H_

#include "base/MathBase.h"

#ifdef PS_OS_LINUX
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include "GL/glu.h"

#elif PS_OS_MAC

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <OpenGL/gl.h>
#include "OpenGL/glu.h"

#endif

#endif /* SELECTGL_H_ */
