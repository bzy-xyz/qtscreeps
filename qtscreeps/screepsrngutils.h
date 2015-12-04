#ifndef SCREEPSRNGUTILS_H
#define SCREEPSRNGUTILS_H

#include <QtCore>
#include <random>
#include <chrono>

void random_init();

quint32 random_uint32();
quint64 random_uint64();

qint32 random_int(qint32 min = 0, qint32 max = 999);
qreal random_real(qreal max = 1.0);

QByteArray random_bytes(size_t bytes);

#endif // SCREEPSRNGUTILS_H
