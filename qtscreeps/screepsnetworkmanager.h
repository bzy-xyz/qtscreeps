#ifndef SCREEPSNETWORKMANAGER_H
#define SCREEPSNETWORKMANAGER_H

#include <QObject>
#include <QtCore>
#include <QtWebSockets/QtWebSockets>
#include <QtNetwork>
#include <QtDebug>

#include "screepsrngutils.h"

class ScreepsNetworkManager : public QObject
{
    Q_OBJECT

private:
    QByteArray currentToken;
    QWebSocket websocket;
    QNetworkAccessManager networkAccessManager;

    QNetworkReply * __do_post_call(QByteArray api_path, QJsonDocument args);
    QNetworkReply * __do_get_call(QByteArray api_path, QMap<QString, QString> args);
    void __ws_send(QString s);

    void __ws_process_payload(QJsonValue v);

public:
    explicit ScreepsNetworkManager(QObject *parent = 0);

    bool IsConnected();

signals:
    void DoLoginSuccess();
    void DoLoginFailed();

    void GotCPUMessage(int cpu, int memory);
    void GotConsoleLog(QList<QString> messages);
    void GotConsoleResults(QList<QString> messages);
    void GotConsoleError(QString message);

public slots:
    void DoLogin(QString email, QString password);
    void DoLoginDone();

    void DoWSOpen();
    void DoWSOpenConnected();

    void WSProcessBinaryMessage(const QByteArray & message);
    void WSProcessTextMessage(const QString & message);

};

#endif // SCREEPSNETWORKMANAGER_H
