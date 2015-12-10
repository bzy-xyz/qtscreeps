#include "screepsnetworkmanager.h"

#include <algorithm>
#include <iterator>

ScreepsNetworkManager::ScreepsNetworkManager(QObject *parent) : QObject(parent)
{
    random_init();

    // websocket connections
    connect(&websocket, SIGNAL(connected()), this, SLOT(DoWSOpenConnected()));
    connect(&websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(WSProcessTextMessage(QString)));
    connect(&websocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(WSProcessBinaryMessage(QByteArray)));

    // bootstrap connections
    connect(this, SIGNAL(UserInfoInitialized()), this, SLOT(DoDefaultSubscriptions()));
}

bool ScreepsNetworkManager::IsConnected()
{
    return currentToken.length() && websocket.state() == QAbstractSocket::ConnectedState;
}

QNetworkReply * ScreepsNetworkManager::__do_post_call(QByteArray api_path, QJsonDocument args)
{

    // build the network request
    QNetworkRequest r = QNetworkRequest(QUrl("https://screeps.com/api/" + QString(api_path)));
    r.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");

    // set the auth token if we have one
    if(currentToken.length())
    {
        r.setRawHeader("X-Token", currentToken);
        r.setRawHeader("X-Username", currentToken);
    }

    return networkAccessManager.post(r, args.toJson(QJsonDocument::Compact));

}

QNetworkReply * ScreepsNetworkManager::__do_get_call(QByteArray api_path, QMap<QString, QString> args)
{

    // build the url query
    QUrlQuery q;
    for(auto i = args.begin(); i != args.end(); i++)
    {
        q.addQueryItem(i.key(), i.value());
    }

    // build the url
    QUrl u = QUrl("https://screeps.com/api/" + QString(api_path));
    u.setQuery(q);

    // build and send the get request
    QNetworkRequest r = QNetworkRequest(u);

    if(currentToken.length())
    {
        r.setRawHeader("X-Token", currentToken);
        r.setRawHeader("X-Username", currentToken);
    }

    return networkAccessManager.get(r);
}

void ScreepsNetworkManager::__ws_really_send(QString s)
{
    if(!IsConnected())
    {
        qDebug() << "tried to do a websocket send without a session!";
    }
    else
    {
        qDebug() << "sending " << s;
        websocket.sendTextMessage("[\"" + s + "\"]");
    }
}

void ScreepsNetworkManager::__ws_send(QString s)
{
    if(!IsWSAuthed())
    {
        qDebug() << "queueing deferred send " << s;
        __ws_deferred_sends.push_back(s);
    }
    else
    {
        WSSendDeferredMessages();
        __ws_really_send(s);
    }
}

void ScreepsNetworkManager::WSSendDeferredMessages()
{
    while(!__ws_deferred_sends.isEmpty())
    {
        QString t = __ws_deferred_sends.takeFirst();
        __ws_really_send(t);
    }
}

void ScreepsNetworkManager::DoLogin(QString email, QString password)
{
    QJsonObject o {
        {"email", email},
        {"password", password}
    };
    QNetworkReply * r = __do_post_call("auth/signin", QJsonDocument(o));

    connect(r, SIGNAL(finished()), this, SLOT(DoLoginDone()));
}

void ScreepsNetworkManager::DoLoginDone()
{
    QNetworkReply * r = (QNetworkReply*) QObject::sender();

    if (r->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error trying to login: " << r->errorString();
        emit DoLoginFailed();
    }
    else
    {
        QByteArray z = r->readAll();
        QJsonParseError jsonErr;
        QJsonDocument d = QJsonDocument::fromJson(z, &jsonErr);

        if(jsonErr.error != QJsonParseError::NoError)
        {
            qDebug() << "Error trying to parse login reply: " << jsonErr.errorString();
            emit DoLoginFailed();
        }
        else if(!d.isObject())
        {
            qDebug() << "Error: login reply is not a JSON object!";
            emit DoLoginFailed();
        }
        else
        {
            const QJsonObject o = d.object();
            if(!o.contains("ok") || !o.contains("token"))
            {
                qDebug() << "Error: malformed login reply: " << d.toJson(QJsonDocument::Compact);
                emit DoLoginFailed();
            }
            else
            {
                currentToken = o["token"].toString().toUtf8();
                DoGetMyInfo();
                DoWSOpen();
            }
        }
    }

    r->deleteLater();
}

void ScreepsNetworkManager::DoGetMyInfo()
{
    QNetworkReply * r = __do_get_call("auth/me", QMap<QString,QString>());
    connect(r, SIGNAL(finished()), this, SLOT(DoGetMyInfoDone()));
}

void ScreepsNetworkManager::DoGetMyInfoDone()
{
    QNetworkReply * r = (QNetworkReply*) QObject::sender();

    if (r->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error trying to get user info: " << r->errorString();
    }
    else
    {
        QByteArray z = r->readAll();
        QJsonParseError jsonErr;
        QJsonDocument d = QJsonDocument::fromJson(z, &jsonErr);

        if(jsonErr.error != QJsonParseError::NoError)
        {
            qDebug() << "Error trying to parse auth/me reply: " << jsonErr.errorString();
        }
        else if(!d.isObject())
        {
            qDebug() << "Error: auth/me reply is not a JSON object (instead '" << z << "')";
        }
        else
        {
            const QJsonObject o = d.object();
            if(!o.contains("ok"))
            {
                qDebug() << "Error: malformed auth/me reply: " << d.toJson(QJsonDocument::Compact);
            }
            else
            {
                bool wasUninitialized = me.isEmpty();
                me = o;
                if(wasUninitialized)
                {
                    emit UserInfoInitialized();
                }
                emit UserInfoUpdated();
            }
        }
    }

    r->deleteLater();
}

QVariant ScreepsNetworkManager::getMeParam(const QString key)
{
    return me.contains(key) ? me[key].toVariant() : QVariant();
}

/**
 * @brief Start the process of opening a websocket (iff we have a token)
 */
void ScreepsNetworkManager::DoWSOpen()
{
    if(!currentToken.length())
    {
        qDebug() << "Error opening WS: no token (must call DoLogin())";
        return;
    }

    qint32 sessionId = random_int();
    QByteArray sessionLabel = "qtscreeps" + random_bytes(4).toHex();

    QUrl u = QUrl("wss://screeps.com/socket/" + QString::number(sessionId) + "/" + QString(sessionLabel) + "/websocket");

    websocket.open(u);
}

/**
 * @brief Continue the process of opening a websocket
 */
void ScreepsNetworkManager::DoWSOpenConnected()
{
    if(!currentToken.length())
    {
        qDebug() << "Error opening WS: no token (did something erase it? did you run DoWSOpen()?)";
        emit DoLoginFailed();
        return;
    }

    // try to auth (bypassing the send defer mechanism because we're special)
    __ws_really_send("auth " + currentToken);

}

void ScreepsNetworkManager::WSProcessTextMessage(const QString &message)
{
    // filter on the message type
    QChar type = message[0];
    QString rest = message.mid(1);

    QJsonParseError jsonErr;

    if(type == QChar('a'))
    {
        QJsonDocument d = QJsonDocument::fromJson(rest.toUtf8(), &jsonErr);
        if(jsonErr.error != QJsonParseError::NoError)
        {
            qDebug() << "Error trying to parse bundle: " << jsonErr.errorString();
        }
        else if(!d.isArray())
        {
            qDebug() << "Error: login reply is not a JSON array!";
        }
        else
        {
            QJsonArray bundle = d.array();
            foreach(auto i, bundle)
            {
                __ws_process_payload(i);
            }
        }
    }
}

void ScreepsNetworkManager::WSProcessBinaryMessage(const QByteArray &message)
{
    qDebug() << "got alien binary message " << message;
}


void ScreepsNetworkManager::__ws_process_payload(QJsonValue v)
{
    qDebug() << "got payload " << v;
    if(v.isString())
    {
        auto str = v.toString();
        // first try to parse the string as a JSON item
        QJsonParseError jsonErr2;
        QJsonDocument d2 = QJsonDocument::fromJson(str.toUtf8(), &jsonErr2);
        if(jsonErr2.error == QJsonParseError::NoError)
        {
            // this is a JSON object or array, do stuff with it
            __ws_interpret_json_payload(QJsonValue::fromVariant(d2.toVariant()));
            //qDebug() << "received alien JSON document: " + d2.toJson(QJsonDocument::Compact);
        }
        else if(str.left(2) == "gz")
        {
            // ok, this is a gzip-compressed object
            qDebug() << "gzip decompression not yet implemented";
        }
        else
        {
            // ok, interpret the string as a command string of some sort
            QVector<QString> strTokens = QVector<QString>::fromList(str.split(" "));
            if(strTokens.length())
            {
                // we got some tokens
                if(strTokens[0] == "auth")
                {
                    // this is an auth message
                    if(strTokens[1] == "ok")
                    {
                        currentToken = strTokens[2].toUtf8();
                        __ws_authed = true;
                        WSSendDeferredMessages();
                        emit DoLoginSuccess();
                        // TODO testing

                    }
                    else
                    {
                        currentToken = "";
                        emit DoLoginFailed();
                    }
                }
                else
                {
                    qDebug() << "received alien message: " + str;
                }
            }
        }
    }
}

void ScreepsNetworkManager::__ws_interpret_json_payload(QJsonValue v)
{
    // got a json payload, need to interpret it properly
    qDebug() << "PAYLOAD " << v;

    // if this is an array we're treating it as a publish for now
    if(v.isArray())
    {
        // stream names are of the format type:paramater
        QString streamName = v.toArray()[0].toString();
        auto streamNameTokens = QVector<QString>::fromList(streamName.split(QChar(':')));
        if(streamNameTokens.length())
        {
            // react by stream type
            // user property?
            if(streamNameTokens[0] == "user")
            {
                // interpret the parameter path
                auto userParamTokens = QVector<QString>::fromList(streamNameTokens[1].split(QChar('/')));
                // react by parameter type (since our player id seems always the same for now...)
                if(userParamTokens[1] == "cpu")
                {
                    // this is a CPU message
                    QJsonObject cpuData = v.toArray()[1].toObject();
                    emit GotCPUMessage(cpuData["cpu"].toInt(), cpuData["memory"].toInt());
                }
                else if(userParamTokens[1] == "console")
                {
                    // this is console data
                    QJsonObject consoleData = v.toArray()[1].toObject();
                    if(consoleData.contains("messages"))
                    {
                        // payload includes messages
                        QJsonObject messagesData = consoleData["messages"].toObject();
                        if(messagesData.contains("log"))
                        {
                            QJsonArray logMessages = messagesData["log"].toArray();
                            // convert to QList<QString> for convenience
                            // (forget about nonstring elems, should not have them anyway)
                            QList<QString> logStrings;
                            std::transform(logMessages.begin(), logMessages.end(), std::back_inserter(logStrings), [](QJsonValueRef s) {
                                return s.toString();
                            });
                            // then emit log signal
                            emit GotConsoleLog(logStrings);
                        }
                        if(messagesData.contains("results"))
                        {
                            QJsonArray logResults = messagesData["log"].toArray();
                            // convert to QList<QString> for convenience
                            // (forget about nonstring elems, should not have them anyway)
                            QList<QString> logResultStrings;
                            std::transform(logResults.begin(), logResults.end(), std::back_inserter(logResultStrings), [](QJsonValueRef s) {
                                return s.toString();
                            });
                            // then emit results signal
                            emit GotConsoleLog(logResultStrings);
                        }
                    }
                    if(consoleData.contains("error"))
                    {
                        // payload includes an error string
                        emit GotConsoleError(consoleData["error"].toString());
                    }
                }
            }
            // minimap overlay?
            else if(streamNameTokens[0] == "roomMap2")
            {

            }
            // room details?
            else if(streamNameTokens[0] == "room")
            {

            }
        }
    }
}

void ScreepsNetworkManager::DoSubscribe(const QString stream)
{
    QString s = QString("subscribe %1").arg(stream);
    qDebug() << s;
    __ws_send(s);
}

void ScreepsNetworkManager::DoDefaultSubscriptions()
{
    // this method needs a valid me object
    if(!me.isEmpty())
    {
        DoSubscribe(QString("user:%1/cpu").arg(me["_id"].toString()));
        DoSubscribe(QString("user:%1/console").arg(me["_id"].toString()));
    }
}
