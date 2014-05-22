#ifndef SCENENODETRANSFORM_H
#define SCENENODETRANSFORM_H

#include <memory>
#include "base/Matrix.h"
#include "base/Vec.h"
#include "base/Quaternion.h"

using namespace PS::MATH;

namespace PS {
namespace SG {

/*!
 * class representing transformation nodes in the scene graph.
 * This will decrease memory foot print since a pointer will determine
 * a scene node connection with a transformation node
 */
class SGTransform {
public:
    SGTransform(bool bAutoUpdateBackward = false);
    SGTransform(const SGTransform* other);
    virtual ~SGTransform();

    //Transform
    void scale(const vec3f& s);
    void rotate(const quat& q);
    void rotate(const vec3f& axis, float deg);
    void translate(const vec3f& t);

    //Get Transform
    vec3f getScale() const { return m_mtxForward.getDiag().xyz();}
    vec3f getTranslate() const {
    	return m_mtxForward.getTranslate();
    }

    void resetTranslate() {
    	vec4f t = m_mtxForward.getCol(3);
    	translate(t.xyz() * -1.0f);
    }

    void reset();
    void updateBackward();

    //For opengl usage
    void bind();
    void unbind();

    const mat44f& forward() const {return m_mtxForward;}
    const mat44f& backward() const {return m_mtxBackward;}

protected:
    mat44f m_mtxForward;
    mat44f m_mtxBackward;
    bool m_bAutoUpdate;
};

typedef std::shared_ptr<SGTransform> SmartPtrSGTransform;

}
}

#endif // SCENENODETRANSFORM_H
