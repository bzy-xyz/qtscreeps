#include "screepsmaingraphicsview.h"
#include "screepsnetworkmanager.h"

#include "screepsroommap2.h"
#include "screepscoordutils.h"

ScreepsMainGraphicsView::ScreepsMainGraphicsView(QWidget * parent)
    : QGraphicsView(parent)
{

    this->setScene(&scene);
    this->setSceneRect(SCREEPS_MINX, SCREEPS_MINY, SCREEPS_XSIZE, SCREEPS_YSIZE);
    this->scale(2.0, 2.0);
    this->centerOn(0, 0);
    this->setDragMode(QGraphicsView::ScrollHandDrag);

    ScreepsNetworkManager & nm = ScreepsNetworkManager::defaultInstance();

    //connect(this, SIGNAL(RequestRoomTerrain(QString)), &nm, SLOT(DoGetRoomTerrain(QString)));
    connect(&nm, SIGNAL(GotRoomTerrain(QString,QString)), this, SLOT(GotRoomTerrain(QString,QString)));

    //connect(this, SIGNAL(SubscribeRoomMap2(QString)), &nm, SLOT(DoSubscribeRoomMap2(QString)));
    connect(&nm, SIGNAL(GotRoomMap2Data(QString,QJsonObject)), this, SLOT(GotRoomMap2Update(QString,QJsonObject)));

    // preallocating rooms statically for now...
    for(int x = 0; x < SCREEPS_XSIZE; x += 50)
    {
        for(int y = 0; y < SCREEPS_YSIZE; y += 50)
        {
            this->__room_create(QtScreeps::roomNameFromBaseCoords(
                                    x + SCREEPS_MINX,
                                    y + SCREEPS_MINY),
                                x + SCREEPS_MINX,
                                y + SCREEPS_MINY);
        }
    }

    this->__handle_viewport_translate();

}

void ScreepsMainGraphicsView::__room_create(QString roomName, qreal x, qreal y)
{
    ScreepsNetworkManager & nm = ScreepsNetworkManager::defaultInstance();

    rooms[roomName] = QSharedPointer<ScreepsRoom>::create(this, roomName);
    rooms[roomName]->install(&scene, x, y);
    //emit RequestRoomTerrain(roomName);
    //emit SubscribeRoomMap2(roomName);

    connect(rooms[roomName].data(), SIGNAL(RequestRoomTerrain(QString)), &nm, SLOT(DoGetRoomTerrain(QString)));
    connect(rooms[roomName].data(), SIGNAL(SubscribeRoomMap2(QString)), &nm, SLOT(DoSubscribeRoomMap2(QString)));
    connect(rooms[roomName].data(), SIGNAL(UnsubscribeRoomMap2(QString)), &nm, SLOT(DoUnsubscribeRoomMap2(QString)));
}

void ScreepsMainGraphicsView::GotRoomTerrain(QString roomName, QString terrainData)
{
    if(rooms.contains(roomName))
    {
        rooms[roomName]->UpdateRoomTerrain(terrainData);
    }
}

void ScreepsMainGraphicsView::GotRoomMap2Update(QString roomName, QJsonObject rm2)
{
    if(rooms.contains(roomName))
    {
        rooms[roomName]->UpdateRoomMap2Data(rm2);
    }
}

bool ScreepsMainGraphicsView::viewportEvent(QEvent *event)
{
    bool ret = QGraphicsView::viewportEvent(event);

    if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent * ev = static_cast<QMouseEvent *>(event);

        if(ev->buttons() & Qt::LeftButton)
        {
            __handle_viewport_translate();
        }
    }

    return ret;
}

void ScreepsMainGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    __handle_viewport_translate();
}

void ScreepsMainGraphicsView::__handle_viewport_translate()
{
    QRectF r = mapToScene(this->viewport()->rect()).boundingRect();

    QSet<QGraphicsItem *> currentlyVisibleItems = QSet<QGraphicsItem*>::fromList(scene.items(r));

    QSet<QGraphicsItem *> newlyVisibleItems = currentlyVisibleItems - roomsLastVisible;

    // notify rooms newly in viewport
    foreach(QGraphicsItem * i, newlyVisibleItems)
    {
        if(i->type() == ScreepsRoomMap2::Type)
        {
            ScreepsRoomMap2 * ii = qgraphicsitem_cast<ScreepsRoomMap2*>(i);
            if(ii)
            {
                ii->attachedRoomEnteredViewport();
            }
        }
    }

    // notify rooms no longer in viewport
    QSet<QGraphicsItem *> noLongerVisible = roomsLastVisible - currentlyVisibleItems;
    foreach(QGraphicsItem * i, noLongerVisible)
    {
        if(i->type() == ScreepsRoomMap2::Type)
        {
            ScreepsRoomMap2 * ii = qgraphicsitem_cast<ScreepsRoomMap2*>(i);
            if(ii)
            {
                ii->attachedRoomLeftViewport();
            }
        }
    }

    roomsLastVisible = currentlyVisibleItems;
}
