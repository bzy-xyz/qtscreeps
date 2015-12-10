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

    QJsonObject me;

    QList<QString> __ws_deferred_sends;
    bool __ws_authed = false;

    QNetworkReply * __do_post_call(QByteArray api_path, QJsonDocument args);
    QNetworkReply * __do_get_call(QByteArray api_path, QMap<QString, QString> args = QMap<QString,QString>());
    void __ws_send(QString s);
    void __ws_really_send(QString s);

    void __ws_process_payload(QJsonValue v);
    void __ws_interpret_json_payload(QJsonValue v);

public:
    static ScreepsNetworkManager & defaultInstance()
    {
        static ScreepsNetworkManager instance;
        return instance;
    }

    explicit ScreepsNetworkManager(QObject *parent = 0);

    bool IsConnected();
    bool IsWSAuthed() {return __ws_authed; }

    QVariant getMeParam(const QString key);

signals:
    void DoLoginSuccess();
    void DoLoginFailed();

    void UserInfoInitialized();
    void UserInfoUpdated();

    void GotCPUMessage(int cpu, int memory);
    void GotConsoleLog(QList<QString> messages);
    void GotConsoleResults(QList<QString> messages);
    void GotConsoleError(QString message);

public slots:
    void DoLogin(QString email, QString password);
    void DoLoginDone();

    void DoGetMyInfo();
    void DoGetMyInfoDone();

    void DoWSOpen();
    void DoWSOpenConnected();

    void WSProcessBinaryMessage(const QByteArray & message);
    void WSProcessTextMessage(const QString & message);

    void DoSubscribe(const QString stream);
    void DoDefaultSubscriptions();

    void WSSendDeferredMessages();

};

#endif // SCREEPSNETWORKMANAGER_H
