#include "conexiones.h"
#include "login.h"

conexiones::conexiones()
{
	qDebug() << "Conexiones\n";
    int x = ServerSelected;
    switch (x) {
        //Calabaza Chile
        case 1:{
            /*
            //test
            userSystem = "root";
            passSystem = "password";
            databasenameSystem = "calabaza";
            urlSystem = "127.0.0.1";
            dbSystem = QSqlDatabase::addDatabase("QMYSQL","CALASYS_Conn");

            userDongle = "root";
            passDongle = "password";
            databasenameDongle = "dongle_SysDongleV2";
            urlDongle = "127.0.0.1";
            dbDongle = QSqlDatabase::addDatabase("QMYSQL","DONGLE_Conn");
           */
			qDebug() << "calabaza Chile\n";
            userSystem          = "gecko";
            passSystem          = "S3ZF2csgxXvjIBAMnfbgKUcB4q3JYdzU_";
            databasenameSystem  = "calabaza";
            urlSystem           = "calasys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbSystem            = QSqlDatabase::addDatabase("QMYSQL","CALASYS_Conn");

            userDongle          = "sysdongle";
            passDongle          = "rnKA0YjOfepfqi34N1Ri3OPjKyKXe6Uf_";
            databasenameDongle  = "dongle_SysDongleV2";
            urlDongle           = "calasys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbDongle            = QSqlDatabase::addDatabase("QMYSQL","DONGLECL_Conn");

            userDongle2          = "sysdongle";
            passDongle2          = "SQ8Y3DjluxDh7uNL6PB1CNmtDtjJK7Jo_";
            databasenameDongle2  = "GeckoBrasil_Dongle";
            urlDongle2           = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbDongle2            = QSqlDatabase::addDatabase("QMYSQL","DONGLEBR_Conn");

            dbSystem.setHostName(urlSystem);
            dbSystem.setDatabaseName(databasenameSystem);
            dbSystem.setUserName(userSystem);
            dbSystem.setPassword(passSystem);

            dbDongle.setHostName(urlDongle);
            dbDongle.setDatabaseName(databasenameDongle);
            dbDongle.setUserName(userDongle);
            dbDongle.setPassword(passDongle);

            dbDongle2.setHostName(urlDongle2);
            dbDongle2.setDatabaseName(databasenameDongle2);
            dbDongle2.setUserName(userDongle2);
            dbDongle2.setPassword(passDongle2);
		
            NoCalaSys = false;

            break;
        }
        //Gecko Brasil
        case 2:{
			qDebug() << "Gecko Brasil\n";
            userSystem          = "gecko";
            passSystem          = "qUm0MoJSY3y2pflhKFj3EzvQdfmQkVb3_";
            databasenameSystem  = "GeckoBrasil";
            urlSystem           = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbSystem            = QSqlDatabase::addDatabase("QMYSQL","CALASYS_Conn");

            userDongle          = "sysdongle";
            passDongle          = "SQ8Y3DjluxDh7uNL6PB1CNmtDtjJK7Jo_";
            databasenameDongle  = "GeckoBrasil_Dongle";
            urlDongle           = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbDongle            = QSqlDatabase::addDatabase("QMYSQL","DONGLEBR_Conn");

            userDongle2          = "sysdongle";
            passDongle2          = "rnKA0YjOfepfqi34N1Ri3OPjKyKXe6Uf_";
            databasenameDongle2  = "dongle_SysDongleV2";
            urlDongle2           = "calasys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbDongle2            = QSqlDatabase::addDatabase("QMYSQL","DONGLECL_Conn");

            dbSystem.setHostName(urlSystem);
            dbSystem.setDatabaseName(databasenameSystem);
            dbSystem.setUserName(userSystem);
            dbSystem.setPassword(passSystem);

            dbDongle.setHostName(urlDongle);
            dbDongle.setDatabaseName(databasenameDongle);
            dbDongle.setUserName(userDongle);
            dbDongle.setPassword(passDongle);

            dbDongle2.setHostName(urlDongle2);
            dbDongle2.setDatabaseName(databasenameDongle2);
            dbDongle2.setUserName(userDongle2);
            dbDongle2.setPassword(passDongle2);

            NoCalaSys = false;

            break;
        }
        //Jabulani
        /*case 3:{
			qDebug() << "jabulani Brasil\n";
            userSystem          = "jabusys";
            passSystem          = "cala*999865";
            databasenameSystem  = "GeckoBrasil";
            urlSystem           = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbSystem            = QSqlDatabase::addDatabase("QMYSQL","geckobrasil_Conn");

            userDongle          = "jabusys";
            passDongle          = "cala*999865";
            databasenameDongle  = "dongle_SysDongleV2";
            urlDongle           = "jabusys.cor2yfouvoqs.sa-east-1.rds.amazonaws.com";
            dbDongle            = QSqlDatabase::addDatabase("QMYSQL","DONGLE_Conn");

            dbSystem.setHostName(urlSystem);
            dbSystem.setDatabaseName(databasenameSystem);
            dbSystem.setUserName(userSystem);
            dbSystem.setPassword(passSystem);

            dbDongle.setHostName(urlDongle);
            dbDongle.setDatabaseName(databasenameDongle);
            dbDongle.setUserName(userDongle);
            dbDongle.setPassword(passDongle);        

            NoCalaSys = true;

            break;
        }*/
    }
}

bool conexiones::GetNoCalaSys()
{
    return NoCalaSys;
}
