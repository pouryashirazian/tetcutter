#ifndef SG_MESH_H
#define SG_MESH_H

#include "GLMeshBuffer.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGMesh : public GLMeshBuffer {
public:
	SGMesh():GLMeshBuffer() {}

	explicit SGMesh(const Geometry& g);
	explicit SGMesh(const AnsiStr& strFilePath);

	virtual ~SGMesh() {
		GLMeshBuffer::cleanup();
	}
    
    void setup(const Geometry& g) {
        GLMeshBuffer::setup(g);
        this->setAABB(g.aabb());
    }

	virtual void draw();

    virtual void drawNoEffect();
};

}
}

#endif
