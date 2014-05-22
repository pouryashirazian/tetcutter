#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H
#include <string>
#include <map>
#include <loki/Singleton.h>
#include <loki/SmartPtr.h>
#include "base/FastAccessToNamedResource.h"

using namespace std;
using namespace Loki;

namespace PS {

//Using LRU algorithm the least recently used asset will be freed
class Asset {
public:
    Asset();
    virtual ~Asset();

    string name() const {return m_name;}
    void setName(const string& name) {m_name = name;}
    U64 lastAccessedTimeStamp() { return m_tsAccessed;}


    //Serialize to disk
    bool isSerializeable() const {return m_isSerializeable;}
    virtual bool load(const char* chrFilePath);
    virtual bool store(const char* chrFilePath);
protected:
    bool m_isSerializeable;
    U64 m_tsAccessed;
    string m_name;
};

/*!
 * AssetManager is a repository for all application assets which may include:
 * 1.Textures
 * 2.Meshes
 * 3.Materials
 * 4.Config files
 * Asset manager provides named access to the assets and an intelligent
 * mechanism for releasing unused assets and accessing used ones.
 * It can also log asset access information into an sqlite database with timing and detailed info.
 * It can be further extended to be controlled by a network server of assets and requests to be
 * fullfilled from those servers.
 */
class AssetManager : public PS::FastAccessNamedResource<Asset> {
public:
    AssetManager();

};

typedef SingletonHolder<AssetManager, CreateUsingNew, PhoenixSingleton> TheAssetManager;

}

#endif // ASSETMANAGER_H
