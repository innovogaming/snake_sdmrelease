#include "conngeckobrasil.h"

ConnGeckoBrasil::ConnGeckoBrasil()
{
    /* test
    user = "root";
    pass = "password";
    databasemame = "dongle_SysDongleV2";
    url = "127.0.0.1";
    db = QSqlDatabase::addDatabase("QMYSQL","CONN_GECKBRAS");
    */
    
    qDebug() << "conncalabazachile_brasil\n";

    user                = "jabusys";
    pass                = "cala*999865";
    databasemame        = "GeckoBrasil_Dongle";
    url                 = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
    db                  = QSqlDatabase::addDatabase("QMYSQL","CONN_GECKBRAS");

    db.setHostName(url);
    db.setDatabaseName(databasemame);
    db.setUserName(user);
    db.setPassword(pass);

    /***************************************************************************** */

    GeckoUser           = "jabusys";
    GeckoPass           = "cala*999865";
    GeckoDatabasename   = "GeckoBrasil";
    GeckoUrl            = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
    GeckoDb             = QSqlDatabase::addDatabase("QMYSQL","db_gecko_brasil");


    GeckoDb.setUserName(GeckoUser);
    GeckoDb.setPassword(GeckoPass);
    GeckoDb.setDatabaseName(GeckoDatabasename);
    GeckoDb.setHostName(GeckoUrl);

}
