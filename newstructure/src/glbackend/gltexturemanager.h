#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "loki/Singleton.h"
#include "base/resourcemanager.h"
#include "gltexture.h"

using namespace Loki;

namespace ps {
    namespace opengl {
    
    typedef FastAccessNamedResource<GLTexture, TypePointer, NoopInsertRemoveBySafeDelete> TexManagerParent;
    
    class TexManager : public TexManagerParent  {
    public:
        TexManager();
        virtual ~TexManager();
        bool add(const AnsiStr& strFP);
    };
    
    typedef SingletonHolder<TexManager, CreateUsingNew, PhoenixSingleton> TheTexManager;
        
    }
}


#endif
