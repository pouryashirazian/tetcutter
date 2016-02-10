#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "base/vec.h"
#include "base/str.h"
#include "glbindable.h"

using namespace ps;
using namespace ps::base;

namespace ps {
    namespace opengl {
     
        class GLTexture : public GLBindable {
        public:
            GLTexture();
            GLTexture(const AnsiStr& strFP, int texunit = 0);
            virtual ~GLTexture();
            
            //Enums
            enum ImageFileType {iftPNG, iftBMP, iftJPG, iftUnsupported};
            
            //IO
            void cleanup();
            bool read(const AnsiStr& strFP);
            bool write(const AnsiStr& strFP);
            void set(const vec3i& dim, U32 handle, int texunit = 0);
            
            //Binding
            void bind();
            void unbind();
            
            //Dimension
            vec3i dim() const {return m_dim;}

            //Set
            U32 handle() const {return m_glTex;}

            
            //Statics
            static ImageFileType GetFileType(const AnsiStr& strFP);
            static GLTexture* CheckerBoard();
        private:
            bool read_png(const AnsiStr& strFP);
            
        protected:
            int m_texunit;
            U32 m_glTex;
            vec3i m_dim;
        };
    }
}


#endif
