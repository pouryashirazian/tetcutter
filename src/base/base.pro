##includes common settings from common.pri file
! include( ../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

# Input
HEADERS += aabb.h \
           alignedarray.h \
           base.h \
           color.h \
           copystack.h \
           debugutils.h \
           directory.h \
           flatarray.h \
           imouselistener.h \
           inifile.h \
           interval.h \
           logger.h \
           matrix.h \
           movingaverage.h \
           profiler.h \
           quaternion.h \
           ray.h \
           resourcemanager.h \
           simdvec.h \
           str.h \
           strbase.h \
           vec.h

SOURCES += aabb.cpp \
           debugutils.cpp \
           directory.cpp \
           imouselistener.cpp \
           inifile.cpp \
           logger.cpp \
           matrix.cpp \
           profiler.cpp \
           resourcemanager.cpp \
           str.cpp
