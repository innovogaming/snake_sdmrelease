#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include <QDebug>

MainWindow *newMain = 0;
int ServerSelected = 0;
int UserId = 0;
QString TerminalIp = NULL;
QString VmId = NULL;

///Constructor
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    QPixmap logoSDM(":/pixmap/login/LogoSnakeSmall.png");
    QPixmap logoInnovo(":/pixmap/login/innovoSmall.png");
    QPixmap loginbg(":/pixmap/login/bg10.png");
    QPalette palette;

    this->setWindowIcon(QIcon(":/icons/login/imgs/favicon.ico"));

    loginbg = loginbg.scaled(this->size(), Qt::IgnoreAspectRatio);

    palette.setBrush(QPalette::Background, loginbg);
    this->setPalette(palette);

    ui->SnakeLogoLabel->setPixmap(logoSDM);
    ui->label_LogoInnovo->setPixmap(logoInnovo);

    time_t	rawtime;
    struct tm *t_now;

    time( &rawtime);
    t_now = localtime( &rawtime);

    qDebug("System Date is: %02d/%02d/%04d",t_now->tm_mday, t_now->tm_mon+1, t_now->tm_year+1900);
    qDebug("System Time is: %02d:%02d:%02d",t_now->tm_hour, t_now->tm_min, t_now->tm_sec);

    //string txtVersion = "Version " + to_string( t_now->tm_year+1900 ) + "." + to_string(  t_now->tm_mon+1 ) + "." + to_string( t_now->tm_mday );
    string txtVersion = "Version 2021.07.14";

    ui->label_versionSDM->setText(txtVersion.c_str());

    connect(ui->PasswordLineEdit,SIGNAL(returnPressed()),ui->LoginPushButton,SIGNAL(clicked()));
    connect(ui->LoginPushButton, SIGNAL(clicked()), this, SLOT(openMainWindow()));

    setServers();
    LoginSuccessful = false;

    connect(this, SIGNAL(canceled()), qApp, SLOT(quit()));
    QTimer::singleShot(0,this,SLOT(InitCheck()));
}

///Destructor
login::~login()
{
    delete ui;
}

/**
 * @brief login::InitCheck
 */
void login::InitCheck(){
    if(HardSerialCheck() == false){
        QMessageBox::StandardButton reply = QMessageBox::critical(this, "No Habilitado",
          "El uso de esta maquina no esta habilitado.\n Para mayor informacion contactar a \n soporte@calabazachile.com", QMessageBox::Ok);
        if(reply == QMessageBox::Ok){
            emit canceled();
        }
    }
}

/**
 * @brief login::setServers
 */
void login::setServers(){
    ui->ServerComboBox->clear();
    ui->ServerComboBox->addItem(" ");
    ui->ServerComboBox->addItem("Gecko Chile");
    ui->ServerComboBox->addItem("Gecko Brasil");
    //ui->ServerComboBox->addItem("jabulani");
}

/**
 * @brief login::on_LoginPushButton_clicked
 */
void login::on_LoginPushButton_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    int CurrentServerIndex = ui->ServerComboBox->currentIndex();

    switch (CurrentServerIndex) {
    case 0:
        QMessageBox::warning(this, "Error", "Seleccione un servidor");
        break;
    case 1:{
        ServerSelected = 1;
        if(connCala.GeckoDb.open() == true){
            QString usertext = ui->UserLineEdit->text();
            QString passtext = ui->PasswordLineEdit->text();

            QCryptographicHash cypher(QCryptographicHash::Md5);
            cypher.addData(passtext.toLatin1());
            QByteArray pass_result = cypher.result();
            QString passhex = pass_result.toHex();

            QSqlQuery query(connCala.GeckoDb);
            query.prepare("SELECT * FROM logins WHERE usuario = :user AND senha = :pass" );
            query.bindValue(":user",usertext);
            query.bindValue(":pass",passhex);

            if(query.exec()){
                if(query.next()){
                    UserId = query.value("id_login").toInt();
                    if(AuthCheckGeckoChile(UserId)){
                        LoginSuccessful = true;
                        TerminalIp = GetTerminalIp();
                        VmId = GetHardSerialVm();
                        LogActionCala("User Log in success", UserId, TerminalIp, VmId);
                    }else{
                        QMessageBox::critical(this, "No Autorizado",
                          "El usuario no esta autorizado.\n Para mayor informacion contactar a \n soporte@calabazachile.com", QMessageBox::Ok);
                    }

                }else{
                    QMessageBox::warning(this, "Error Login", "Usuario o contraseña no encontrado");
                }

            }else{
                QString LastQueryError = query.lastError().text();
                dbQueryError(LastQueryError);
            }
        }else{
            QString LastDbError = connCala.GeckoDb.lastError().text();
            dbConnectionError(LastDbError);
        }
        connCala.GeckoDb.close();
        break;
    }
    case 2:{
        ServerSelected = 2;
        if(connGeckoBrasil.GeckoDb.open() == true){
            QString usertext = ui->UserLineEdit->text();
            QString passtext = ui->PasswordLineEdit->text();

            QCryptographicHash cypher(QCryptographicHash::Md5);
            cypher.addData(passtext.toLatin1());
            QByteArray pass_result = cypher.result();
            QString passhex = pass_result.toHex();

            QSqlQuery query(connGeckoBrasil.GeckoDb);
            query.prepare("SELECT * FROM logins WHERE usuario = :user AND senha = :pass" );
            query.bindValue(":user",usertext);
            query.bindValue(":pass",passhex);

            if(query.exec()){
                if(query.next()){
                    UserId = query.value("id_login").toInt();
                    if(AuthCheckGeckoBrasil(UserId)){
                        LoginSuccessful = true;
                        TerminalIp = GetTerminalIp();
                        VmId = GetHardSerialVm();
                        LogActionGeckoBrasil("User Log in success", UserId, TerminalIp, VmId);
                    }else{
                        QMessageBox::critical(this, "No Autorizado",
                          "El usuario no esta autorizado.\n Para mayor informacion contactar a \n soporte@calabazachile.com", QMessageBox::Ok);
                    }

                }else{
                    QMessageBox::warning(this, "Error Login", "Usuario o contraseña no encontrado");
                }

            }else{
                QString LastQueryError = query.lastError().text();
                dbQueryError(LastQueryError);
            }
        }else{
            QString LastDbError = connGeckoBrasil.GeckoDb.lastError().text();
            dbConnectionError(LastDbError);
        }
        connGeckoBrasil.GeckoDb.close();
        break;
    }
    case 3:{
        ServerSelected = 3;
        if(connJabulani.GeckoDb.open() == true){
            QString usertext = ui->UserLineEdit->text();
            QString passtext = ui->PasswordLineEdit->text();

            QCryptographicHash cypher(QCryptographicHash::Md5);
            cypher.addData(passtext.toLatin1());
            QByteArray pass_result = cypher.result();
            QString passhex = pass_result.toHex();

            QSqlQuery query(connJabulani.GeckoDb);
            query.prepare("SELECT * FROM logins WHERE usuario = :user AND senha = :pass" );
            query.bindValue(":user",usertext);
            query.bindValue(":pass",passhex);

            if(query.exec()){
                if(query.next()){
                    UserId = query.value("id_login").toInt();
                    if(AuthCheckGeckoBrasil(UserId)){
                        LoginSuccessful = true;
                        TerminalIp = GetTerminalIp();
                        VmId = GetHardSerialVm();
                        LogActionJabulani("User Log in success", UserId, TerminalIp, VmId);
                    }else{
                        QMessageBox::critical(this, "No Autorizado",
                          "El usuario no esta autorizado.\n Para mayor informacion contactar a \n soporte@calabazachile.com", QMessageBox::Ok);
                    }

                }else{
                    QMessageBox::warning(this, "Error Login", "Usuario o contraseña no encontrado");
                }

            }else{
                QString LastQueryError = query.lastError().text();
                dbQueryError(LastQueryError);
            }
        }else{
            QString LastDbError = connJabulani.GeckoDb.lastError().text();
            dbConnectionError(LastDbError);
        }
        connJabulani.GeckoDb.close();
        break;
    }
    default:
        QMessageBox::warning(this, "Error", "Seleccione un servidor");
        break;
    }

    QApplication::restoreOverrideCursor();
}

/**
 * @brief login::openMainWindow
 */
void login::openMainWindow(){
    if( LoginSuccessful == true ){
        newMain = new MainWindow();
        this->hide();
        newMain->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newMain->size(), qApp->desktop()->availableGeometry()));
        newMain->show();
    }
}

void login::LogActionCala(QString action, int UserId, QString TerminalIp_ , QString VmId_){
    if (connCala.GeckoDb.open() == true){
        QSqlQuery query(connCala.GeckoDb);
        QString act;
        act = "";
        act.append(action);

        query.prepare("INSERT INTO snake_log (user_id, terminal_ip, vm_id, descripcion) VALUES (:userid, :terminalip, :vmid, :desc)");
        query.bindValue(":userid",UserId);
        query.bindValue(":terminalip",TerminalIp_);
        query.bindValue(":vmid",VmId_);
        query.bindValue(":desc",act);
        if(query.exec()){
            if(query.next()){
                qDebug() << "snake_log success";
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connCala.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }
}

void login::LogActionGeckoBrasil(QString action, int UserId, QString TerminalIp_ , QString VmId_){
    if (connGeckoBrasil.GeckoDb.open() == true){
        QSqlQuery query(connGeckoBrasil.GeckoDb);
        QString act;
        act = "";
        act.append(action);

        query.prepare("INSERT INTO snake_log (user_id, terminal_ip, vm_id, descripcion) VALUES (:userid, :terminalip, :vmid, :desc)");
        query.bindValue(":userid",UserId);
        query.bindValue(":terminalip",TerminalIp_);
        query.bindValue(":vmid",VmId_);
        query.bindValue(":desc",act);
        if(query.exec()){
            if(query.next()){
                qDebug() << "snake_log success";
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connGeckoBrasil.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }
}

void login::LogActionJabulani(QString action, int UserId, QString TerminalIp_ , QString VmId_){
    if (connJabulani.GeckoDb.open() == true){
        QSqlQuery query(connJabulani.GeckoDb);
        QString act;
        act = "";
        act.append(action);

        query.prepare("INSERT INTO snake_log (user_id, terminal_ip, vm_id, descripcion) VALUES (:userid, :terminalip, :vmid, :desc)");
        query.bindValue(":userid",UserId);
        query.bindValue(":terminalip",TerminalIp_);
        query.bindValue(":vmid",VmId_);
        query.bindValue(":desc",act);
        if(query.exec()){
            if(query.next()){
                qDebug() << "snake_log success";
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connJabulani.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }
}

std::string login::GetStdOutFromCommand(std::string cmd){
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];

    stream = popen(cmd.std::string::c_str(), "r");
    if (stream) {
        while (!feof(stream)){
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        }
        pclose(stream);
    }
    else{
        QMessageBox::warning(this, "Error", "Problema al ejecutar Sub Proceso");
    }
    return data;
}

QString login::GetTerminalIp(){
    std::string cmdin;
    std::string cmdout;
    QString cmdparsed;
    cmdin = "curl ipecho.net/plain";
    cmdout = GetStdOutFromCommand(cmdin);
    cmdparsed = cmdout.c_str();
    return cmdparsed;
}


bool login::AuthCheckGeckoChile(int idlogin){
    bool isAuth = false;
    QString queryres;
    if (connCala.GeckoDb.open() == true){
        QSqlQuery query(connCala.GeckoDb);
        query.prepare("SELECT * FROM acesso_sub WHERE id_login=:idlogin AND id_menu=1027");
        query.bindValue(":idlogin", idlogin);

        if(query.exec()){
            if(query.next()){
                queryres = query.value("acesso").toString();
                if(queryres == 'S'){
                    isAuth = true;
                }else if(queryres == 'N'){
                    isAuth = false;
                }
            }else{
                isAuth = false;
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connCala.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }

    return isAuth;
}

bool login::AuthCheckGeckoBrasil(int idlogin){
    bool isAuth = false;
    QString queryres;
    if (connGeckoBrasil.GeckoDb.open() == true){
        QSqlQuery query(connGeckoBrasil.GeckoDb);
        query.prepare("SELECT * FROM acesso_sub WHERE id_login=:idlogin AND id_menu=1027");
        query.bindValue(":idlogin", idlogin);

        if(query.exec()){
            if(query.next()){
                queryres = query.value("acesso").toString();
                if(queryres == 'S'){
                    isAuth = true;
                }else if(queryres == 'N'){
                    isAuth = false;
                }
            }else{
                isAuth = false;
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connGeckoBrasil.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }

    return isAuth;
}

bool login::HardSerialCheck(){
    bool isEnabled = false;
    int queryres;
    QString HardSerialVm;
    HardSerialVm = GetHardSerialVm();
    if (connCala.GeckoDb.open() == true){
        QSqlQuery query(connCala.GeckoDb);
        query.prepare("SELECT enabled FROM snake_control WHERE hardserial =:hardserial");
        query.bindValue(":hardserial", HardSerialVm);

        if(query.exec()){
            if(query.next()){
                queryres = query.value("enabled").toInt();
                if(queryres == 1){
                    isEnabled = true;
                }else if(queryres == 0){
                    isEnabled = false;
                }
            }else{
                InsertNewHardSerialVm(HardSerialVm);
                isEnabled = false;
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = connCala.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }

    return isEnabled;
}


void login::InsertNewHardSerialVm(QString hardserial){
    if (connCala.GeckoDb.open() == true){
        QSqlQuery query(connCala.GeckoDb);

        query.prepare("INSERT INTO snake_control (hardserial, enabled, authorized) VALUES (:hardserial, :enabled, :authorized)");
        query.bindValue(":hardserial", hardserial);
        query.bindValue(":enabled", QString("0"));
        query.bindValue(":authorized", QString("0"));
        if(query.exec()){
            if(query.next()){
                qDebug() << "insert successful in snake_control";
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }

    }else{
        QString dblasterr = connCala.GeckoDb.lastError().text();
        dbConnectionError(dblasterr);
    }
}

QString login::GetHardSerialVm(){
    std::string cmdin;
    std::string cmdout;
    QString     cmdparsed;
    cmdin   = "cat /sys/class/net/ens33/address";
    cmdout  = GetStdOutFromCommand(cmdin);
    cmdparsed = cmdout.c_str();
    return cmdparsed;
}

/*********************************************************************************** */
/****************************** error message fuctions **************************** */
/*********************************************************************************** */

/**
 * @brief login::dbConnectionError
 * @param err QString
 */
void login::dbConnectionError(QString err){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(err);
    errorMessageBox.exec();
}

/**
 * @brief login::dbQueryError
 * @param err QString
 */
void login::dbQueryError(QString err){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(err);
    errorMessageBox.exec();
}


