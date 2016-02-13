#include "sgassetmanager.h"
#include "base/logger.h"

using namespace ps::scene;

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

