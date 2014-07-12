//
//  TexManager.h
//  hifem
//
//  Created by pshiraz on 1/21/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#ifndef hifem_TexManager_h
#define hifem_TexManager_h

#include "loki/Singleton.h"
#include "base/FastAccessToNamedResource.h"
#include "GLTexture.h"

using namespace Loki;

namespace PS {
    namespace GL {
    
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
