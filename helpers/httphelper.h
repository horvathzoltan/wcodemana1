#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include <QObject>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUuid>

namespace helper
{
class HttpHelper : public QObject
{
    Q_OBJECT
public:
    explicit HttpHelper(QObject *parent = nullptr);

    void SendPost(QUuid id, const QString& source_lang, const QString& dest_lang, const QString& msg);//, QUuid guid);

public slots:
    void onFinish(QNetworkReply *rep);
signals:
    void ResponseOk(QByteArray s, QUuid id);
};
}
#endif // HTTPHELPER_H
