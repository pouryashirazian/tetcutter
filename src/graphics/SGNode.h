/*
 * SGNode.h
 *
 *  Created on: Mar 9, 2014
 *      Author: pourya
 */

#ifndef SGNODE_H_
#define SGNODE_H_

#include <string>
#include <memory>
#include "SGEffect.h"
#include "SGTransform.h"
#include "AABB.h"

using namespace PS;
using namespace std;

namespace PS {
namespace SG {


/*!
 * \brief The SceneNode class is an element in the scenegraph can have
 * effect and transformation associated with.
 */
class SGNode
{
public:
	SGNode();
	SGNode(const string& name, bool visible = true);
	virtual ~SGNode();

    //Pure virtual function for all kids to override
	virtual void draw() = 0;
	virtual void drawBBox() const;

    //Advances animation
    virtual void timestep() { }

    //Computes the bounding box of the model
	void setAABB(const AABB& box) { m_aabb = box;}
    virtual AABB aabb() const { return m_aabb;}

    //Selection
    virtual int intersect(const Ray& r);

    //Name for hashing and fetching of the nodes
    string name() const {return m_name;}
    void setName(const string& name) {m_name = name;}

    //Visibility
    bool isVisible() const {return m_visible;}
    void setVisible(bool visible) {m_visible = visible;}

    //Animation
    bool isAnimate() const {return m_animate;}
    void setAnimate(bool animate) {m_animate = animate;}

    //Selection
    bool isSelected() const {return m_selected;}
    void setSelected(bool selected) {m_selected = selected;}

    //Effect to be managed by asset and shadermanager collections
    SmartPtrSGEffect effect() const {return m_spEffect;}
    void setEffect(const SmartPtrSGEffect& spEffect) { m_spEffect = spEffect;}

    //Transformation
    SmartPtrSGTransform transform() const { return m_spTransform;}
    void setTransform(const SmartPtrSGTransform& spTransform) {
    	m_spTransform = spTransform;
    }
    void resetTransform();


    //TODO: IO to be able to read and write scene nodes to disk in very fast binary format
    bool read() {return false;}
    bool write() {return false;}
protected:
	//Each scene node should have its associated bounding box.
    //For selection, Hidden Surface Culling and Ray-tracing
	AABB m_aabb;

	//Attribs
    string m_name;
    bool m_visible;
    bool m_animate;
    bool m_selected;

    //bool m_bAnimated;
    std::shared_ptr<SGEffect> m_spEffect;
    std::shared_ptr<SGTransform> m_spTransform;
};

}
}

#endif /* SGNODE_H_ */
