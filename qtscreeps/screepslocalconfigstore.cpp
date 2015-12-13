#include "screepslocalconfigstore.h"

ScreepsLocalConfigStore::ScreepsLocalConfigStore(QObject *parent)
    : ScreepsLocalConfigStore(parent, "config")
{
}

ScreepsLocalConfigStore::ScreepsLocalConfigStore(QObject *parent, QString storeName) : QObject(parent)
{
    qDebug() << "CONSTRUCT CONFIG STORE";
    // get or make a data location
    QString appdata_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir dir(appdata_path);
    if(!dir.exists())
    {
        dir.mkpath(appdata_path);
    }
    if(!dir.exists("qtscreeps"))
    {
        dir.mkpath("qtscreeps");
    }

    db = QSqlDatabase::addDatabase("QSQLITE", storeName);
    db.setDatabaseName(dir.absoluteFilePath(QString("qtscreeps/%1.db").arg(storeName)));

    if(!db.open())
    {
        qFatal("could not open config database!");
    }

    // set up the db if it's not yet set up
    QSqlQuery q1("CREATE TABLE IF NOT EXISTS config (key PRIMARY KEY, value)", db);
    //QSqlQuery q2("CREATE TABLE IF NOT EXISTS terrainCache (room PRIMARY KEY, terrain)", db);

    // ready!
}

QString ScreepsLocalConfigStore::get(QString key)
{
    // pull in a config value
    QSqlQuery q = QSqlQuery(db);
    q.prepare("SELECT value FROM config WHERE key = :key");
    q.bindValue(":key", key);
    q.exec();

    if(q.next())
    {
        return q.value(0).toString();
    }
    else
    {
        return QString();
    }
}

bool ScreepsLocalConfigStore::set(QString key, QString value)
{
    QSqlQuery q = QSqlQuery(db);
    q.prepare("INSERT OR REPLACE INTO config (key, value) VALUES (:key, :value)");
    q.bindValue(":key", key);
    q.bindValue(":value", value);
    return q.exec();
}

bool ScreepsLocalConfigStore::setBatch(QVariantList keys, QVariantList values)
{
    QSqlQuery q = QSqlQuery(db);
    q.prepare("INSERT OR REPLACE INTO config (key, value) VALUES (:key, :value)");
    q.bindValue(":key", keys);
    q.bindValue(":value", values);
    return q.execBatch();
}

QSqlError ScreepsLocalConfigStore::lastError()
{
    return db.lastError();
}
