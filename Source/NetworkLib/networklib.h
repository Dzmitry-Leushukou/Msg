#ifndef NETWORKLIB_H
#define NETWORKLIB_H

#include "NetworkLib_global.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>


class NETWORKLIB_EXPORT NetworkLib
{
public:
    static bool internet_connection();
};

#endif // NETWORKLIB_H
