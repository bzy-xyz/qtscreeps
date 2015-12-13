#include "screepscoordutils.h"

#include <QtCore>

/**
 * @brief Returns the name of the room containing the given world coordinates.
 *
 * By convention, [0, 0] in world coordinates is the tile [0, 0] in the room E0S0.
 *
 * @param x
 * @param y
 * @return
 */
QString QtScreeps::roomNameFromBaseCoords(qreal x, qreal y)
{
    int roomX = qFloor((x + 0.5) / 50);
    int roomY = qFloor((y + 0.5) / 50);

    QString xFrag =
            roomX >= 0
            ? QString("E%1").arg(roomX)
            : QString("W%1").arg(-roomX - 1);
    QString yFrag =
            roomY >= 0
            ? QString("S%1").arg(roomY)
            : QString("N%1").arg(-roomY - 1);

    return xFrag + yFrag;
}
