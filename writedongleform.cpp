#include "writedongleform.h"
#include "ui_writedongleform.h"
#include "login.h"

/*************************************************************
*** Este codigo re-utiliza codigo heredado del
*** software SysDongle y se han modificado funciones.
*** Para mayor informacion, revisar codigo fuente SysDongle
***
*** Poner atencion en la seccion de codigo con ADVERTENCIA
*** debido a que se encuentran funciones criticas de la dongle.
*** Si el codigo desarrollado no tiene conexion a una
*** base de datos con los datos correspondientes,
*** puede causar bloqueo de la Dongle debido a que escribe
*** un dato incorrecto o vacio.
***
**************************************************************/

WriteDongleForm::WriteDongleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WriteDongleForm)
{
    ui->setupUi(this);
    //boton deletedongle deshabilitado hasta corregir bug
    ui->button_DeleteDongle->setEnabled(false);

    ProgressBarHide();

    setGameList();
    setValoresPeriodo();
    connect(ui->comboBox_GameName, SIGNAL(currentIndexChanged(int)), SLOT(slot_previousIndex(int)));

    dongleIsFree = true;
    LocalSesion.id = UserId;
    DonglePrintDisabled();
    SetInfoLabel("Seleccione Juego y Version");

}

WriteDongleForm::~WriteDongleForm()
{
    delete ui;
}

void WriteDongleForm::on_button_MakeDongle_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    GameName = ui->comboBox_GameName->currentText();
    GameVer = ui->comboBox_GameVersion->currentText();
    qDebug() << "Iniciando Makedongle2";
    MakeDongleDisabled();
    makedongle();
    MakeDongleEnabled();
    QApplication::restoreOverrideCursor();
    ProgressBarHide();
}

/*********************************************************************************** */
/********************************** Make Dongle function **************************** */
/*********************************************************************************** */


/**
 * @brief WriteDongleForm::makedongle
 */
void WriteDongleForm::makedongle(){
    Dongle::dongle_data_ dongle_local;
    Dongle_Handle_ handle_local;
    Volatile_Data_ local_volatile;
    QString texto;
    QString Dongle_GameName;
    QString ToLogMachineId;
    QString ToLogGameId;
    QString ToLogGameVersion;
    QString ToLogVerPass;
    QString ToLogIdGameVer;
    QString ToLogPeriodo;
    QString ToLogHardSerial;
    bool remake;
    int DiasPeriodo;

    DonglePrintDisabled();
    ProgressBarReset();
    ProgressBarShow();

    dongleIsFree = false;

    if(GameName.isEmpty() || GameVer.isEmpty()){
        QMessageBox::critical(this,"Error","Nombre o Version de Juego vacio");
        return;
    }

    dongle.Detect_Dongle();
    dongle.Detect_Dongle();

    ProgressBarChange(5);
    qDebug() << "debug1 WriteDongle.cpp - start dongle->get_dongle_data";
    dongle_local = dongle.Get_Dongle_Data();

    if(dongle.getDongleConn()){
        SetInfoLabel("Dongle Conectada");
    }else{
        SetWarningLabel("Dongle no conectada");
        QMessageBox::critical(this, "Error Dongle", "Dongle no conectada");
        return;
    }

    SetInfoLabel("Inicio Escritura Dongle");
    ProgressBarChange(10);
    qDebug() << "debug2 WriteDongle.cpp - checking harserial.isEmpty";
    if(dongle_local.hardserial.isEmpty())
    {
        QMessageBox::critical(this,"Error Dongle","Dongle no conectada o no creada, hardserial vacio");
        SetWarningLabel("Error grabado dongle");
        return;
    }
    ProgressBarChange(12);
    qDebug() << "hardserial: " + dongle_local.hardserial;
    qDebug() << "debug3 WriteDongle.cpp3 - checking if hardserial is in DB";
    if(!Check_Dongle_Serial(dongle_local.hardserial))
    {
        qDebug() << "debug3.1 - WriteDongleForm - Dongle NO Registrado...!!!  en check_dongle_serial";
        QMessageBox::critical(this,"Error Dongle","Dongle no encontrada en esta DB");
        SetWarningLabel("Error grabado dongle");
        return;
    }

    ProgressBarChange(15);
    qDebug() << "debug4 WriteDongle.cpp - init of get_dongle_handle";
    handle_local = Get_Dongle_Handle(dongle_local.hardserial);
    qDebug() << "debug4.2 HandlePass:" << handle_local.Pass;
    qDebug() << "debug4.3 HandledataValid:" <<handle_local.data.valid;
    remake = false;

    ProgressBarChange(17);
    if(handle_local.data.valid)
    {
        qDebug() << "debug4.4 WriteDongle.cpp - hable_local.data.valid is true 1";
        texto = QString("Esta Dongle se encuenta actualmente asociada al\njuego: ");
        Dongle_GameName = Get_GameName_by_Id(handle_local.data.GameId);
        texto.append(Dongle_GameName);
        texto.append("\nMachineId: ");
        texto.append(QString::number(handle_local.data.MachineId));
        texto.append("\n¿Desea Rehacer dongle?");

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Atencion",texto,QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::No)
        {
           QMessageBox::information(this,"Rehacer Dongle","Cancelando operacion");
           SetWarningLabel("Operacion Cancelada");
           return;
        }
        if(reply == QMessageBox::Yes)
        {
           QMessageBox::information(this,"Rehacer Dongle","Continuando operacion");
           SetInfoLabel("Continuando Operacion");
        }

        remake = true;
    }

    ProgressBarChange(20);
    SetInfoLabel("Verificando Dongle");

    qDebug() << "debug5 WriteDongle.cpp - init of getGameIdFromGameName";
    gameId = getGameIdFromGameName(GameName);

    ProgressBarChange(23);
    qDebug() << "debug6 WriteDongle.cpp - init Reserve_Volatile";
    if(!Reserve_Volatile(local_volatile))
    {
        qDebug() << "debug7 WriteDongle.cpp - Reserve_Volatile: Dongle not created, no hay chance para crear dongle";
        return;
    }

    ProgressBarChange(25);
    qDebug() << "debug8 WriteDongle.cpp - if dongle.sesion_init";
    if(!dongle.sesion_init(handle_local.Pass))
    {
        //Free_Request_Make_Dongle(LocalSesion.id);
        QMessageBox::warning(this,"Dongle created","Error, super pass doesn't match...");
        return;
    }

    ProgressBarChange(30);
    qDebug() << "debug9 WriteDongle.cpp - dongle.dongle_format";
    if(!dongle.dongle_format())
    {
        //Free_Request_Make_Dongle(LocalSesion.id);
        QMessageBox::information(this,"Dongle not created","La dongle no pudo ser formateada");
        return;
    }

    ProgressBarChange(35);
    SetInfoLabel("Escribiendo Dongle Apps");
    qDebug() << "debug10 WriteDongle.cpp - Escribiendo Dongle Apps";

    int count;
    WriteDongleForm::App_Data_List_ local_app_list;
    Dongle::dongle_file_ local_file;
    WriteDongleForm::App_Data_ local_app;

    qDebug() << "debug11 WriteDongle.cpp - get_apps_list";
    local_app_list = Get_Apps_List(gameId,GameVer);
    count = local_app_list.count;
    qDebug() << "App Count:" << count;

    for(int i=0;i<count;i++){
       local_app.GameId  = local_app_list.GameId[i];
       local_app.GameVer = local_app_list.GameVer[i];
       local_app.AppName = local_app_list.AppName[i];
       local_app.AppId   = local_app_list.AppId[i];

       if(!Get_App(local_app)){
           qDebug() << "debug12 en WriteDongleForm.cpp - if get_app(local_app) false";
           //Free_Request_Make_Dongle(LocalSesion.id);
           QMessageBox::information(this,"Dongle not created","Error some applications can't be found...");
           return;
       }

       local_file.file_name = local_app.AppName;
       local_file.file_id = local_app.AppId.toInt(0,16);
       local_file.file_data = local_app.Data;
       local_file.file_exec = true;
       local_file.file_internal = false;
       qDebug() << local_file.file_name << local_file.file_id;
       qDebug() << "debug13 en WriteDongleForm.cpp - dongle_upload(local_file)";
       dongle.dongle_upload(local_file);
    }

    ProgressBarChange(40);
    if(remake){
        qDebug() << "debug14 en WriteDongleForm.cpp - app 8226 remake true";
        qDebug() << "MachineId de la dongle:" <<handle_local.data.MachineId;
        qDebug() << "Ultimo MachineId asignado en caso de ser primera vez:" << local_volatile.MachineId;
        labelMachineId = QString::number(handle_local.data.MachineId);
        local_file.file_data = QString::number(handle_local.data.MachineId).toLatin1();
        local_file.file_exec = false;
        local_file.file_internal = true;
        local_file.file_name = QString("MaId");
        local_file.file_id = 8226;
        dongle.dongle_upload(local_file);
    }
    else{
        qDebug() << "debug15 en WriteDongleForm.cpp - app 8226 remake false";
        local_volatile.MachineId = local_volatile.MachineId +1;
        labelMachineId = QString::number(local_volatile.MachineId);
        qDebug() << "New MachineId Value:" << local_volatile.MachineId;
        local_file.file_data = QString::number(local_volatile.MachineId).toLatin1();
        local_file.file_exec = false;
        local_file.file_internal = true;
        local_file.file_name = QString("MaId");
        local_file.file_id = 8226;
        dongle.dongle_upload(local_file);
    }

    ProgressBarChange(50);
    SetInfoLabel("Escribiendo PassVersion");

    qDebug() << "debug16 en WriteDongleForm.cpp - app 8224 - Upload ID";
    local_file.file_data = QString::number(gameId).toLatin1();
    local_file.file_exec = false;
    local_file.file_internal = true;
    local_file.file_name = QString("id");
    local_file.file_id = 8224;
    dongle.dongle_upload(local_file);

    ProgressBarChange(55);
    qDebug() << "debug17 en WriteDongleForm.cpp - Get_PassVer_From_GameId_And_GameVer";
    QString VerPass = Get_PassVer_From_GameId_And_GameVer(gameId,GameVer);
    if(VerPass.isEmpty()){
        SetWarningLabel("Pass Version esta vacio");
        return;
    }

    labelGameId = QString::number(gameId);
    labelVersion = GameVer;

    qDebug() << "debug18 en WriteDongleForm.cpp - app 8272 - Upload Pass Version";
    local_file.file_data = VerPass.toLatin1();
    local_file.file_exec = false;
    local_file.file_internal = false;
    local_file.file_name = QString("Version");
    local_file.file_id = 8272;
    dongle.dongle_upload(local_file);

    ProgressBarChange(60);
    SetInfoLabel("Escribiendo GameVersion");

    qDebug() << "debug19 en WriteDongleForm.cpp - app 8274 - Upload GameVersion";
    local_file.file_data = GameVer.toLatin1();
    local_file.file_exec = false;
    local_file.file_internal = false;
    local_file.file_name = QString("GameVer");
    local_file.file_id = 8274;
    dongle.dongle_upload(local_file);

    ProgressBarChange(70);
    SetInfoLabel("Actualizando Street Dongle");

    QByteArray oldpass;
    QByteArray newpass;

    qDebug() << "Starting get_gameversion_id";
    u_int32_t idGameVer = Get_GameVersion_Id(gameId, GameVer);

    DiasPeriodo = getValoresPeriodo(ui->comboBox_Periodo->currentText());
    qDebug() << "Dias de periodo: " << DiasPeriodo;

    //________ADVERTENCIA
    // Verificar conexion a DB.
    // Verificar existencia de versiones de Pass.

    ProgressBarChange(80);
    if(remake){
         qDebug() << "debug20 en WriteDongleForm.cpp - update_street_dongle y register calasys, remake true";
         Update_Street_Dongle(QString::number(handle_local.data.MachineId),gameId,GameVer,VerPass, idGameVer, DiasPeriodo);
         if(!conn.GetNoCalaSys()){
             Register_Calasys(QString::number(handle_local.data.MachineId), QString::number(gameId));
         }
         //Register_Calasys(QString::number(handle_local.data.MachineId), QString::number(gameId));
         newpass = Get_Pass_Version(VerPass);
         oldpass = handle_local.Pass;
         qDebug() << "HandlePass:" << handle_local.Pass;

         ToLogGameId = QString::number(gameId);
         ToLogGameVersion = GameVer;
         ToLogIdGameVer = QString::number(idGameVer);
         ToLogMachineId = QString::number(handle_local.data.MachineId);
         ToLogPeriodo = QString::number(DiasPeriodo);
         ToLogVerPass = VerPass;
         ToLogHardSerial = dongle_local.hardserial;
    }else{
         qDebug() << "debug21 en WriteDongleForm.cpp - store_new_street_dongle";
         Store_New_Street_Dongle(local_volatile.MachineId,gameId,dongle_local.hardserial,GameVer, idGameVer,VerPass);
         if(!conn.GetNoCalaSys()){
             Register_Calasys(QString::number(local_volatile.MachineId), QString::number(gameId));
         }
         //Register_Calasys(QString::number(local_volatile.MachineId), QString::number(gameId));
         newpass = Get_Pass_Version(VerPass);
         oldpass = Get_Pass_Version(QString("V0.0.0"));

         ToLogGameId = QString::number(gameId);
         ToLogGameVersion = GameVer;
         ToLogIdGameVer = QString::number(idGameVer);
         ToLogMachineId = QString::number(local_volatile.MachineId);
         ToLogPeriodo = QString::number(DiasPeriodo);
         ToLogVerPass = VerPass;
         ToLogHardSerial = dongle_local.hardserial;
    }

    ProgressBarChange(95);
    SetInfoLabel("Registrando en calasys");
    qDebug() << " OldPass: " << oldpass;
    qDebug() << " Verpass:" << VerPass << " NewPass:" << newpass;

    if(newpass.isEmpty()){
         return;
    }else{
        if(!dongle.dongle_change_pass(oldpass,newpass)){
            qDebug() << "debug22 en WriteDongleForm.cpp - if !dongle.dongle_change_pass(oldpass,newpass) is false";
            qDebug() << "Password No Cambiada";
            //Free_Request_Make_Dongle(LocalSesion.id);
            QMessageBox::warning(this,"Dongle not created","Password no cambiada...");
            return;
        }else{
            SetInfoLabel("Grabado Finalizado");
            //Free_Request_Make_Dongle(LocalSesion.id);
            QMessageBox::information(this,"Dongle created","Dongle Successfully Created !");

            DonglePrintEnabled();
            QString ActContent;
            ActContent = "Make Dongle Sucess: ";
            ActContent.append("GameId:").append(ToLogGameId);
            ActContent.append(" GameVersion: ").append(ToLogGameVersion);
            ActContent.append(" MachineId: ").append(ToLogMachineId);
            ActContent.append(" Periodo: ").append(ToLogPeriodo);
            ActContent.append(" HardSerial: ").append(ToLogHardSerial);
            LogAction(ActContent, UserId, TerminalIp, VmId);
            GameVersionToPrint = GameVer;
            GameIdToPrint = QString::number(gameId);
            MachineIdToPrint = ToLogMachineId;

        }
    }
    ProgressBarChange(99);
     qDebug() << "debug21 en WriteDongleForm.cpp - process complete \n";
}

std::string WriteDongleForm::GetStdoutFromCommand(std::string cmd) {
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    //cmd.append(" 2>&1");

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

/*********************************************************************************** */
/******************************* Dongle function ************************************ */
/*********************************************************************************** */

/**
 * @brief WriteDongleForm::Check_Dongle_Serial
 *  Busca si el HardSerial esta en la DB
 * @param serial
 * @return bool result
 */
bool WriteDongleForm::Check_Dongle_Serial(QString serial)
{
    bool result;
    if(conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT * FROM DongleStock WHERE HardSerial = :serial");
        query.bindValue(":serial",serial);
        if(query.exec()){
            if(query.next()){
                qDebug() << "debug3.0 en WriteDongleForm.cpp - successfully found hardserial";
                result = true;
            }else{
                qDebug() << "debug3.0 en WriteDongleForm.cpp - not found hardserial";
                result = false;
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            result = false;
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        result = false;
    }

    return result;
}

/**
 * @brief WriteDongleForm::Get_Dongle_Handle
 * @param HardSerial
 * @return Dongle_Handle_ local_data
 */
WriteDongleForm::Dongle_Handle_ WriteDongleForm::Get_Dongle_Handle(QString HardSerial)
{
    QSqlQuery query(conn.dbDongle);
    Dongle_Handle_ local_data;
    if (conn.dbDongle.open() == true){
        qDebug() << "debug4.1 en writedongleform - starting get_dongle_handle";

        query.prepare("SELECT * FROM DongleStock WHERE HardSerial = :HardSerial");
        query.bindValue(":HardSerial",HardSerial);
        //en esta primer query se verifica si el hardserial existe en donglestock
        //y si no existe se asigna la variable local.data.valid = 0
        if(query.exec()){
            if(query.next() && query.size() == 1){
                qDebug() << "Llenando data del Primer Query";
                local_data.HardSerial = query.value("HardSerial").toInt();
                local_data.Supplier = query.value("Supplier").toString();
                local_data.Activa = query.value("Activa").toInt();
                local_data.Operativa = query.value("Operativa").toInt();
                local_data.TimeStamp = query.value("TimeStamp").toString();
                local_data.VerPass = query.value("VerPass").toString();
                local_data.UserId = query.value("UserId").toInt();
                local_data.valid = 1;
             //   qDebug() << local_data;
            }else{
                local_data.valid = 0;
                local_data.data.valid = 0;
                return local_data;
            }
            local_data.Pass = Get_Pass_Version(local_data.VerPass);
        }

        //cuando uno registra una dongle nueva, esta con el campo de activa = 0
        //osea que cuando es nueva solo se deja el campo de local.data.valid = 0
        //mas todos los otros campos de local.data = 0
        if(local_data.Activa){
            query.prepare("SELECT * FROM StreetDongle WHERE MachineId = :Activa");
            query.bindValue(":Activa",local_data.Activa);
            if(query.exec()){
                if(query.next() && query.size() ==1){
                    local_data.data.MachineId = query.value("MachineId").toInt();
                    local_data.data.GameId = query.value("GameId").toInt();
                    local_data.data.CreationDate = query.value("CreationDate").toString();
                    local_data.data.mIn = query.value("mIn").toLongLong();
                    local_data.data.mIn = query.value("mIn").toLongLong();
                    local_data.data.mOut = query.value("mOut").toLongLong();
                    local_data.data.creditIn = query.value("creditIn").toLongLong();
                    local_data.data.creditOut = query.value("creditOut").toLongLong();
                    local_data.data.moneyInWhole = query.value("moneyInWhole").toLongLong();
                    local_data.data.moneyInCents = query.value("moneyInCents").toLongLong();
                    local_data.data.moneyOutWhole = query.value("moneyOutWhole").toLongLong();
                    local_data.data.moneyOutCents = query.value("moneyOutCents").toLongLong();
                    local_data.data.period = query.value("period").toInt();
                    local_data.data.expirationDate = query.value("expirationDate").toString();
                    local_data.data.lastUpdate = query.value("lastUpdate").toString();

                    if(local_data.data.lastUpdate.isEmpty()){
                        local_data.data.lastUpdate = QString("0000-00-00 00:00:00");
                    }

                    if(local_data.data.expirationDate.isEmpty()){
                        local_data.data.expirationDate = QString("0000-00-00 00:00:00");
                    }
                    local_data.data.valid = 1;
                }else{
                    local_data.data.valid = 0;
                }
            }
        }else{
            local_data.data.valid = 0;
        }
    }else{
        local_data.data.valid = 0;
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return local_data;
}

/**
 * @brief WriteDongleForm::Get_GameName_by_Id
 * Obtiene el Nombre del juego segun ID.
 * @param gameId
 * @return QString result
 */
QString WriteDongleForm::Get_GameName_by_Id(int gameId){
    QString result;
    QSqlQuery query(conn.dbDongle);
    if(conn.dbDongle.open() == true){
        query.prepare("SELECT * FROM Games WHERE GameId=:gameid");
        query.bindValue(":gameid",gameId);
        if(query.exec()){
            if(query.next()){
                result = query.value("GameName").toString();
            }else{
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
                result = QString("");
            }
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        result = QString("");
    }
    return result;
}

/**
 * @brief WriteDongleForm::getGameIdFromGameName
 * Obtiene id de juego segun su nombre
 * @param gameName1
 * @return int result
 */
int WriteDongleForm::getGameIdFromGameName(QString gameName1){
    QSqlQuery query(conn.dbDongle);
    int result;
    result = 0;

    if(conn.dbDongle.open() == true){
        query.prepare("SELECT * FROM Games WHERE GameName =:gamename");
        query.bindValue(":gamename",gameName1);
        if(query.exec()){
            qDebug() << "debug5.1 query Size of getgameidfromgamename: " << query.size();
            if(query.next()){
                 result = query.value("GameId").toInt();
            }else{
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return result;
}


/**
 * @brief WriteDongleForm::Reserve_Volatile
 * Funcion para verificar si es que el usuario esta grabando en varias terminales.
 * @param Volatile_Data_ &data
 * @return bool
 */
bool WriteDongleForm::Reserve_Volatile(WriteDongleForm::Volatile_Data_ &data){
    QMessageBox::StandardButton reply;
    QString message;
    WriteDongleForm::Volatile_Data_ local_volatile;
    local_volatile = Status_Request_Make_Dongle();

    if(local_volatile.MakingDongle == QString("Busy")){
        qDebug() << "El sistyema esta siendo usado...";
        if(LocalSesion.id == local_volatile.UserId){
            message = "El usuario logueado esta creando una Dongle en otro terminal.\n Desea Forzar la creacion?";
            reply = QMessageBox::question(0, "Warning Dongle",message,QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes){
                //Free_Request_Make_Dongle(LocalSesion.id);
              //  Reserve_Volatile(data);
            }
            if (reply == QMessageBox::No){
                data = local_volatile;
                return false;
            }
        }
        else{
            qDebug() << "debug else dentro de reserve volatile";
            QMessageBox::information(this,"Dongle not created","Another user is now Making a Dongle, please try again in some seconds...");
            return false;
        }
    }else if(local_volatile.MakingDongle == QString("Free")){
        data = local_volatile;
     //   Request_Make_Dongle(LocalSesion.id);
        return true;
    }
    return false;
}

/**
 * @brief WriteDongleForm::Status_Request_Make_Dongle
 * funcion que busca si hay operaciones ocurriendo al mismo tiempo con el mismo usuario
 * Si no encuentra datos, busca el ultimo dongle grabado
 * Si encuentra datos retorna el timestamp de cuando se grabo
 * @return Volatile_Data_ local_Volatile
 */
WriteDongleForm::Volatile_Data_ WriteDongleForm::Status_Request_Make_Dongle(){
    QSqlQuery query(conn.dbDongle);
    Volatile_Data_ local_Volatile;

    if (conn.dbDongle.open() == true){
        local_Volatile.CreationDate = QString("");
        local_Volatile.err = 0;
        local_Volatile.UserId = 0;
        local_Volatile.MakingDongle = QString("");
        query.prepare("SELECT * FROM Volatile");

        if(query.exec()){
            if(query.next()){
                qDebug() << "hay datos en Query, dongle not free";
                local_Volatile.UserId = query.value("UserId").toInt();
                local_Volatile.CreationDate = query.value("TimeStamp").toString();
                local_Volatile.MakingDongle = query.value("MakingDongle").toString();
                local_Volatile.MachineId = 0;
                local_Volatile.err = 0;
            }
            else{
                qDebug() << "no hay datos en Query, dongle free";
                QSqlQuery query1(conn.dbDongle);
                local_Volatile.err = 0;
                local_Volatile.MakingDongle = QString("Free");
                query1.prepare("SELECT * FROM StreetDongle ORDER BY MachineId DESC LIMIT 1");
                if(query1.exec()){
                    qDebug() << "Ejecutó Query...";
                    if(query1.next()){
                        local_Volatile.MachineId = query1.value("MachineId").toInt();
                    }
                }
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    local_Volatile.err = 1;
    return local_Volatile;
}

// NOT USED
//Funcion usada solo pára liberar el uso del dongle y usuario
/**
 * @brief WriteDongleForm::Free_Request_Make_Dongle
 * Funcion que elimina los datos volatiles del usuario utilizando la dongle
 * para demostrar que desocupo la dongle.
 * @param UserId
 */
void WriteDongleForm::Free_Request_Make_Dongle(int UserId)
{
    QSqlQuery query(conn.dbDongle);

    query.prepare("SELECT * FROM Volatile WHERE UserId = :UserId");
    query.bindValue(":UserId",UserId);

    if(conn.dbDongle.open() == true){
        if(UserId != 0){
            if(query.exec()){
                qDebug() << "Ejecutó query";
                if(query.next()){
                    query.prepare("DELETE FROM Volatile WHERE UserId = :UserId");
                    query.bindValue(":UserId",UserId);
                    if(query.exec()){
                        qDebug() << "Ejecutó query";
                    }
                }
            }else{
                qDebug() << "No Ejecutó query";
            }
        }else{
            query.prepare("DELETE FROM Volatile WHERE 1=1");
            if(query.exec()){
                qDebug() << "Ejecutando Query Force";
            }
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

}

/**
 * @brief WriteDongleForm::Get_Apps_List
 * Funcion que retorna la lista de apps de la dongle.
 * @param GameId
 * @param GameVer
 * @return WriteDongleForm::App_Data_List_ App_local
 */
WriteDongleForm::App_Data_List_ WriteDongleForm::Get_Apps_List(int GameId,QString GameVer){
    WriteDongleForm::App_Data_List_ App_local;
    QSqlQuery query(conn.dbDongle);

    qDebug() << "GameId:"<<GameId << "GameVer:"<< GameVer;

    if(conn.dbDongle.open() == true){
        query.prepare("SELECT GameId,GameVer,AppName,AppId,Tipo,Attr,TimeStamp,UserId FROM AppFiles WHERE GameId = :GameId AND GameVer = :GameVer");
        query.bindValue(":GameId",GameId);
        query.bindValue(":GameVer",GameVer);

        App_local.count =0;
        if(query.exec()){
            App_local.count =query.size();
            while(query.next()){
                App_local.GameId.append(query.value("GameId").toString());
                App_local.GameVer.append(query.value("GameVer").toString());
                App_local.AppName.append(query.value("AppName").toString());
                App_local.AppId.append(query.value("AppId").toString());
                App_local.Type.append(query.value("Tipo").toString());
                App_local.Attr.append(query.value("Attr").toString());
                App_local.TimeStamp.append(query.value("TimeStamp").toString());
                App_local.UserId.append(query.value("UserId").toString());
            }
        }
        return App_local;
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

}

/**
 * @brief WriteDongleForm::Get_App
 * Obtiene el numero de AppDongle.
 * @param App
 * @return
 */
bool WriteDongleForm::Get_App(App_Data_ & App){
    bool res;
    QSqlQuery query(conn.dbDongle);

    if(conn.dbDongle.open() == true){
        query.prepare("SELECT * FROM AppFiles WHERE GameId = :GameId AND GameVer = :GameVer AND AppName = :AppName AND AppId = :AppId");
        query.bindValue(":GameId",App.GameId);
        query.bindValue(":GameVer",App.GameVer);
        query.bindValue(":AppName",App.AppName);
        query.bindValue(":AppId",App.AppId);
        if(query.exec()){
            if(query.next()){
                App.Attr = query.value("Attr").toString();
                App.Data = query.value("Data").toByteArray();
                res = true;
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            res = false;
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        res = false;
    }
    return res;
}

/**
 * @brief WriteDongleForm::Get_PassVer_From_GameId_And_GameVer
 * @param GameId
 * @param GameVer
 * @return QString Verpass
 */
QString WriteDongleForm::Get_PassVer_From_GameId_And_GameVer(int GameId, QString GameVer){
    QSqlQuery query(conn.dbDongle);
    QString   VerPass;

    if(conn.dbDongle.open() == true){
        query.prepare("SELECT * FROM GamesVersions WHERE GameId = :GameId AND GameVersion = :GameVer");
        query.bindValue(":GameId",GameId);
        query.bindValue(":GameVer",GameVer);
        if(query.exec()){
            if(query.next()){
                VerPass = query.value("PassVersion").toString();
            }else{
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
                qDebug() << "Debug No pass founded";
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            qDebug() << "Error query no ejecutado";
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    return VerPass;
}

/**
 * @brief WriteDongleForm::Update_Street_Dongle
 * Actualiza los datos en la DB de la dongle a grabar
 * @param Machine_Id
 * @param GameId
 * @param GameVersion
 * @param VerPass
 * @param idGameVer
 * @param Periodo
 * @return bool
 */
bool WriteDongleForm::Update_Street_Dongle(QString Machine_Id, int GameId, QString GameVersion, QString VerPass , u_int32_t idGameVer, int Periodo){
    bool result;
    QSqlQuery query(conn.dbDongle);
    qDebug() << "MachineId a actualizar:" << Machine_Id;
    qDebug() << "periodo a actualizar:" << Periodo;
    if(conn.dbDongle.open() == true){
        query.prepare("UPDATE StreetDongle SET GameId = :GameId, PassVersion = :passver, mIn = :mIn, mOut = :mOut, creditIn = :creditIn, creditOut = :creditOut, moneyInWhole = :moneyInWhole, moneyInCents = :moneyInCents, moneyOutWhole = :moneyOutWhole, moneyOutCents = :moneyOutCents, period = :period, expirationDate = DATE_SUB(NOW(), INTERVAL -:EXCALC DAY), lastUpdate = NOW(), GameCreation = NOW(), idVersionGame = :idGameVer, GameVersion = :gamever WHERE MachineId = :MachineId");
        query.bindValue(":GameId",GameId);
        query.bindValue(":gamever",GameVersion);
        query.bindValue(":passver",VerPass);
        query.bindValue(":mIn",QString("0"));
        query.bindValue(":mOut",QString("0"));
        query.bindValue(":creditIn",QString("0"));
        query.bindValue(":creditOut",QString("0"));
        query.bindValue(":moneyInWhole",QString("0"));
        query.bindValue(":moneyInCents",QString("0"));
        query.bindValue(":moneyOutWhole",QString("0"));
        query.bindValue(":moneyOutCents",QString("0"));
        query.bindValue(":period",QString::number(Periodo));
        query.bindValue(":EXCALC",QString::number(Periodo));
        query.bindValue(":idGameVer",idGameVer);
        query.bindValue(":MachineId",Machine_Id);

        if(query.exec()){
            qDebug() << "Ejecutó Query Update_Street_Dongle";
            result = true;
        }else{
            qDebug() << "No Ejecutó Query Update_Street_Dongle debido a:" << query.lastError();
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            result = false;
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        result = false;
    }
    return result;
}

/**
 * @brief WriteDongleForm::Register_Calasys
 * Inserta o actualiza en la DB Calasys datos de dongle
 * @param MachineId
 * @param GameId
 * @return bool
 */
bool WriteDongleForm::Register_Calasys(QString MachineId, QString GameId){
    QSqlQuery query(conn.dbSystem);
    bool existe = false;
    bool ReturnValue;

    if(conn.dbSystem.open() == true){
        query.prepare("SELECT * FROM interface WHERE numero = :MachineId");
        query.bindValue(":MachineId",MachineId);
        if(query.exec()){
            if(query.next()){
                qDebug() << "debug 20 - Register calasys: Query encontrada, el machineId ya esta asociado a una interface";
                existe = true;
            }
        }else{
            qDebug() << "debug 20 - Register calasys: No Ejecutó Query Register_Calasys debido a:" << query.lastError();
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }

        if(existe){
            query.prepare("UPDATE interface SET id_jogo = :GameId WHERE numero = :MachineId");
            query.bindValue(":MachineId",MachineId);
            query.bindValue(":GameId",GameId);
            if(query.exec()){
                qDebug() << "Lib_Query: Ejecuto Update Calasys MachineId:" << MachineId << "GameId:" << GameId;
                ReturnValue = true;
            }else{
                qDebug() << "Lib_Query: NO Ejecuto Update Calasys";
                ReturnValue = false;
            }
        }else{
            query.prepare("INSERT INTO interface (`data_inclusao`, `numero`, `id_maquina`, `id_jogo`, `excluido`, `serie`) VALUES( NOW(), :MachineId, :id_maquina, :GameId, :excluido, :serie)");
            query.bindValue(":MachineId",MachineId);
            query.bindValue(":id_maquina",QString("0"));
            query.bindValue(":GameId",GameId);
            query.bindValue(":excluido",QString("N"));
            query.bindValue(":serie",QString("0"));
            if(query.exec()){
                qDebug() << "Lib_Query: Ejecuto Insert Calasys MachineId:" << MachineId << "GameId:" << GameId;
                ReturnValue = true;
            }else{
                qDebug() << "Lib_Query: NO Ejecuto Insert Calasys";
                ReturnValue = false;
            }
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return ReturnValue;
}

/**
 * @brief WriteDongleForm::Store_New_Street_Dongle
 * @param MachineId
 * @param GameId
 * @param HardSerial
 * @param gameVer
 * @param idGameVer
 * @param VerPass
 * @return int error
 */
int WriteDongleForm::Store_New_Street_Dongle(int MachineId, int GameId, QString HardSerial, QString gameVer, u_int32_t idGameVer, QString VerPass){
    QSqlQuery query(conn.dbDongle);
    int err = 0;

    if (conn.dbDongle.open() == true){
        query.prepare("INSERT INTO StreetDongle (MachineId, HardSerial, GameId, period, lastUpdate, CreationDate, GameCreation, idVersionGame, GameVersion, PassVersion, expirationDate, mIn, mOut, creditIn, creditOut, moneyInWhole, moneyInCents, moneyOutWhole, moneyOutCents, userId, adminId) VALUES(:MachineId, :HardSerial,:GameId, :period, NOW(), NOW(), NOW(), :idGameVer, :GameVer, :PassVer, DATE_SUB(NOW(), INTERVAL -90 DAY), :mIn, :mOut, :creditIn, :creditOut, :moneyInWhole, :moneyInCents, :moneyOutWhole, :moneyOutCents, :userId, :adminId)");
        query.bindValue(":MachineId",MachineId);
        query.bindValue(":HardSerial",HardSerial);
        query.bindValue(":GameId",GameId);
        query.bindValue(":idGameVer",idGameVer);
        query.bindValue(":GameVer",gameVer);
        query.bindValue(":PassVer",VerPass);
        query.bindValue(":period",QString("90"));
        query.bindValue(":mIn",QString("0"));
        query.bindValue(":mOut",QString("0"));
        query.bindValue(":creditIn",QString("0"));
        query.bindValue(":creditOut",QString("0"));
        query.bindValue(":moneyInWhole",QString("0"));
        query.bindValue(":moneyInCents",QString("0"));
        query.bindValue(":moneyOutWhole",QString("0"));
        query.bindValue(":moneyOutCents",QString("0"));
        query.bindValue(":userId",QString("0"));
        query.bindValue(":adminId",QString("0"));


        qDebug() << "MachineId:" << MachineId << " GameId:" << GameId << "HardSerial:" << HardSerial;
        if(query.exec()){
            qDebug() << "Guardando Datos en tabla... en streetDongle";
            qDebug() << "Ejecutó Query1..";
        }else{
            qDebug() << " Query exec() error" << query.lastError();
            qDebug() << "NO Ejecutó Query1.. en streetDongle";
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            err = err + 1;
        }

        query.prepare("UPDATE DongleStock SET Activa = :MachineId, VerPass = :VerPass WHERE HardSerial = :HardSerial LIMIT 1");
        query.bindValue(":MachineId",MachineId);
        query.bindValue(":HardSerial",HardSerial);
        query.bindValue(":VerPass",VerPass);

        if(query.exec()){
            qDebug() << "Guardando Datos en tabla... en DongleStock";
            qDebug() << "Ejecutó Query2..";
        }else{
            qDebug() << "NO Ejecutó Query1.. en DongleStock";
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            err = err + 2;
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return err;
}

/**
 * @brief WriteDongleForm::Store_New_Dongle
 * @param hardSerial
 * @param Supplier
 * @param PassVersion
 * @param UserId
 * @return bool
 */
bool WriteDongleForm::Store_New_Dongle(QString hardSerial, QString Supplier, QString PassVersion,int UserId){
    QSqlQuery query(conn.dbDongle);
    if (conn.dbDongle.open() == true){
        query.prepare("INSERT INTO DongleStock (HardSerial,Supplier,Activa,Operativa,TimeStamp,VerPass,UserId) VALUES(:HardSerial,:Supplier,:Activa,:Operativa,NOW(),:VerPass,:UserId) ");
        query.bindValue(":HardSerial",hardSerial);
        query.bindValue(":Supplier",Supplier);
        query.bindValue(":Activa",QString("0"));
        query.bindValue(":Operativa",QString("1"));
        query.bindValue(":VerPass",PassVersion);
        query.bindValue(":UserId",UserId);
        if(query.exec()){
            qDebug() <<  "Ejecutó query";
            return true;
        }else{
            qDebug() <<  "No Ejecutó query";
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            return false;
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}


/**
 * @brief WriteDongleForm::Get_GameVersion_Id
 * Obtiene IDGAMEVER segun Gameversion y gameId
 * @param GameId
 * @param GameVersion
 * @return
 */
u_int32_t WriteDongleForm::Get_GameVersion_Id(int GameId, QString GameVersion){
    QSqlQuery query(conn.dbDongle);
    u_int32_t result = 0;
    if(conn.dbDongle.open() == true){
        query.prepare("SELECT idGameVer FROM GamesVersions WHERE GameVersion =:gameversion AND GameId =:gameid");
        query.bindValue(":gameversion",GameVersion);
        query.bindValue(":gameid",GameId);
        if(query.exec()){
            if(query.next()){
                qDebug() << "Getting IdGame by gameversion";
                result = query.value("idGameVer").toInt();
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    return result;
}


/**
 * @brief WriteDongleForm::Get_Pass_Version
 * Esta funcion busca en la DB la pass segun la version indicada
 * y retorna el dato de la pass de Fabrica.
 * @param PassVersion
 * @return QByteArray res
 */
QByteArray WriteDongleForm::Get_Pass_Version(QString PassVersion){
    QByteArray res;
    QSqlQuery query(conn.dbDongle);
    if(conn.dbDongle.open() == true){
        query.prepare("SELECT PassData FROM PassVersions WHERE PassVersion = :PassVersion");
        query.bindValue(":PassVersion",PassVersion);
        if(query.exec()){
            if(query.next()){
                res = query.value(0).toByteArray();
            }
        }else{
            res = NULL;
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        res = NULL;
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    qDebug() << "PassData: " << res;
    return res;
}

/**
 * @brief WriteDongleForm::get_SupplierPassData
 * @param SupplierName
 * @param PassVersion
 * @return QByteArray PassData
 */
QByteArray WriteDongleForm::get_SupplierPassData(QString SupplierName,QString PassVersion){
    QString PassData;
    QSqlQuery query(conn.dbDongle);
    if(conn.dbDongle.open() == true){
        query.prepare("SELECT * FROM SuppliersPass WHERE PassVersion = :PassVer AND SupplierName = :supplier");
        query.bindValue(":PassVer",PassVersion);
        query.bindValue(":supplier",SupplierName);

        if(query.exec()){
            if(query.next()){
                PassData = query.value("PassData").toString();
            }else{
                qDebug() << "No hay data...";
            }
        }else{
            qDebug() << "No ejecuto Query, error:"<< query.lastError();
        }
    }
    qDebug() << "PassData:" <<PassData;
    return PassData.toLatin1();
}


/* *********************************************************************************************** */

/**
 * @brief WriteDongleForm::setValoresPeriodo
 */
void WriteDongleForm::setValoresPeriodo(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Period");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("Periodo").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_Periodo->clear();
        for(int i = 0; i < result.size(); i++){
            ui->comboBox_Periodo->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}


/**
 * @brief WriteDongleForm::getValoresPeriodo
 * @param Dias
 * @return int
 */
int WriteDongleForm::getValoresPeriodo(QString Dias){
    int res = 0;
    qDebug() << "Periodo Seleccionado: " << Dias;
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT Dias FROM Period WHERE Periodo = :Dias");
        query.bindValue(":Dias",Dias);
        if(query.exec()){
            if(query.next()){
                res = query.value("Dias").toInt();
            }else{
                res = 0;
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
        }else{
            res = 0;
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        res = 0;
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return res;
}


/**
 * @brief WriteDongleForm::on_button_PrintDongle_clicked
 */
void WriteDongleForm::on_button_PrintDongle_clicked()
{
    QString ReturnValuePrintPs;
    std::string DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd.c_str();

    if(printer.WritePs(GameVersionToPrint,GameIdToPrint,MachineIdToPrint,DayMonthYearToPrint) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        return;
    }else{
        ReturnValuePrintPs = printer.PrintPs();
        if (ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Etiqueta Dongle", "Impresion Completa");
        }else{
            QMessageBox::critical(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
}

/**
 * @brief WriteDongleForm::on_button_RegisterDongle_clicked
 */
void WriteDongleForm::on_button_RegisterDongle_clicked(){
    registrardongle = new RegisterDongleDialog(this);
    registrardongle->setModal(true);
    setSupplierListReg();
    connect(registrardongle, SIGNAL(RegisterDongleOk()), this, SLOT(slot_RegisterDongle()));
    connect(registrardongle, SIGNAL(SupplierChanged()), this, SLOT(slot_SupplierSelectedRegister()));

    registrardongle->exec(); 

}

/**
 * @brief WriteDongleForm::on_button_DeleteDongle_clicked
 */
void WriteDongleForm::on_button_DeleteDongle_clicked(){
    deletedongle = new DeleteDongleDialog(this);
    deletedongle->setModal(true);
    setSupplierListDel();
    connect(deletedongle, SIGNAL(DeleteDongleOk()), this, SLOT(slot_DeleteDongle()));
    connect(deletedongle, SIGNAL(SupplierChanged()), this, SLOT(slot_SupplierSelectedDelete()));

    deletedongle->exec();
}

/**
 * @brief WriteDongleForm::slot_RegisterDongle
 * Funcion encargada de registrar la dongle a la DB
 */
void WriteDongleForm::slot_RegisterDongle(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString Supplier;
    QString PassVersion;
    Dongle::dongle_data_ local_data;
    int errors = 0;

    Supplier = registrardongle->getSupplier();
    PassVersion = registrardongle->getPassVersion();

    if(Supplier.isEmpty() || PassVersion.isEmpty()){
        QMessageBox::warning(this,"Registrar Dongle","Proveedor o PassVersion vacio");
    }else{
        qDebug() << "Register Dongle";
        qDebug() << "SupplierName:" << Supplier << " PassVersion:" << PassVersion;
        local_data = dongle.Get_Dongle_Data();

        if(local_data.hardserial.isEmpty()){
            QMessageBox::warning(this,"Registrar Dongle","Dongle no detectada");
            return;
        }

        if(Check_Dongle_Serial(local_data.hardserial)){
            qDebug() << "Dongle ya esta Registrada";
            QMessageBox::warning(this,"Registrar Dongle","Dongle ya esta registrada");
        }else{
            qDebug() << "Dongle no Registrada, continuando";
            QByteArray passFactory;
            passFactory = get_SupplierPassData(Supplier,PassVersion);
            qDebug() << "passFactory:" << passFactory;
            QByteArray newPass = Get_Pass_Version(QString("V0.0.0"));
            qDebug() << "NewPass:" << newPass;

            if(newPass.isEmpty() || passFactory.isEmpty()){
                qDebug() << "Algunas de las claves esta Vacia, no se puede cambiar la clave...";
                errors++;
                return;
            }
            if(!dongle.dongle_change_pass(passFactory,newPass)){
                qDebug() << "Password No Cambiada";
                QMessageBox::critical(this,"Registrar Dongle","Dongle no registrada, SupplierPass no concuerda");
                errors++;
                return;
            }
            if(errors == 0){
                if(Store_New_Dongle(local_data.hardserial,Supplier,"V0.0.0",LocalSesion.id)){
                    QMessageBox::information(this,"Registrar Dongle","Dongle registrada Correctamente");
                    QString ActionContent;
                    ActionContent = "New Dongle Registered with ";
                    ActionContent.append("HardSerial: ").append(local_data.hardserial);
                    LogAction(ActionContent, UserId, TerminalIp, VmId);
                }else{
                    QMessageBox::information(this,"Registrar Dongle","Dongle no registrada en DB, pero el password a cambiado");
                }
            }
        }
    }
    QApplication::restoreOverrideCursor();
}

/**
 * @brief WriteDongleForm::slot_DeleteDongle
 * Funcion encargada de eliminar delr egistro a la dongle
 */
void WriteDongleForm::slot_DeleteDongle(){
    QString Supplier;
    QString PassVersion;
    QByteArray oldPass;
    QByteArray newPass;
    Dongle::dongle_data_ local_data;
    QSqlQuery query(conn.dbDongle);
    int errors = 0;

    Supplier = deletedongle->getSupplier();
    PassVersion = deletedongle->getPassVersion();

    if(Supplier.isEmpty() || PassVersion.isEmpty()){
        QMessageBox::warning(this,"Eliminar Dongle","Proveedor o PassVersion vacio");
    }else{
        if(conn.dbDongle.open() == true){
            qDebug() << "SupplierName:" << Supplier << " PassVersion:" << PassVersion;
            local_data = dongle.Get_Dongle_Data();

            if(local_data.hardserial.isEmpty()){
                qDebug() << "Dongle no detectada";
                QMessageBox::warning(this,"Eliminar Dongle","Dongle no detectada");
                return;
            }

            if(!Check_Dongle_Serial(local_data.hardserial)){
                QMessageBox::warning(this,"Eliminar Dongle","Dongle no registrada");
                qDebug() << "No Registrada";
            }else{
                qDebug() << "Ya Registrada";
                Dongle_Handle_ handle_local;
                handle_local = Get_Dongle_Handle(local_data.hardserial);

                if(handle_local.data.valid){
                    qDebug() << "HandlePass:" << handle_local.Pass;
                    qDebug() << "HandledataValid:" << handle_local.data.valid;
                    oldPass = handle_local.Pass;
                }else{
                    oldPass = Get_Pass_Version(QString("V0.0.0"));
                }

                newPass = get_SupplierPassData(Supplier,PassVersion);
                qDebug() << "oldPass:" << oldPass;
                qDebug() << "newPass:" << newPass;

                if(newPass.isEmpty() || oldPass.isEmpty()){
                    qDebug() << "Algunas de las claves esta Vacia, no se puede cambiar la clave...";
                    QMessageBox::warning(this,"Eliminar Dongle","New/old Pass empty");
                    errors++;
                    return;
                }
                if(!dongle.dongle_change_pass(oldPass,newPass)){
                    qDebug() << "Password No Cambiada";
                    QMessageBox::warning(this,"Eliminar Dongle","Dongle no eliminada, SupplierPass no concuerda");
                    errors++;
                    return;
                }

                /*
                query.prepare("UPDATE DongleStock SET Operativa = :operativa WHERE HardSerial = :HardSerial LIMIT 1");
                query.bindValue(":operativa",0);
                query.bindValue(":HardSerial",local_data.hardserial);
                */
                if(errors == 0){
                    query.prepare("DELETE FROM DongleStock WHERE HardSerial = :HardSerial LIMIT 1");
                    query.bindValue(":HardSerial",local_data.hardserial);

                    if(query.exec()){
                        qDebug() << "Delete Dongle: Borrado de DongleStock ";
                    }else{
                        QString querylasterr = query.lastError().text();
                        dbQueryError(querylasterr);
                        errors++;
                    }
                }
                if(errors == 0){
                    query.prepare("DELETE FROM StreetDongle WHERE HardSerial = :HardSerial LIMIT 1");
                    query.bindValue(":HardSerial",local_data.hardserial);

                    if(query.exec()){
                        qDebug() << "Delete Dongle: Borrado de StreetDongle ";
                        QMessageBox::information(this,"Eliminar Dongle","Dongle Eliminada Correctamente");
                        QString ActionContent;
                        ActionContent = "Deleted Dongle with ";
                        ActionContent.append("HardSerial: ").append(local_data.hardserial);
                        LogAction(ActionContent, UserId, TerminalIp, VmId);
                    }else{
                        QString querylasterr = query.lastError().text();
                        dbQueryError(querylasterr);
                    }
                }
            }
        }else{
            QString dblasterr = conn.dbDongle.lastError().text();
            dbConnectionError(dblasterr);
        }
    }
}

/**
 * @brief WriteDongleForm::setSupplierListReg
 * llena el combobox de los suppliers en ventana Registrar dongle
 */
void WriteDongleForm::setSupplierListReg(){
    QComboBox* cboxsupplier = registrardongle->findChild<QComboBox*>("cbox_Supplier");
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Suppliers");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("SupplierName").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        cboxsupplier->addItem("Escoja un Proveedor");
        for(int i = 0; i < result.size(); i++){
            cboxsupplier->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

}

/**
 * @brief WriteDongleForm::setSupplierListDel
 * llena el combobox de los suppliers en ventana Eliminar dongle
 */
void WriteDongleForm::setSupplierListDel(){
    QComboBox* cboxsupplier = deletedongle->findChild<QComboBox*>("cbox_Supplier");
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Suppliers");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("SupplierName").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        cboxsupplier->addItem("Escoja un Proveedor");
        for(int i = 0; i < result.size(); i++){
            cboxsupplier->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

}

/**
 * @brief WriteDongleForm::slot_SupplierSelectedRegister
 * añade la version de password segun el supplier seleccionado en ventana Registrar Dongle
 */
void WriteDongleForm::slot_SupplierSelectedRegister(){
    QComboBox* cboxPassVersion = registrardongle->findChild<QComboBox*>("cbox_PassVersion");
    registrardongle->setSupplier();
    QString value = registrardongle->getSupplier();
    if(value.contains("Escoja")){

    }else{
        if (conn.dbDongle.open() == true){
            QSqlQuery query(conn.dbDongle);
            QStringList result;
            query.prepare("SELECT * FROM SuppliersPass WHERE SupplierName =:supplier");
            query.bindValue(":supplier", value);
            if(query.exec()){
                while(query.next()){
                    result.append(query.value("PassVersion").toString());
                }
            }else{
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
            cboxPassVersion->clear();
            for(int i = 0; i < result.size(); i++){
                cboxPassVersion->addItem(result[i]);
            }
        }else{
            QString dblasterr = conn.dbDongle.lastError().text();
            dbConnectionError(dblasterr);
        }
    }

}

/**
 * @brief WriteDongleForm::slot_SupplierSelectedDelete
 * añade la version de password segun el supplier seleccionado en ventana Eliminar Dongle
 */
void WriteDongleForm::slot_SupplierSelectedDelete(){
    QComboBox* cboxPassVersion = deletedongle->findChild<QComboBox*>("cbox_PassVersion");
    deletedongle->setSupplier();
    QString value = deletedongle->getSupplier();
    if(value.contains("Escoja")){

    }else{
        if (conn.dbDongle.open() == true){
            QSqlQuery query(conn.dbDongle);
            QStringList result;
            query.prepare("SELECT * FROM SuppliersPass WHERE SupplierName =:supplier");
            query.bindValue(":supplier", value);
            if(query.exec()){
                while(query.next()){
                    result.append(query.value("PassVersion").toString());
                }
            }else{
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
            cboxPassVersion->clear();
            for(int i = 0; i < result.size(); i++){
                cboxPassVersion->addItem(result[i]);
            }
        }else{
            QString dblasterr = conn.dbDongle.lastError().text();
            dbConnectionError(dblasterr);
        }
    }
}

/**
 * @brief WriteDongleForm::LogAction
 * funcion de log en DB
 * @param action
 * @param UserId
 * @param TerminalIp_
 * @param VmId_
 */
void WriteDongleForm::LogAction(QString action, int UserId, QString TerminalIp_, QString VmId_){
    if (conn.dbSystem.open() == true){
        QSqlQuery query(conn.dbSystem);
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
        QString dblasterr = conn.dbSystem.lastError().text();
        dbConnectionError(dblasterr);
    }
}


/*********************************************************************************** */
/****************************** Game and Version fuctions ************************** */
/*********************************************************************************** */

/**
 * @brief WriteDongleForm::setGameList
 * llena el combobox con los nombres de los juegos
 */
void WriteDongleForm::setGameList(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT DISTINCT Games.GameName FROM Games, GamesVersions WHERE Games.GameId = GamesVersions.GameId AND GamesVersions.tipo = 'dongle' AND Games.Status = 'dongle' ");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("GameName").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_GameName->clear();
        for(int i = 0; i < result.size(); i++){
            ui->comboBox_GameName->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}


/**
 * @brief WriteDongleForm::setGameVerList
 * llena el combobox con las versiones del juego seleccionado
 */
void WriteDongleForm::setGameVerList(){
    ui->comboBox_GameVersion->clear();
    QStringList gamevers;
    QString gameNameCurrent;
    gameNameCurrent = ui->comboBox_GameName->currentText();
    gamevers = getGameVerList(gameNameCurrent);
    for(int i=0;i<gamevers.size();i++)
    {
        ui->comboBox_GameVersion->addItem(gamevers[i]);
    }
}


/**
 * @brief WriteDongleForm::getGameVerList
 * obtiene las versiones del juego seleccionado
 * @param gamename qstring
 * @return result2 qstringlist de la lista de juegos
 */
QStringList WriteDongleForm::getGameVerList(QString gamename){
    QStringList result2;
    if (conn.dbDongle.open() == true){
        int gameId;
        gameId = getGameId(gamename);
        QSqlQuery query2(conn.dbDongle);
        query2.prepare("SELECT * FROM GamesVersions WHERE tipo = 'dongle' AND GameId =:gameid");
        query2.bindValue(":gameid",gameId);

        if(query2.exec()){
            while(query2.next()){
                result2.append(query2.value("GameVersion").toString());
            }
        }else{
            QString querylasterr = query2.lastError().text();
            dbQueryError(querylasterr);
        }

    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return result2;
}


/**
 * @brief WriteDongleForm::getGameId
 * obtiene el Id del juego escogido
 * @param gamename
 * @return result1 int id del juego
 */
int WriteDongleForm::getGameId(QString gamename){
    int result1 = 0;
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT GameId FROM Games WHERE GameName =:gamename");
        query.bindValue(":gamename",gamename);

        if(query.exec()){
            if(query.next()){
                 result1 = query.value("GameId").toInt();
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return result1;
}


/**
 * @brief WriteDongleForm::slot_previousIndex
 * identifica el cambio de index
 * @param previousIndex int para comparar los index
 */
void WriteDongleForm::slot_previousIndex(int previousIndex){
    int currentIndex;
    currentIndex = ui->comboBox_GameName->currentIndex();
    if(currentIndex == previousIndex){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        setGameVerList();
        QApplication::restoreOverrideCursor();
    }
}

/*********************************************************************************** */
/****************************** error message fuctions ***************************** */
/*********************************************************************************** */

/**
 * @brief WriteDongleForm::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr qstring
 */
void WriteDongleForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WriteDongleForm::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr qstring
 */
void WriteDongleForm::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WriteDongleForm::MakeDongleDisabled
 */
void WriteDongleForm::MakeDongleDisabled(){
    ui->button_MakeDongle->setEnabled(false);
    ui->comboBox_GameName->setEnabled(false);
    ui->comboBox_GameVersion->setEnabled(false);
    ui->comboBox_Periodo->setEnabled(false);
    //ui->button_DeleteDongle->setEnabled(false);
    ui->button_RegisterDongle->setEnabled(false);
}

/**
 * @brief WriteDongleForm::MakeDongleEnabled
 */
void WriteDongleForm::MakeDongleEnabled(){
    ui->button_MakeDongle->setEnabled(true);
    ui->comboBox_GameName->setEnabled(true);
    ui->comboBox_GameVersion->setEnabled(true);
    ui->comboBox_Periodo->setEnabled(true);
    //ui->button_DeleteDongle->setEnabled(true);
    ui->button_RegisterDongle->setEnabled(true);
}

/**
 * @brief WriteDongleForm::SetWarningLabel
 * @param labelmessage
 */
void WriteDongleForm::SetWarningLabel(QString labelmessage){
    ui->label_DongleConected->setFrameStyle(QFrame::Panel);
    ui->label_DongleConected->setStyleSheet("QLabel { background-color: rgb(255, 69, 69); color: rgb(255, 255, 255); border-color: rgb(170, 0, 0); }");
    ui->label_DongleConected->setText(labelmessage);
}

/**
 * @brief WriteDongleForm::SetInfoLabel
 * @param labelmessage
 */
void WriteDongleForm::SetInfoLabel(QString labelmessage){
    ui->label_DongleConected->setFrameStyle(QFrame::Panel);
    ui->label_DongleConected->setStyleSheet("QLabel { background-color: rgb(255, 255, 255); color: rgb(1, 78, 165 ); border-color: rgb(1, 78, 165 ); }");
    ui->label_DongleConected->setText(labelmessage);
}

/**
 * @brief WriteDongleForm::DonglePrintEnabled
 */
void WriteDongleForm::DonglePrintEnabled(){
    ui->button_PrintDongle->setEnabled(true);
}

/**
 * @brief WriteDongleForm::DonglePrintDisabled
 */
void WriteDongleForm::DonglePrintDisabled(){
    ui->button_PrintDongle->setEnabled(false);
}

void WriteDongleForm::ProgressBarHide(){
    ui->progressBar->hide();
}

void WriteDongleForm::ProgressBarShow(){
    ui->progressBar->show();
}

void WriteDongleForm::ProgressBarChange(int i){
    ui->progressBar->setValue(i);
}

void WriteDongleForm::ProgressBarReset(){
    ui->progressBar->reset();
}


