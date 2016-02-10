#ifndef SGEFFECT_H
#define SGEFFECT_H

//#include "loki/SmartPtr.h"
#include <memory>
#include "glbackend/glshadermanager.h"

using namespace Loki;
using namespace ps::opengl;

namespace ps {
    namespace scene {
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
