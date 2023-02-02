#ifndef CONNCALABAZACHILE_H
#define CONNCALABAZACHILE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>


class ConnCalabazaChile
{
public:
    ConnCalabazaChile();
    QString GeckoUser;
    QString GeckoPass;
    QString GeckoDatabasename;
    QString GeckoUrl;
    QSqlDatabase GeckoDb;

    QString user;
    QString pass;
    QString databasemame;
    QString url;
    QSqlDatabase db;

    QString user2;
    QString pass2;
    QString databasemame2;
    QString url2;
    QSqlDatabase db2;
};

#endif // CONNCALABAZACHILE_H
