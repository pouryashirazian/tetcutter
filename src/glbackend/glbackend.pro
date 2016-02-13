##includes common settings from common.pri file
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

INCLUDEPATH += ..

# Input
HEADERS += glbindable.h \
           glbuffer.h \
           glfuncs.h \
           glmeshbuffer.h \
           glselect.h \
           glshader.h \
           glshadermanager.h \
           gltexture.h \
           gltexturemanager.h \
           gltypes.h

SOURCES += glbindable.cpp \
           glbuffer.cpp \
           glfuncs.cpp \
           glmeshbuffer.cpp \
           glshader.cpp \
           glshadermanager.cpp \
           gltexture.cpp \
           gltexturemanager.cpp \
           ../3rdparty/lodepng/lodepng.cpp \

LIBS += -L../../bin -lbase

LIBS += -lGLEW -lglut
