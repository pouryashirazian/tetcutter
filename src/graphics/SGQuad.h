//
//  SGQuad.h
//  hifem
//
//  Created by pshiraz on 1/21/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_SGQuad_h
#define hifem_SGQuad_h

#include "SGMesh.h"
#include "graphics/GLTexture.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
    namespace SG {
        
        class SGQuad : public SGMesh {
            
        public:
        	SGQuad();
            SGQuad(float w, float h, GLTexture* aTex = NULL);
            virtual ~SGQuad();
            
            virtual void draw();
            
            //Set Texture
            void setTexture(GLTexture* lpTex) {m_lpTex = lpTex;}
        protected:
            void setup(float w, float h);
            
        protected:
            GLTexture* m_lpTex;
        };
        
    }
}



#endif
