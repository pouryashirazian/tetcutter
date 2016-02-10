#include "gltexturemanager.h"
#include "base/directory.h"

using namespace ps::opengl;

TexManager::TexManager() {

}

TexManager::~TexManager() {
    TexManagerParent::cleanup();
}

bool TexManager::add(const AnsiStr &strFP) {
    GLTexture* tex = new GLTexture(strFP);
    if(tex) {
        return TexManagerParent::add(tex, ps::dir::ExtractFileTitleOnly(strFP).cptr());
    }

    SAFE_DELETE(tex);
    return false;
}
