#include "sgeffect.h"

namespace ps {
    namespace scene {
        
        SGEffect::SGEffect() {
            m_etype = etCustomShader;
            m_lpShader = NULL;
        }
        
        SGEffect::SGEffect(GLShader* s) {
            m_etype = etCustomShader;
            m_lpShader = s;
        }
        
        SGEffect::~SGEffect() {
            m_lpShader = NULL;
        }
        
        void SGEffect::bind() {
            if(m_lpShader)
                m_lpShader->bind();
        }

        void SGEffect::unbind() {
            if(m_lpShader)
                m_lpShader->unbind();
        }
    }
}
