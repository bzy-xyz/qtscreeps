#include "screepsmaingraphicsview.h"
#include "screepsnetworkmanager.h"

#include "screepsroommap2.h"
#include "screepscoordutils.h"

ScreepsMainGraphicsView::ScreepsMainGraphicsView(QWidget * parent)
    : QGraphicsView(parent)
{

    this->setScene(&scene);
    this->setSceneRect(-1550, -1550, 4600, 4600);
    this->scale(2.0, 2.0);
    this->centerOn(0, 0);
    this->setDragMode(QGraphicsView::ScrollHandDrag);

    ScreepsNetworkManager & nm = ScreepsNetworkManager::defaultInstance();

    //connect(this, SIGNAL(RequestRoomTerrain(QString)), &nm, SLOT(DoGetRoomTerrain(QString)));
    connect(&nm, SIGNAL(GotRoomTerrain(QString,QString)), this, SLOT(GotRoomTerrain(QString,QString)));

    //connect(this, SIGNAL(SubscribeRoomMap2(QString)), &nm, SLOT(DoSubscribeRoomMap2(QString)));
    connect(&nm, SIGNAL(GotRoomMap2Data(QString,QJsonObject)), this, SLOT(GotRoomMap2Update(QString,QJsonObject)));

    // preallocating rooms statically for now...
    for(int x = 0; x < 4600; x += 50)
    {
        for(int y = 0; y < 4600; y += 50)
        {
            this->__room_create(QtScreeps::roomNameFromBaseCoords(x - 1550, y - 1550), x - 1550, y - 1550);
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

    QList<QGraphicsItem *> visibleItems = scene.items(r);
    foreach(QGraphicsItem * i, visibleItems)
    {
        if(i->type() == ScreepsRoomMap2::Type)
        {
            ScreepsRoomMap2 * ii = qgraphicsitem_cast<ScreepsRoomMap2*>(i);
            if(ii)
            {
                ii->attachedRoomInViewport();
            }
        }
    }
}
