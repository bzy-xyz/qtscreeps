#include "screepsrngutils.h"

// RNG for low-entropy applications
std::mt19937_64 rng;
bool isInit = 0;

void random_init()
{
    if(!isInit)
    {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(seed);
        isInit = true;
    }
}

quint32 random_uint32()
{
    return (quint32)rng();
}

quint64 random_uint64()
{
    return (quint64)rng();
}

qint32 random_int(qint32 min, qint32 max)
{
    std::uniform_int_distribution<qint32> d(min, max);
    return d(rng);
}

qreal random_real(qreal max)
{
    std::uniform_real_distribution<qreal> d(0.0, max);
    return d(rng);
}

QByteArray random_bytes(size_t bytes)
{
    QByteArray ret = QByteArray(bytes, 0);
    std::uniform_int_distribution<char> d(0, 255);
    for(int i = 0; i < bytes; i++)
    {
        ret[i] = d(rng);
    }
    return ret;
}
