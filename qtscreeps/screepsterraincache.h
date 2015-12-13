#ifndef SCREEPSTERRAINCACHE_H
#define SCREEPSTERRAINCACHE_H

#include <QtCore>
#include "screepslocalconfigstore.h"

class ScreepsTerrainCache : public QObject
{
    Q_OBJECT
public:

    static ScreepsTerrainCache & defaultInstance()
    {
        static ScreepsTerrainCache instance;
        return instance;
    }

    explicit ScreepsTerrainCache(QObject *parent = 0);

    ~ScreepsTerrainCache();

    QString get(QString key);
    bool set(QString key, QString value);

signals:

public slots:

private:
    QSharedPointer<ScreepsLocalConfigStore> configStore;

    QMap<QString, QString> inMemoryCache;
};

#endif // SCREEPSTERRAINCACHE_H
