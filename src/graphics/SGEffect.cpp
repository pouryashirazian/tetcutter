//
//  SGEffect.cpp
//  hifem
//
//  Created by pshiraz on 1/14/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#include "SGEffect.h"


namespace PS {
    namespace SG {
        
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
                m_lpShader->start();
        }

        void SGEffect::unbind() {
            if(m_lpShader)
                m_lpShader->stop();
        }
    }
}