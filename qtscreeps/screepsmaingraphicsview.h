#ifndef SCREEPSMAINGRAPHICSVIEW_H
#define SCREEPSMAINGRAPHICSVIEW_H

#include <QtCore>
#include <QtWidgets>

#include "screepsroom.h"

#define SCREEPS_MINX -1550
#define SCREEPS_MINY -1550
#define SCREEPS_XSIZE 3100
#define SCREEPS_YSIZE 3100

class ScreepsMainGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    ScreepsMainGraphicsView(QWidget * parent = 0);

signals:
    void RequestRoomTerrain(QString roomName);
    void SubscribeRoomMap2(QString roomName);
    void SubscribeRoomFeed(QString roomName);

    void StatusBarSignal(QString msg);

public slots:
    void GotRoomTerrain(QString roomName, QString terrainData);
    void GotRoomMap2Update(QString roomName, QJsonObject rm2);

protected:
    virtual bool viewportEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent * event);

private:
    QGraphicsScene scene;
    QMap<QString, QSharedPointer<ScreepsRoom>> rooms;

    QSet<QGraphicsItem *> roomsLastVisible;

    void __room_create(QString roomName, qreal x, qreal y);

    void __handle_viewport_translate();
};

#endif // SCREEPSMAINGRAPHICSVIEW_H
