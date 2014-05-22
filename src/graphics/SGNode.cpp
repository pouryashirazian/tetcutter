/*
 * SGNode.cpp
 *
 *  Created on: Mar 9, 2014
 *      Author: pourya
 */
#include "SGNode.h"
#include "GLFuncs.h"

namespace PS {
    namespace SG {
        //////////////////////////////////////////////
        SGNode::SGNode(): m_name("unset"), m_visible(true), m_animate(true), m_selected(false),
            m_spEffect(std::make_shared<SGEffect>()),
            m_spTransform(std::make_shared<SGTransform>())
        {
        }

        SGNode::SGNode(const string& name, bool visible):m_name(name), m_visible(visible), m_animate(true),
			m_selected(false),
			m_spEffect(std::make_shared<SGEffect>()),
			m_spTransform(std::make_shared<SGTransform>())
        {
        }

        SGNode::~SGNode() {
        }

        void SGNode::drawBBox() const
        {
            DrawAABB(m_aabb.lower(), m_aabb.upper(), vec3f(0,0,1), 1.0f);
        }

        int SGNode::intersect(const Ray& r) {
            return m_aabb.intersect(r, 0.0f, FLT_MAX);
        }

        void SGNode::resetTransform() {
            if(m_spTransform == NULL)
                m_spTransform = SmartPtrSGTransform(new SGTransform());
            else
                m_spTransform->reset();
        }

    }
}

