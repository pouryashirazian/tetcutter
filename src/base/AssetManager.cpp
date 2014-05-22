#include "AssetManager.h"
#include "base/Logger.h"

namespace PS {

Asset::Asset():m_isSerializeable(false), m_tsAccessed(0) {
}

Asset::~Asset() {

}

bool Asset::load(const char *chrFilePath) {
    return false;
}

bool Asset::store(const char *chrFilePath) {
    return false;
}


AssetManager::AssetManager() {

}

}
