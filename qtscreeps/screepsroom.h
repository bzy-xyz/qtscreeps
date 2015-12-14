#ifndef SCREEPSROOM_H
#define SCREEPSROOM_H

#include <QtCore>
#include <QtWidgets>

#include "screepsroommap2.h"

class ScreepsRoomMap2;

class ScreepsRoom : public QObject
{
    Q_OBJECT
public:
    explicit ScreepsRoom(QObject * parent = 0);

    ScreepsRoom(QObject *parent, QString roomName);

    void install(QGraphicsScene * scene, qreal x, qreal y);

    void handleEnteredViewport();
    void handleLeftViewport();

signals:
    void RequestRoomTerrain(QString roomName);
    void SubscribeRoomMap2(QString roomName);
    void SubscribeRoomFeed(QString roomName);

    void UnsubscribeRoomMap2(QString roomName);
    void UnsubscribeRoomFeed(QString roomName);

public slots:
    //void update();

    void UpdateRoomTerrain(QString terrainData);
    void UpdateRoomMap2Data(QJsonObject rm2data);

private:
    QSharedPointer<ScreepsRoomMap2> worldBackground;

    QString roomName;

    QString terrainData;

    bool hasRequestedTerrain = false;
    bool hasRequestedMapData = false;

};

#endif // SCREEPSROOM_H
