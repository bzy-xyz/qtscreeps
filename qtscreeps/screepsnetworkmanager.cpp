#include "screepsnetworkmanager.h"

ScreepsNetworkManager::ScreepsNetworkManager(QObject *parent) : QObject(parent)
{
    random_init();
    connect(&websocket, SIGNAL(connected()), this, SLOT(DoWSOpenConnected()));
    connect(&websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(WSProcessTextMessage(QString)));
    connect(&websocket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(WSProcessBinaryMessage(QByteArray)));
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

void ScreepsNetworkManager::__ws_send(QString s)
{
    if(!IsConnected())
    {
        qDebug() << "tried to do a websocket send without a session!";
    }
    else
    {
        websocket.sendTextMessage("[\"" + s + "\"]");
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
                DoWSOpen();
            }
        }
    }

    r->deleteLater();
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

    // try to auth
    __ws_send("auth " + currentToken);

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
    if(v.isString())
    {
        auto str = v.toString();
        // first try to parse the string as a JSON item
        QJsonParseError jsonErr2;
        QJsonDocument d2 = QJsonDocument::fromJson(str.toUtf8(), &jsonErr2);
        if(jsonErr2.error == QJsonParseError::NoError)
        {
            // this is a JSON object or array, do stuff with it
            qDebug() << "received alien JSON document: " + d2.toJson(QJsonDocument::Compact);
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
                        emit DoLoginSuccess();
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
