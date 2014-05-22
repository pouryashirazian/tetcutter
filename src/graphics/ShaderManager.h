#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <loki/Singleton.h>
#include "base/MathBase.h"
#include "base/FastAccessToNamedResource.h"
#include "GLShader.h"

using namespace std;
using namespace Loki;
using namespace PS::GL;

namespace PS {

void ReleaseShader(U32 shader);

template<typename T>
struct ShaderNoopInsertGLRemove {
    static void Insert(T element) {
        PS_UNUSED(element);
    }

    static void Remove(T element) {
        ReleaseShader(element);
    }
};

/*!
 * \brief The ShaderManager class is a collection for all shader programs
 * in a graphics app and provides:
 * 1.Named access to shaders
 * 2.Cleanup and releasing shaders
 * 3.Global Singleton access
 */
typedef FastAccessNamedResource<GLShader, TypePointer, NoopInsertRemoveBySafeDelete> ShaderManagerParent;

class ShaderManager : public ShaderManagerParent  {
public:
    ShaderManager();
    virtual ~ShaderManager();
    bool add(const char* name, const char* vShaderCode, const char* fShaderCode, const char* gShaderCode = NULL);
    
    bool addFromFile(const char* name,
                     const AnsiStr& strVShaderFP,
                     const AnsiStr& strFShaderFP);

    bool addFromFile(const char* name,
                     const AnsiStr& strVShaderFP,
                     const AnsiStr& strFShaderFP,
                     const AnsiStr& strGShaderFP);

    int addFromFolder(const char* chrShadersPath);
};

typedef SingletonHolder<ShaderManager, CreateUsingNew, PhoenixSingleton> TheShaderManager;

}

#endif // SHADERMANAGER_H
