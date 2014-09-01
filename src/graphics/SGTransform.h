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
    SGTransform(const SGTransform& other);
    virtual ~SGTransform();

    void copyFrom(const SGTransform& other);

    //Transform
    void scale(const vec3f& delta);
    void scale(float sfactor);
    void rotate(const quat& q);
    void rotate(const vec3f& axis, float deg);
    void translate(const vec3f& delta);

    //Get Transform
    vec3f getScale() const { return m_scale;}
    vec3f getTranslate() const { return m_translate; }
    quatf getRotate() const { return m_rotate;}

    //Set Transform
    void setScale(const vec3f& s);
    void setRotate(const quat& r);
    void setTranslate(const vec3f& t);


    void reset();
    void resetScale();
    void resetRotate();
    void resetTranslate();
    void syncMatrices();

    //For opengl usage
    void bind();
    void unbind();

    const mat44f& forward() const {return m_mtxForward;}
    const mat44f& backward() const {return m_mtxBackward;}

protected:
    vec3f m_translate;
    vec3f m_scale;
    quatf m_rotate;

    mat44f m_mtxForward;
    mat44f m_mtxBackward;
    bool m_autoUpdate;
    int m_changes;
};

typedef std::shared_ptr<SGTransform> SmartPtrSGTransform;

}
}

#endif // SCENENODETRANSFORM_H
