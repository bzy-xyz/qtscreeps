#include "screepsterraincache.h"

ScreepsTerrainCache::ScreepsTerrainCache(QObject *parent) : QObject(parent)
{
    configStore = QSharedPointer<ScreepsLocalConfigStore>::create(this, "terrainCache");
}

ScreepsTerrainCache::~ScreepsTerrainCache()
{
    if(!configStore.isNull())
    {
        configStore->setBatch(QVariant(inMemoryCache.keys()).toList(), QVariant(inMemoryCache.values()).toList());
    }
}

QString ScreepsTerrainCache::get(QString key)
{
    if(!inMemoryCache.contains(key))
    {
        QString s = configStore->get(key);
        if(s.length())
        {
            inMemoryCache[key] = s;
        }
        return s;
    }
    else
    {
        return inMemoryCache[key];
    }
}

bool ScreepsTerrainCache::set(QString key, QString value)
{
    inMemoryCache[key] = value;
    return true;
}
