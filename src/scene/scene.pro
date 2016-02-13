##includes common settings from common.pri file
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

INCLUDEPATH += .

# Input
HEADERS += arcballcamera.h \
           geometry.h \
           gizmo.h \
           sgassetmanager.h \
           sgbox.h \
           sgeffect.h \
           sgengine.h \
           sgfloor.h \
           sgheaders.h \
           sgmesh.h \
           sgnode.h \
           sgtransform.h \
           surfacemesh.h

SOURCES += arcballcamera.cpp \
           geometry.cpp \
           gizmo.cpp \
           sgassetmanager.cpp \
           sgbox.cpp \
           sgeffect.cpp \
           sgengine.cpp \
           sgfloor.cpp \
           sgheaders.cpp \
           sgmesh.cpp \
           sgnode.cpp \
           sgtransform.cpp \
           surfacemesh.cpp

LIBS += -L../../bin -lbase -lglbackend
LIBS += -lGLEW -lglut

