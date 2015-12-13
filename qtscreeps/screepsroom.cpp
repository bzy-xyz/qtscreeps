#include "screepsroom.h"
#include "screepsnetworkmanager.h"
#include "screepsterraincache.h"

ScreepsRoom::ScreepsRoom(QObject * parent)
    : QObject(parent)
{
    worldBackground = QSharedPointer<ScreepsRoomMap2>::create(this);
}

ScreepsRoom::ScreepsRoom(QObject *parent, QString roomName)
    : ScreepsRoom(parent)
{
    this->roomName = roomName;
}

void ScreepsRoom::UpdateRoomTerrain(QString terrainData)
{
    worldBackground->setTerrain(terrainData);
    this->terrainData = terrainData;

    ScreepsTerrainCache & cfgStore = ScreepsTerrainCache::defaultInstance();

    QString cachedTerrain = cfgStore.get(QString("terrain_")+this->roomName);
    if(!cachedTerrain.length())
    {
        cfgStore.set(QString("terrain_")+this->roomName, terrainData);
    }
}

void ScreepsRoom::UpdateRoomMap2Data(QJsonObject rm2data)
{
    worldBackground->updateRoomMap2(rm2data);
    if(terrainData == QString())
    {
        emit RequestRoomTerrain(roomName);
    }
}

void ScreepsRoom::install(QGraphicsScene *scene, qreal x, qreal y)
{
    scene->addItem(worldBackground.data());
    worldBackground->setPos(x, y);
}

void ScreepsRoom::handleInViewport()
{
    if(!hasRequestedTerrain)
    {
        hasRequestedTerrain = true;

        // try to load terrain from cache
        ScreepsTerrainCache & cfgStore = ScreepsTerrainCache::defaultInstance();

        QString cachedTerrain = cfgStore.get(QString("terrain_")+this->roomName);
        if(cachedTerrain.length())
        {
            this->UpdateRoomTerrain(cachedTerrain);
        }
        else
        {
            emit RequestRoomTerrain(roomName);
        }
    }

    if(!hasRequestedMapData)
    {
        hasRequestedMapData = true;
        emit SubscribeRoomMap2(roomName);
    }
}
