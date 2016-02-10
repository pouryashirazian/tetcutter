#ifndef SG_MESH_H
#define SG_MESH_H

#include "sgnode.h"
#include "geometry.h"
#include "glbackend/glmeshbuffer.h"


using namespace ps;
using namespace ps::opengl;

namespace ps {
namespace scene {

class SGMesh : public SGNode {
public:
    SGMesh():SGNode() {}

	explicit SGMesh(const Geometry& g);
	explicit SGMesh(const AnsiStr& strFilePath);
    virtual ~SGMesh();
    
    void setup(const Geometry& g);

    //draw
	virtual void draw();
    virtual void drawNoEffect();

    //glmesh
    GLMesh& glmesh() { return m_meshbuffer;}
    const GLMesh& const_glmesh() const { return m_meshbuffer;}

protected:
    GLMesh m_meshbuffer;
};

}
}

#endif
