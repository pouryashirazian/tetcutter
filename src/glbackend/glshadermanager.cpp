#include "glshadermanager.h"
#include "glfuncs.h"
#include "base/logger.h"
#include "base/directory.h"
#include "glselect.h"

using namespace ps;
using namespace ps::dir;
using namespace ps::opengl;
using namespace ps::utils;


void ReleaseShader(U32 shader) {
    if(glIsProgram(shader))
        glDeleteProgram(shader);
}

ShaderManager::ShaderManager() {

}

ShaderManager::~ShaderManager() {
    ShaderManagerParent::cleanup();
}


bool ShaderManager::add(const char* name, const char* vShaderCode, const char* fShaderCode, const char* gShaderCode) {
    GLShader* aShader = new GLShader(vShaderCode, fShaderCode, gShaderCode);
    if(aShader->isCompiled()) {
        if(ShaderManagerParent::add(aShader, name)) {
            if(gShaderCode != NULL)
                vloginfo("Added Vertex-Geometry-Fragment Shader from inline code. Name: %s.", name);
            else
                vloginfo("Added Vertex-Fragment Shader from inline code. Name: %s.", name);
            return true;
        }
        else {
            vlogerror("Unable to add inline shader named: %s", name);
            return false;
        }
    }
    else
    {
        SAFE_DELETE(aShader);
        return false;
    }
}

bool ShaderManager::addFromFile(const char* name,
                                const AnsiStr& strVShaderFP,
                                const AnsiStr& strFShaderFP) {
    AnsiStr strTitle = (name != NULL) ? AnsiStr(name) : ExtractFileTitleOnly(strVShaderFP);

    GLShader* aShader = new GLShader();
    if(aShader->compileFromFile(strVShaderFP, strFShaderFP)) {
        if(ShaderManagerParent::add(aShader, strTitle.cptr())) {
            AnsiStr strArg = TheEventLogger::Instance().shortenPathBasedOnRoot(strVShaderFP);
            vloginfo("Added Vertex-Fragment Shader from file: %s, Name: %s.", strArg.cptr(), name);
            return true;
        }
        else {
            SAFE_DELETE(aShader);
            vlogerror("Unable to add Vertex-Fragment Shader from file. Name: %s", name);
            return false;
        }
    }
    else {
        SAFE_DELETE(aShader);
        return false;
    }
}

bool ShaderManager::addFromFile(const char* name,
                                const AnsiStr& strVShaderFP,
                                const AnsiStr& strFShaderFP,
                                const AnsiStr& strGShaderFP) {

    AnsiStr strTitle = (name != NULL) ? AnsiStr(name) : ExtractFileTitleOnly(strVShaderFP);

    GLShader* aShader = new GLShader();
    if(aShader->compileFromFile(strVShaderFP, strFShaderFP, strGShaderFP)) {
        if(ShaderManagerParent::add(aShader, strTitle.cptr())) {
            AnsiStr strArg = TheEventLogger::Instance().shortenPathBasedOnRoot(strVShaderFP);
            vloginfo("Added Vertex-Geometry-Fragment Shader from file: %s, Name: %s.", strArg.cptr(), name);
            return true;
        }
        else {
            SAFE_DELETE(aShader);
            vlogerror("Unable to add Vertex-Geometry-Fragment Shader from file. Name: %s", name);
            return false;

        }
    }
    else {
        SAFE_DELETE(aShader);
        return false;
    }
}

int ShaderManager::addFromFolder(const char* chrShadersPath)
{
    int count = 0;
    vector<AnsiStr> vFiles;
    ListFilesInDir(vFiles, chrShadersPath, "vsh", true);

    for(int i=0; i<(int)vFiles.size(); i++) {
        AnsiStr strVShaderFP = vFiles[i];
        AnsiStr strFShaderFP = ChangeFileExt(strVShaderFP, AnsiStr(".fsh"));
        AnsiStr strGShaderFP = ChangeFileExt(strVShaderFP, AnsiStr(".gsh"));
        AnsiStr strTitle = ExtractFileTitleOnly(strVShaderFP);

        if(FileExists(strGShaderFP)) {
            if(addFromFile(strTitle.cptr(), strVShaderFP, strFShaderFP, strGShaderFP)) {
                count++;
            }
        }
        else {
            if(addFromFile(strTitle.cptr(), strVShaderFP, strFShaderFP)) {
                count++;
            }
        }
    }

    return count;
}

