#include "conncalabazachile.h"

ConnCalabazaChile::ConnCalabazaChile()
{
    /* test
    user = "root";
    pass = "password";
    databasemame = "dongle_SysDongleV2";
    url = "127.0.0.1";
    db = QSqlDatabase::addDatabase("QMYSQL","CONN_CALACH");
    */

	qDebug() << "conncalabazachile_chile\n";

    GeckoUser           = "gecko";
    GeckoPass           = "S3ZF2csgxXvjIBAMnfbgKUcB4q3JYdzU_";
    GeckoDatabasename   = "calabaza";
    GeckoUrl            = "calasys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
    GeckoDb             = QSqlDatabase::addDatabase("QMYSQL","db_gecko_chile");


    GeckoDb.setUserName(GeckoUser);
    GeckoDb.setPassword(GeckoPass);
    GeckoDb.setDatabaseName(GeckoDatabasename);
    GeckoDb.setHostName(GeckoUrl);

    /***************************************************************************** */

    user                = "sysdongle";
    pass                = "rnKA0YjOfepfqi34N1Ri3OPjKyKXe6Uf_";
    databasemame        = "dongle_SysDongleV2";
    url                 = "calasys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
    db                  = QSqlDatabase::addDatabase("QMYSQL","db_device_chile");

    db.setHostName(url);
    db.setDatabaseName(databasemame);
    db.setUserName(user);
    db.setPassword(pass);

    /***************************************************************************** */

    user2                = "sysdongle";
    pass2                = "rnKA0YjOfepfqi34N1Ri3OPjKyKXe6Uf_";
    databasemame2        = "GeckoBrasil_Dongle";
    url2                 = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.co";
    db2                  = QSqlDatabase::addDatabase("QMYSQL","db_device_brasil");

    db2.setHostName(url2);
    db2.setDatabaseName(databasemame2);
    db2.setUserName(user2);
    db2.setPassword(pass2);

}
