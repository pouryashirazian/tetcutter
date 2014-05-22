#include <iostream>
#include <fstream>
#include "GLShader.h"
//#include "selectgl.h"
#include "GL/glew.h"
#include "base/FileDirectory.h"
#include "base/Logger.h"

using namespace PS::FILESTRINGUTILS;

namespace PS {
    namespace GL {
        
        GLShader::GLShader()
        {
            init();
        }
        
        GLShader::GLShader(const char* pszVertexShader, const char* pszFragmentShader, const char* pszGeometryShader)
        {
            init();
            compileCode(pszVertexShader, pszFragmentShader);
        }
        
        GLShader::~GLShader()
        {
            if(glIsProgram(m_glShader))
                glDeleteProgram(m_glShader);
            m_isCompiled = false;
        }
        
        
        void GLShader::init()
        {
            m_isCompiled = false;
            m_isRunning = false;
            m_glShader = 0;
            
        }
        
        int GLShader::getUniformLocation(const char *chrUniformName) {
            return glGetUniformLocation(m_glShader, chrUniformName);
        }
        
        void GLShader::setUniform(int idLoc, const vec4f& v) {
            glUniform4f(idLoc, v.x, v.y, v.z, v.w);
        }

        void GLShader::setUniform(int idLoc, const vec3f& v) {
            glUniform3f(idLoc, v.x, v.y, v.z);
        }

        void GLShader::setUniform(int idLoc, const vec2f& v) {
            glUniform2f(idLoc, v.x, v.y);
        }

        void GLShader::setUniform(int idLoc, float f) {
            glUniform1f(idLoc, f);
        }

        void GLShader::setUniform1i(int idLoc, int i) {
            glUniform1i(idLoc, i);
        }

        
        
        //Reads the shader code from file and compiles
        int GLShader::compileFromFile(const AnsiStr& strVertexShaderFP, const AnsiStr& strFragmentShaderFP)
        {
            if((strVertexShaderFP.length() == 0)||(strFragmentShaderFP.length() == 0))
                return ERR_SHADER_FILE_NOT_FOUND;
            
            AnsiStr strVShaderCode;
            AnsiStr strFShaderCode;
            
            //Read both files
            bool bres = readShaderCode(strVertexShaderFP, strVShaderCode);
            if(!bres) return ERR_SHADER_FILE_NOT_FOUND;
            bres = readShaderCode(strFragmentShaderFP, strFShaderCode);
            if(!bres) return ERR_SHADER_FILE_NOT_FOUND;
            
            return compileCode(strVShaderCode.cptr(), strFShaderCode.cptr());
        }
        
        int	 GLShader::compileFromFile(const AnsiStr& strVertexShaderFP,
                                       const AnsiStr& strFragmentShaderFP,
                                       const AnsiStr& strGeometryShaderFP) {
            if((strVertexShaderFP.length() == 0)||(strFragmentShaderFP.length() == 0)||
                (strGeometryShaderFP.length() == 0))
                return ERR_SHADER_FILE_NOT_FOUND;
            
            AnsiStr strVShaderCode;
            AnsiStr strFShaderCode;
            AnsiStr strGShaderCode;
            
            //Read both files
            bool bres = readShaderCode(strVertexShaderFP, strVShaderCode);
            if(!bres) return ERR_SHADER_FILE_NOT_FOUND;
            
            bres = readShaderCode(strFragmentShaderFP, strFShaderCode);
            if(!bres) return ERR_SHADER_FILE_NOT_FOUND;
            
            bool isValidGShader = false;
            bres = readShaderCode(strGeometryShaderFP, strGShaderCode);
            if(bres && strGShaderCode.length() > 0)
                isValidGShader = true;
            
            return compileCode(strVShaderCode.cptr(), strFShaderCode.cptr(), isValidGShader ? strGShaderCode.cptr() : NULL);
        }
        
        void GLShader::reportShaderCompileErrors(U32 uShaderName, const char* pshadertype) {
            
            int i32InfoLogLength, i32CharsWritten;
            glGetShaderiv(uShaderName, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
            
            char* pszInfoLog = new char[i32InfoLogLength];
            glGetShaderInfoLog(uShaderName, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
            char* pszMsg = new char[i32InfoLogLength+256];
            
            sprintf(pszMsg, "Failed to compile %s shader: ", pshadertype);
            strcat(pszMsg, pszInfoLog);
            LogError(pszMsg);
            
            SAFE_DELETE_ARRAY(pszMsg);
            SAFE_DELETE_ARRAY(pszInfoLog);
        }
        
        //////////////////////////////////////////////////////////////////////////
        int GLShader::compileCode(const char* vShaderCode, const char* vFragmentCode, const char* vGeometryCode)
        {
            GLint bShaderCompiled;
            
            //Vertex Shader
            U32 uiVertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(uiVertexShader, 1, (const char**)&vShaderCode, NULL);
            glCompileShader(uiVertexShader);
            
            //Check if compilation succeeded
            glGetShaderiv(uiVertexShader, GL_COMPILE_STATUS, &bShaderCompiled);
            if (!bShaderCompiled) {
                reportShaderCompileErrors(uiVertexShader, "vertex");
                return false;
            }
            
            //Fragment Shader
            U32 uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(uiFragShader, 1, (const char**)&vFragmentCode, NULL);
            glCompileShader(uiFragShader);
            
            //Check if compilation succeeded
            glGetShaderiv(uiFragShader, GL_COMPILE_STATUS, &bShaderCompiled);
            if (!bShaderCompiled) {
                reportShaderCompileErrors(uiFragShader, "fragment");
            }
            
            //Geometry Shader
            U32 uiGeometryShader;
            if(vGeometryCode) {
            	/*
                uiGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(uiGeometryShader, 1, (const char**)&vGeometryCode, NULL);
                glCompileShader(uiGeometryShader);
                
                //Check if compilation succeeded
                glGetShaderiv(uiGeometryShader, GL_COMPILE_STATUS, &bShaderCompiled);
                if (!bShaderCompiled) {
                    reportShaderCompileErrors(uiGeometryShader, "geometry");
                }
                */
            }
            
            // Create the shader program
            m_glShader = glCreateProgram();
            
            //Attach all shaders
            glAttachShader(m_glShader, uiVertexShader);
            glAttachShader(m_glShader, uiFragShader);
            if(vGeometryCode)
                glAttachShader(m_glShader, uiGeometryShader);
            
            
            
            //Now that the shader is compiled we analyze it and find all
            //uniform and attribute variables
            //m_bAnalyzed = analyze();
            
            // Link the program
            glLinkProgram(m_glShader);
            
            // Check if linking succeeded in the same way we checked for compilation success
            GLint bLinked;
            glGetProgramiv(m_glShader, GL_LINK_STATUS, &bLinked);
            
            if (!bLinked)
            {
                int i32InfoLogLength, i32CharsWritten;
                glGetProgramiv(m_glShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
                char* pszInfoLog = new char[i32InfoLogLength];
                glGetProgramInfoLog(m_glShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
                
                char* pszMsg = new char[i32InfoLogLength+256];
                strcpy(pszMsg, "Failed to link program: ");
                strcat(pszMsg, pszInfoLog);
                
                SAFE_DELETE_ARRAY(pszMsg);
                SAFE_DELETE_ARRAY(pszInfoLog);
                return false;
            }
            m_isCompiled = true;
            return true;
        }
        
        void GLShader::start()
        {
            if(!isReadyToRun())
                return;
            //Run program
            glUseProgram(m_glShader);
            m_isRunning = true;
        }
        
        void GLShader::stop()
        {
            glUseProgram(0);
            m_isRunning = false;
        }
        
        bool GLShader::removeAllCppComments(AnsiStr& strCode)
        {
            AnsiStr strOutput;
            
            char ch1, ch2;
            bool bAppend = true;
            bool bSingleLineComment = false;
            
            strCode.replaceChars('\t', ' ');
            int i=0;
            while(i < (strCode.length() - 1))
            {
                ch1 = strCode[i];
                ch2 = strCode[i+1];
                if((ch1 == '/')&&(ch2 == '/'))
                {
                    //Line Comment
                    bAppend = false;
                    bSingleLineComment = true;
                }
                else if((ch1 == '/')&&(ch2 == '*'))
                {
                    bAppend = false;
                    bSingleLineComment = false;
                }
                
                
                if(bAppend)
                {
                    strOutput.appendFromT(ch1);
                    if(i == strCode.length() - 2)
                        strOutput.appendFromT(ch2);
                }
                else
                {
                    if(bSingleLineComment)
                    {
                        if((ch1 == '\r')&&(ch2 == '\n'))
                        {
                            bAppend = true;
                            i++;
                        }
                    }
                    else
                    {
                        if((ch1 == '*')&&(ch2 == '/'))
                        {
                            bAppend = true;
                            i++;
                        }
                    }
                }
                
                i++;
            }//End While
            
            strCode = strOutput;
            
            return true;
        }
        /*
         bool GLShader::analyze()
         {
         if(m_strVertexShaderCode.length() == 0)
         return false;
         if(m_strFragmentShaderCode.length() == 0)
         return false;
         
         m_vAttribs.resize(0);
         m_vUniforms.resize(0);
         
         std::vector<AnsiStr> vLines;
         std::vector<AnsiStr> vContent;
         AnsiStr strLine, strWord;
         VARPROP var;
         int step = 0;
         int ctDetected = 0;
         
         AnsiStr strCode = m_strVertexShaderCode;
         //Need to replace tab with space
         removeAllCppComments(strCode);
         
         //Append all lines of code
         if(strCode.decompose(';', vContent) > 0)
         {
         for(size_t i=0; i<vContent.size(); i++)
         vLines.push_back(vContent[i]);
         }
         //PS::FILESTRINGUTILS::WriteTextFile(m_strVertexShaderFile + ".txt", vLines);
         vContent.resize(0);
         
         /////////////////////////////////////////////////////////////////////////////////////
         strCode = m_strFragmentShaderCode;
         removeAllCppComments(strCode);
         
         if(strCode.decompose(';', vContent) > 0)
         {
         for(size_t i=0; i<vContent.size(); i++)
         vLines.push_back(vContent[i]);
         }
         //PS::FILESTRINGUTILS::WriteTextFile(m_strFragmentShaderFile + ".txt", vLines);
         vContent.resize(0);
         
         
         //Now process line by line
         for(size_t i=0; i<vLines.size(); i++)
         {
         step = 0;
         strLine = vLines[i];
         //attribute highp vec4 myVertex;
         //attribute mediump vec4 myUV;
         //uniform mediump mat4 myMVPMatrix;
         //USAGE + PRECISION + TYPE + NAME
         int pos;
         
         while(strLine.length() > 0)
         {
         if(strLine.lfind(' ', pos))
         {
         strWord = strLine.substr(0, pos);
         strLine = strLine.substr(pos);
         }
         else
         {
         strWord = strLine;
         strLine.resize(0);
         }
         
         strWord.removeStartEndSpaces();
         strLine.removeStartEndSpaces();
         
         //See if this is a var line
         if(step == 0)
         {
         if(strWord == AnsiStr("attribute"))
         var.usage = vuAttribute;
         else if(strWord == AnsiStr("uniform"))
         var.usage = vuUniform;
         else
         break;
         }
         else
         {
         if(step == 1)
         {
         if(strWord == AnsiStr("lowp"))
         var.precision = vpLow;
         else if(strWord == AnsiStr("mediump"))
         var.precision = vpMedium;
         else if(strWord == AnsiStr("highp"))
         var.precision = vpHigh;
         else
         {
         //Precision is optional so set type name here and increment step
         var.precision = vpUndefined;
         var.strType = strWord;
         step++;
         }
         }
         else if(step == 2)
         {
         var.strType = strWord;
         }
         else if(step == 3)
         {
         var.strName = strWord;
         if(var.usage == vuAttribute)
         {
         //Set Attribute location
         var.idxLocation = (int)m_vAttribs.size();
         glBindAttribLocation(m_glShader, var.idxLocation, var.strName.ptr());
         m_vAttribs.push_back(var);
         }
         else
         {
         //Get assigned uniform location for this uniform variable
         var.idxLocation = glGetUniformLocation(m_glShader, var.strName.ptr());
         m_vUniforms.push_back(var);
         }
         ctDetected++;
         break;
         }
         }
         
         step++;
         }
         }
         
         return true;
         }
         */
        
        
        bool GLShader::readShaderCode(const AnsiStr& strFilePath, AnsiStr& strCode)
        {
            std::ifstream fp;
            fp.open(strFilePath.ptr(), std::ios::binary);
            if(!fp.is_open())
                return false;
            
            size_t size;
            fp.seekg(0, std::ios::end);
            size = fp.tellg();
            fp.seekg(0, std::ios::beg);
            
            char * buf = new char[size+1];
            //Read file content
            fp.read(buf, size);
            buf[size] = '\0';
            
            strCode = AnsiStr(buf);
            SAFE_DELETE(buf);
            fp.close();
            
            return true;
        }
        
        bool GLShader::isReadyToRun()
        {
            return (m_isCompiled && (glIsProgram(m_glShader) == GL_TRUE));
        }
        
        
        bool GLShader::isBinaryShaderSupported() {
            return false;
        }
        
        bool GLShader::isGLExtensionSupported(const char *extension)
        {
            // The recommended technique for querying OpenGL extensions;
            // from http://opengl.org/resources/features/OGLextensions/
            const GLubyte *extensions = NULL;
            const GLubyte *start;
            GLubyte *where, *terminator;
            
            // Extension names should not have spaces.
            where = (GLubyte *) strchr(extension, ' ');
            if (where || *extension == '\0')
                return 0;
            
            extensions = glGetString(GL_EXTENSIONS);
            
            // It takes a bit of care to be fool-proof about parsing the
            // OpenGL extensions string. Don't be fooled by sub-strings, etc.
            start = extensions;
            for (;;) {
                where = (GLubyte *) strstr((const char *) start, extension);
                if (!where)
                    break;
                terminator = where + strlen(extension);
                if (where == start || *(where - 1) == ' ')
                    if (*terminator == ' ' || *terminator == '\0')
                        return true;
                start = terminator;
            }
            
            return false;
        }
        
#ifdef SAVE_LOAD_BINARY_SHADER
        /*!****************************************************************************
         @Function		saveBinaryProgram
         @Return		bool	True if save succeeded.
         @Description	This function takes as input the ID of a shader program object
         which should have been created prior to calling this function,
         as well as a filename to save the binary program to.
         The function will save out a file storing the binary shader
         program, and the enum value determining its format.
         ******************************************************************************/
        bool GLShader::saveBinaryProgram(const char* Filename, GLuint &ProgramObjectID)
        {
#if !defined (TARGET_OS_IPHONE)
            //Quick check to make sure that the program actually exists.
            GLint linked;
            glGetProgramiv(ProgramObjectID, GL_LINK_STATUS, &linked);
            if (!linked)
            {
                //Shaders not linked correctly, no binary to retrieve.
                return false;
            }
            
            // Get the length of the shader binary program in memory.
            // Doing this ensures that a sufficient amount of memory is allocated for storing the binary program you retrieve.
            GLsizei length=0;
            glGetProgramiv(ProgramObjectID,GL_PROGRAM_BINARY_LENGTH_OES,&length);
            
            // Pointer to the binary shader program in memory, needs to be allocated with the right size.
            GLvoid* ShaderBinary = (GLvoid*)malloc(length);
            
            // The format that the binary is retreived in.
            GLenum binaryFormat=0;
            
            // Error checking variable - this should be greater than 0 after glGetProgramBinaryOES, otherwise there was an error.
            GLsizei lengthWritten=0;
            
            // Get the program binary from GL and save it out.
            glGetProgramBinaryOES(ProgramObjectID,length,&lengthWritten,&binaryFormat,ShaderBinary);
            if (!lengthWritten)
            {
                // Save failed. Insufficient memory allocated to write binary shader.
                return false;
            }
            
            // Cache the program binary for future runs
            FILE* outfile = fopen(Filename, "wb");
            
            if(!outfile)
            {
                printf("Failed to open %s for writing to.\n", Filename);
                return false;
            }
            
            // Save the binary format.
            if(!fwrite((void*)&binaryFormat,sizeof(GLenum),1,outfile)) return false; // Couldn't write binary format to file.
            
            // Save the actual binary program.
            if(!fwrite(ShaderBinary, length,1,outfile)) return false;				 // Couldn't write binary data to file.
            
            // Close the file.
            fclose(outfile);
            
            // Free the memory used by Shader Binary.
            free(ShaderBinary);
            return true;
#else
            return false;
#endif
        }
        
        
        /*!****************************************************************************
         @Function		loadBinaryProgram
         @Return		bool	True if load succeeded.
         @Description	This function takes as input the ID of a shader program object
         which should have been created prior to calling this function,
         as well as a filename to load the binary program from.
         The function will load in a file storing the binary shader
         program, and the enum value determining its format.
         It will then load the binary into memory.
         
         @Note:			This function is not able to check if the shaders have changed.
         If you change the shaders then the file this saves out either
         needs to be deleted	or a new file used.
         ******************************************************************************/
        bool GLShader::loadBinaryProgram(const char* Filename, GLuint &ProgramObjectID)
        {
#if !defined (TARGET_OS_IPHONE)
            // Open the file.
            FILE* infile = fopen(Filename, "rb");
            
            // File open failed, either doesn't exist or is empty.
            if (!infile) return false;
            
            // Find initialise the shader binary.
            fseek(infile, 0, SEEK_END);
            GLsizei length = (GLint)ftell(infile)-sizeof(GLenum);
            
            if (!length) return false;	// File appears empty.
            
            // Allocate a buffer large enough to store the binary program.
            GLvoid* ShaderBinary = (GLvoid*)malloc(length);
            
            // Read in the binary format
            GLenum format=0;
            fseek(infile, 0, SEEK_SET);
            fread(&format, sizeof(GLenum), 1, infile);
            
            // Read in the program binary.
            fread(ShaderBinary, length, 1, infile);
            fclose(infile);
            
            // Create an empty shader program
            ProgramObjectID = glCreateProgram();
            
            // Load the binary into the program object -- no need to link!
            glProgramBinaryOES(ProgramObjectID, format, ShaderBinary, length);
            
            // Delete the binary program from memory.
            free(ShaderBinary);
            
            // Check that the program was loaded correctly, uses the same checks as when linking with a standard shader.
            GLint loaded;
            glGetProgramiv(ProgramObjectID, GL_LINK_STATUS, &loaded);
            if (!loaded)
            {
                // Something must have changed. Need to recompile shaders.
                int i32InfoLogLength, i32CharsWritten;
                glGetProgramiv(ProgramObjectID, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
                char* pszInfoLog = new char[i32InfoLogLength];
                glGetProgramInfoLog(ProgramObjectID, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
                char* pszMsg = new char[i32InfoLogLength+256];
                strcpy(pszMsg, "Failed to load binary program: ");
                strcat(pszMsg, pszInfoLog);
                printf(pszMsg);
                
                delete [] pszMsg;
                delete [] pszInfoLog;
                return false;
            }
            return true;
#else
            return false;
#endif
        }
        
#endif
    }
}
