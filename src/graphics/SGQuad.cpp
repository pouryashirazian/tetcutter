
//
//  SGQuad.cpp
//  hifem
//
//  Created by pshiraz on 1/21/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#include "SGQuad.h"
#include "selectgl.h"

namespace PS {
    namespace SG {
        
        //Effect
        class TexturedEffect : public SGEffect {
        public:
            TexturedEffect(GLShader* s):SGEffect(s) {
                m_idSampler = s->getUniformLocation("sampler2d");
            }
            
            void bind() {
                SGEffect::bind();
                m_lpShader->setUniform1i(m_idSampler, 0);
                
            }
        private:
            int m_idSampler;
        };
        
        
        //Quad
        SGQuad::SGQuad(float w, float h, GLTexture* aTex) {
            m_lpTex = aTex;
            
            float hw = 0.5f * w;
            float hh = 0.5f * h;
            vec3f n = vec3f(0,0,1);
            
            Geometry g;
            g.init();
            g.addVertex(vec3f(-hw, -hh, 0.0f));
            g.addVertex(vec3f(hw, -hh, 0.0f));
            g.addVertex(vec3f(hw, hh, 0.0f));
            g.addVertex(vec3f(-hw, hh, 0.0f));
            g.addNormal(n);
            g.addNormal(n);
            g.addNormal(n);
            g.addNormal(n);
            
            
            g.addTexCoord(vec2f(0, 0));
            g.addTexCoord(vec2f(1, 0));
            g.addTexCoord(vec2f(1, 1));
            g.addTexCoord(vec2f(0, 1));
            g.addTriangle(vec3u32(0, 1, 2));
            g.addTriangle(vec3u32(2, 3, 0));
            this->setup(g);
            
            if(TheShaderManager::Instance().has("textured")) {
                m_spEffect = SmartPtrSGEffect(new TexturedEffect(TheShaderManager::Instance().get("textured")));
            }
        }
        
        SGQuad::~SGQuad() {
            GLMeshBuffer::cleanup();
        }
        
        void SGQuad::draw() {
            glDisable(GL_CULL_FACE);
            if(m_lpTex)
                m_lpTex->bind();

            SGMesh::draw();

            if(m_lpTex)
                m_lpTex->unbind();

            glEnable(GL_CULL_FACE);
        }
        
    }
}
