#include "screepsterraincache.h"

ScreepsTerrainCache::ScreepsTerrainCache(QObject *parent) : QObject(parent)
{
    configStore = QSharedPointer<ScreepsLocalConfigStore>::create(this, "terrainCache");
}

ScreepsTerrainCache::~ScreepsTerrainCache()
{
    if(!configStore.isNull())
    {
        configStore->setBatch(QVariant(previouslyUncached.keys()).toList(), QVariant(previouslyUncached.values()).toList());
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
    if(!inMemoryCache.contains(key))
    {
        previouslyUncached[key] = value;
    }
    inMemoryCache[key] = value;
    return true;
}
