//
//  TexManager.cpp
//  hifem
//
//  Created by pshiraz on 1/21/14.
//  Copyright (c) 2014 pshiraz. All rights reserved.
//

#include "TexManager.h"
#include "base/FileDirectory.h"

using namespace PS::FILESTRINGUTILS;

namespace PS {
    namespace GL {
        TexManager::TexManager() {
            
        }
        
        TexManager::~TexManager() {
            TexManagerParent::cleanup();
        }

        bool TexManager::add(const AnsiStr &strFP) {
            GLTexture* tex = new GLTexture(strFP);
            if(tex) {
                return TexManagerParent::add(tex, ExtractFileTitleOnly(strFP).cptr());
            }
            
            SAFE_DELETE(tex);
            return false;
        }
    }
}