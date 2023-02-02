#ifndef CONEXIONES_H
#define CONEXIONES_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

class conexiones
{
public:
    conexiones();
    bool GetNoCalaSys();

    bool NoCalaSys;

    QString userDongle;
    QString passDongle;
    QString databasenameDongle;
    QString urlDongle;
    QSqlDatabase dbDongle;

    QString userDongle2;
    QString passDongle2;
    QString databasenameDongle2;
    QString urlDongle2;
    QSqlDatabase dbDongle2;

    QString userSystem;
    QString passSystem;
    QString databasenameSystem;
    QString urlSystem;
    QSqlDatabase dbSystem;
};

#endif // CONEXIONES_H
