#pragma once
#ifndef CGLSHADER_H
#define CGLSHADER_H

#include "base/String.h"
#include "base/Vec.h"
#include <vector>

using namespace PS::MATH;
#define GL_SUCCESS		   1
#define ERR_SHADER_FILE_NOT_FOUND -1
#define ERR_SHADER_COMPILE_ERROR  -2
#define ERR_SHADER_LINK_ERROR	  -3

namespace PS {
    namespace GL {
        
        //An OpenGL shader program
        class GLShader
        {
        public:
            GLShader();
            GLShader(const char* pszVertexShader, const char* pszFragmentShader, const char* pszGeometryShader = NULL);
            virtual ~GLShader();
            
            enum ShaderType {stVertex = 0, stFragment = 1, stGeometry = 2};
            enum VARUSAGE {vuAttribute, vuUniform};
            enum VARPRECISION {vpLow, vpMedium, vpHigh, vpUndefined};
            struct VARPROP
            {
                AnsiStr strName;
                AnsiStr strType;
                VARUSAGE usage;
                VARPRECISION precision;
                int idxLocation;
            };
            
            void init();
            
            //After analysis is done we fetch the locations of uniforms
            int		 getUniformLocation(const char* chrUniformName);
            void setUniform(int idLoc, const vec4f& v);
            void setUniform(int idLoc, const vec3f& v);
            void setUniform(int idLoc, const vec2f& v);
            void setUniform(int idLoc, float f);
            void setUniform1i(int idLoc, int i);
            
            
            
            //After analysis is done user can get the location where attribs set Automatically
            int 	 getAttribLocation(const char* chrAttribName);
            bool 	 setAttribLocation(const char* chrAttribName, int idxLoc);
            
            U32 	program() const { return m_glShader;}
            
            //Status
            bool isCompiled() const {return m_isCompiled;}
            bool isRunning() const {return m_isRunning;}
            bool isReadyToRun();
            
            
            /*
             * Opens the text file containing the shader code and reads the content for both
             * Vertex Shader and Fragment Shader programs.
             * @param strVertexShaderFP File path to the vertex shader code
             * @param strFragmentShaderFP File path to the fragment shader program
             * @return 1 if shader compiles successfully
             */
            int	 compileFromFile(const AnsiStr& strVertexShaderFP,
                                 const AnsiStr& strFragmentShaderFP);
            
            int	 compileFromFile(const AnsiStr& strVertexShaderFP,
                                 const AnsiStr& strFragmentShaderFP,
                                 const AnsiStr& strGeometryShaderFP);
            
            
            /*
             * Compiles shader code and performs an analysis on the number variables defined.
             * Reports compile and link errors. This function attaches all attribute variables to
             * their locations based on their position in the list (i.e. First Found first position)
             * All Found attributes are added to attributes list and all uniforms are being put in the
             * uniforms list.
             * @param vShaderCode string containing Vertex Shader code.
             * @param vFragmentCode string containing Fragment Shader code.
             * @return 1 if successfully compile and link the shader program
             */
            int  compileCode(const char* vShaderCode, const char* vFragmentCode, const char* vGeometryCode = NULL);
            
            //Load and Save Binary Shaders
            static bool isBinaryShaderSupported();
            static bool isGLExtensionSupported(const char *extension);
            
            
            bool loadBinaryProgram(const char* Filename, U32 &ProgramObjectID);
            bool saveBinaryProgram(const char* Filename, U32 &ProgramObjectID);
            
            //Program run
            void start();
            
            //Program stop
            void stop();
            
        private:
            void reportShaderCompileErrors(U32 uShaderName, const char* pshadertype);
            bool removeAllCppComments(AnsiStr& strCode);
            bool readShaderCode(const AnsiStr& strFilePath, AnsiStr& strCode);
            
        private:
            U32   m_glShader;
            bool  m_isCompiled;
            bool  m_isRunning;
        };
        
    }
}
#endif


