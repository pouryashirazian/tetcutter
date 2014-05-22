//
//  SGEffect.h
//  hifem
//
//  Created by pshiraz on 1/14/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_SGEffect_h
#define hifem_SGEffect_h

//#include "loki/SmartPtr.h"
#include <memory>
#include "ShaderManager.h"

using namespace Loki;
using namespace PS::GL;

namespace PS {
    namespace SG {
        class SGEffect {
        public:
            enum EffectType {etMaterial, etTexture, etCustomShader};
            SGEffect();
            SGEffect(GLShader* s);
            virtual ~SGEffect();
            
            
            
            //Bind Effect
            virtual void bind();
            
            //Unbind Effect
            virtual void unbind();
            
            //Shader Access
            GLShader* shader() const {return m_lpShader;}
            void setShader(GLShader* s) {m_lpShader = s;}
        protected:
            GLShader* m_lpShader;
            EffectType m_etype;
        };

        //SmartPtrSceneNodeEffect
        /*
        typedef Loki::SmartPtr<
        SGEffect,
        RefCounted,
        AllowConversion,
        AssertCheck,
        DefaultSPStorage > SmartPtrSGEffect;
         */
        typedef std::shared_ptr<SGEffect> SmartPtrSGEffect;

    }
}



#endif
