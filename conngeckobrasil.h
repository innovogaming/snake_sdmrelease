#ifndef CONNGECKOBRASIL_H
#define CONNGECKOBRASIL_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>


class ConnGeckoBrasil
{
public:
    ConnGeckoBrasil();
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

#endif // CONNGECKOBRASIL_H
