//
//  selectcl.h
//  hifem
//
//  Created by pshiraz on 1/28/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_selectcl_h
#define hifem_selectcl_h

#include "base/MathBase.h"

#ifdef PS_OS_WINDOWS
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_ext.h>
#elif defined(PS_OS_LINUX)
#include <CL/opencl.h>
#include <GL/glxew.h>
#elif defined(PS_OS_MAC)
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <GL/glxew.h>
#endif

#endif
