#ifndef SCREEPSROOMMAP2_H
#define SCREEPSROOMMAP2_H

#include <QtCore>
#include <QtWidgets>

#include "screepsroom.h"

class ScreepsRoom;

/**
 * @brief A GraphicsItem representing a roomMap2 display.
 *
 * Rooms in QtScreeps are aligned such that 0, 0 in graphics relative coordinates is
 * placed at the center of the tile [0, 0], and drawn at 1:1 scale.
 * This naturally means that all drawing must be done in float coordinates,
 * and that the bounding rectangle of the room extends from -0.5 to 49.5
 * on both coordinate axes.
 */
class ScreepsRoomMap2 : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    ScreepsRoomMap2(QGraphicsItem * parent = 0);
    ScreepsRoomMap2(ScreepsRoom * attachedRoom, QGraphicsItem * parent = 0);

    void setTerrain(QString serialized);
    void updateRoomMap2(QJsonObject rm2);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    void attachedRoomInViewport();

    int type() const
    {
        return Type;
    }

private:
    ScreepsRoom * attachedRoom;

    QJsonObject roomMap2;
    QByteArray terrain;
};

#endif // SCREEPSROOMMAP2_H
