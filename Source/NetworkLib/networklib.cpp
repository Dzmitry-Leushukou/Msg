#include "networklib.h"

bool NetworkLib::internet_connection()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply * reply= manager->get(QNetworkRequest(QUrl("http://www.google.com")));
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool ans = (reply->error() == QNetworkReply::NoError);
    reply->deleteLater();
    delete manager;
    reply=nullptr;
    manager=nullptr;
    return ans;
}
