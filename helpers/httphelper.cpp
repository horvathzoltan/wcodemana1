#include "httphelper.h"
#include "log.h"

#include <QNetworkReply>
#include <QUrlQuery>
#include <QUuid>
#include <QVariant>

namespace helper
{
HttpHelper::HttpHelper(QObject *parent)
    : QObject{parent}
{

}

void HttpHelper::SendPost(QUuid id, const QString& source_lang, const QString& dest_lang, const QString& msg)//, QUuid guid )
{
    QString urltxt = R"(https://api-free.deepl.com/v2/translate?auth_key=1bfdbee3-6605-3752-dd94-3b5ae775f090:fx&text={TEXT}&target_lang={LANG})";
    auto urltxt2 = urltxt.replace("{TEXT}", msg).replace("{LANG}",dest_lang);
    QUrl url = QUrl(urltxt2);

    QByteArray postData;

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);

    QNetworkRequest request(url);

    QObject* o = new QObject();
    o->setProperty("_id", id);
    request.setOriginatingObject(o);

    QUrlQuery params;
    params.addQueryItem("POST", R"(/v2/translate?auth_key=1bfdbee3-6605-3752-dd94-3b5ae775f090:fx> HTTP/1.0)");
    params.addQueryItem("Host:", R"(api-free.deepl.com)");
    params.addQueryItem("User-Agent:", R"(wcode1)");
    params.addQueryItem("Accept:", R"(*/*)");
    params.addQueryItem("Content-Length:", R"(4096)");
    params.addQueryItem("content-type", R"(application/x-www-form-urlencoded)");

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));

    //QHttpMultiPart http;

    //QHttpPart receiptPart;
    //receiptPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\""));
    //receiptPart.setBody(postData);

    //http.append(receiptPart);

    mgr->post(request, params.query().toUtf8());
}

void HttpHelper::onFinish(QNetworkReply *rep)
{
    QByteArray b = rep->readAll();
    QString err = rep->errorString();
    QString str(b);
    zInfo("reply: "+str);
    QUuid id;
    QObject *o = rep->request().originatingObject();
    if(o!=nullptr){
        QVariant v = o->property("_id");
        if(v.isValid()){
            id = v.toUuid();
        }
        delete o;
    }
    emit ResponseOk(b, id);
}
}
