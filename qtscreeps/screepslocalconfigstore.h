#ifndef SCREEPSCONFIGSTORE_H
#define SCREEPSCONFIGSTORE_H

#include <QtCore>
#include <QtSql>

class ScreepsLocalConfigStore : public QObject
{
    Q_OBJECT

    QSqlDatabase db;

public:
    static ScreepsLocalConfigStore & defaultInstance()
    {
        static ScreepsLocalConfigStore instance;
        return instance;
    }

    explicit ScreepsLocalConfigStore(QObject *parent = 0);

    ScreepsLocalConfigStore(QObject *parent, QString storeName);

    QString get(QString key);
    bool set(QString key, QString value);
    QSqlError lastError();


signals:

public slots:
};

#endif // SCREEPSCONFIGSTORE_H
