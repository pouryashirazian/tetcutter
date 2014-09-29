//
//  SGTexture.cpp
//  hifem
//
//  Created by pshiraz on 1/21/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#include "GLTexture.h"
#include "base/FileDirectory.h"
#include "base/Logger.h"
#include "lodepng/lodepng.h"
#include "selectgl.h"
#include "GLTypes.h"

using namespace PS::FILESTRINGUTILS;

namespace PS {
    namespace GL {
        GLTexture::GLTexture() {
            m_texunit = 0;
            m_glTex = INVALID_GLBUFFER;
        }
        
        GLTexture::GLTexture(const AnsiStr& strFP, int texunit) {
            m_texunit = texunit;
            m_glTex = INVALID_GLBUFFER;
            if(!read(strFP)) {
                LogErrorArg1("Texture creation failed for file: %s", strFP.cptr());
            }
        }
        
        GLTexture::~GLTexture() {
            cleanup();
        }
        
        //
        void GLTexture::cleanup() {
            if(glIsTexture(m_glTex))
                glDeleteTextures(1, &m_glTex);
        }
        
        GLTexture* GLTexture::CheckerBoard()
        {
            GLTexture* lpOutput = new GLTexture();
            
            const int TEX_SIZE = 64;
            GLubyte image[TEX_SIZE][TEX_SIZE][3];
            int i,j,c;
            
            for(i = 0; i < TEX_SIZE; i++)
            {
                for(j = 0; j < TEX_SIZE; j++)
                {
                    c = (((i & 0x8) == 0)^((j & 0x8) == 0)) * 255;
                    image[i][j][0] = (GLubyte)c;
                    image[i][j][1] = (GLubyte)c;
                    image[i][j][2] = (GLubyte)c;
                }
            }
            
            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

            //Params
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            vec3i dim = vec3i(TEX_SIZE, TEX_SIZE, 3);
            lpOutput->set(dim, id, 0);
            return lpOutput;
        }

        
        bool GLTexture::read(const AnsiStr& strFP) {
            if(!FileExists(strFP))
                return false;
            ImageFileType ft = GetFileType(strFP);
            if(ft != iftPNG)
                return false;
            
            //Cleanup before reading the image file
            this->cleanup();
            
            //Generate Texture
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glActiveTexture(GL_TEXTURE0 + m_texunit);
            glGenTextures(1, &m_glTex);
            glBindTexture(GL_TEXTURE_2D, m_glTex);
            
            //Load image data
            switch (ft) {
                case iftPNG:
                    read_png(strFP);
                    break;
                default:
                    break;
            }

            //Params
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glPopAttrib();
            return true;
        }
        
        bool GLTexture::read_png(const AnsiStr& strFP) {
            
            vector<U8> data;
            U32 w, h;
            U32 error = lodepng::decode(data, w, h, strFP.cptr());
            if(error) {
                LogErrorArg1("Unable to load png image from file: %s", strFP.cptr());
                return false;
            }
            
            m_dim = vec3i(w, h, 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_dim.x, m_dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
            return true;
        }
        
        bool GLTexture::write(const AnsiStr& strFP) {
            return false;
        }
        
        void GLTexture::set(const vec3i& dim, U32 handle, int texunit) {
            this->cleanup();
            m_dim = dim;
            m_glTex = handle;
            m_texunit = texunit;
        }
        
        void GLTexture::bind() {
            if(m_glTex != INVALID_GLBUFFER) {
                glActiveTexture(GL_TEXTURE0 + m_texunit);
                glBindTexture(GL_TEXTURE_2D, m_glTex);
            }
        }
        
        void GLTexture::unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        GLTexture::ImageFileType GLTexture::GetFileType(const AnsiStr &strFP) {
            AnsiStr strExt = PS::FILESTRINGUTILS::ExtractFileExt(strFP);
            strExt.toUpper();
            
            if(strExt == "BMP")
                return iftBMP;
            else if(strExt == "PNG")
                return iftPNG;
            else if(strExt == "JPG")
                return iftJPG;
            else
                return iftUnsupported;
        }
    }
}
