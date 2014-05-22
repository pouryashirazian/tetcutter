#ifndef SG_MESH_H
#define SG_MESH_H

#include "SceneGraph.h"
#include "GLMeshBuffer.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGMesh : public GLMeshBuffer {
public:
	SGMesh():GLMeshBuffer() {}
	SGMesh(const Geometry& g):GLMeshBuffer(g) {
        this->setAABB(g.aabb());
    }

	virtual ~SGMesh() {
		GLMeshBuffer::cleanup();
	}
    
    void setup(const Geometry& g) {
        GLMeshBuffer::setup(g);
        this->setAABB(g.aabb());
    }

	virtual void draw();

    virtual void drawNoEffect() {
		GLMeshBuffer::draw();
    }
};

}
}

#endif
