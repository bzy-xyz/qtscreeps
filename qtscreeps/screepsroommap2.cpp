#include "screepsroommap2.h"
#include "screepsnetworkmanager.h"

ScreepsRoomMap2::ScreepsRoomMap2(QGraphicsItem * parent)
    : QGraphicsItem(parent)
{
    terrain = QByteArray(2500, 0);
}

ScreepsRoomMap2::ScreepsRoomMap2(ScreepsRoom *aR, QGraphicsItem *parent)
    : ScreepsRoomMap2(parent)
{
    attachedRoom = aR;
}

QRectF ScreepsRoomMap2::boundingRect() const
{
    return QRectF(-0.5, -0.5, 50, 50);
}

void ScreepsRoomMap2::setTerrain(QString serialized)
{
    //qDebug() << "update terrain " << serialized;
    terrain = serialized.toLatin1();
    this->update();
}

void ScreepsRoomMap2::updateRoomMap2(QJsonObject rm2)
{
    //qDebug() << "update rm2 " << rm2;
    roomMap2 = rm2;
    this->update();
}

void ScreepsRoomMap2::attachedRoomInViewport()
{
    attachedRoom->handleInViewport();
}

/**
 * @brief Magic happens here.
 * @param painter
 * @param option
 * @param widget
 */
void ScreepsRoomMap2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ScreepsNetworkManager & nm = ScreepsNetworkManager::defaultInstance();

    // draw the base terrain
    for(int y = 0; y < 50; y++)
    {
        for(int x = 0; x < 50; x++)
        {
            QColor c;
            switch(terrain[y * 50 + x])
            {
            case '0':
                c = x == 0 || x == 49 || y == 0 || y == 49
                        ? QColor(144, 144, 144)
                        : QColor(96, 96, 96);
                break;
            case '1':
            case '3':
                c = QColor(32, 32, 32);
                break;
            case '2':
                c = QColor(32, 96, 32);
                break;
            default:
                c = QColor(0,0,0);
                break;
            }

            painter->fillRect(QRectF(x-0.5, y-0.5, 1, 1), c);
        }
    }

    // draw the walls and roads
    if(roomMap2.contains("walls"))
    {
        QJsonArray walls = roomMap2["walls"].toArray();
        for(auto i = walls.begin(); i != walls.end(); i++)
        {
            QJsonArray p = (*i).toArray();
            qreal x = p[0].toInt();
            qreal y = p[1].toInt();
            QColor c = QColor(32, 32, 32);

            painter->fillRect(QRectF(x-0.5, y-0.5, 1, 1), c);
        }
    }

    if(roomMap2.contains("roads"))
    {
        QJsonArray roads = roomMap2["roads"].toArray();
        for(auto i = roads.begin(); i != roads.end(); i++)
        {
            QJsonArray p = (*i).toArray();
            qreal x = p[0].toInt();
            qreal y = p[1].toInt();
            QColor c = QColor(152, 152, 152);

            painter->fillRect(QRectF(x-0.5, y-0.5, 1, 1), c);
        }
    }

    if(roomMap2.contains("pb"))
    {
        QJsonArray pbs = roomMap2["pb"].toArray();
        for(auto i = pbs.begin(); i != pbs.end(); i++)
        {
            QJsonArray p = (*i).toArray();
            qreal x = p[0].toInt();
            qreal y = p[1].toInt();
            QColor c = QColor(204, 32, 204);

            painter->fillRect(QRectF(x-0.5, y-0.5, 1, 1), c);
        }
    }

    // draw player owned stuff
    foreach(QString k, roomMap2.keys())
    {
        if(k != "walls" && k != "roads" && k != "pb")
        {
            QJsonArray things = roomMap2[k].toArray();
            for(auto i = things.begin(); i != things.end(); i++)
            {
                QJsonArray p = (*i).toArray();
                qreal x = p[0].toInt();
                qreal y = p[1].toInt();
                QColor c = nm.getMeParam("_id") == k
                        ? QColor(0, 255, 0)
                        : QColor(255, 0, 0);

                painter->fillRect(QRectF(x-0.5, y-0.5, 1, 1), c);
            }
        }
    }
}
