#ifndef CONNJABULANI_H
#define CONNJABULANI_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>


class ConnJabulani
{
public:
    ConnJabulani();
    QString user;
    QString pass;
    QString databasemame;
    QString url;
    QSqlDatabase db;

    QString GeckoUser;
    QString GeckoPass;
    QString GeckoDatabasename;
    QString GeckoUrl;
    QSqlDatabase GeckoDb;
};

#endif // CONNJABULANI_H
