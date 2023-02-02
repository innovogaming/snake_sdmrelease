#include "writegameform.h"
#include "ui_writegameform.h"
#include "login.h"

WriteGameForm::WriteGameForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WriteGameForm)
{
    ui->setupUi(this);

    setGameList();
    setDrives();
    connect(ui->comboBox_GameName, SIGNAL(currentIndexChanged(int)), SLOT(slot_previousIndex(int)));
    connect(ui->comboBox_GameVersion, SIGNAL(currentIndexChanged(int)), SLOT(slot_versionChange()));
    ProgressBarHide();
    ui->button_PrintGameLabel->setEnabled(false);
}


WriteGameForm::~WriteGameForm()
{
    delete ui;
}

/**
 * @brief WriteGameForm::on_button_Makeiso_clicked
 */
void WriteGameForm::on_button_Makeiso_clicked()
{
    ui->button_PrintGameLabel->setEnabled(false);
    ProgressBarReset();
    ProgressBarShow();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::string command1;
    std::string output1;
    std::string str1, str2, str3;

    GameName = ui->comboBox_GameName->currentText();
    GameVer = ui->comboBox_GameVersion->currentText();
    GamePath = "";

///crear el path
    if (conn.dbDongle.open() == true){
        QSqlQuery query1(conn.dbDongle);

        //game id
        int GameId = getGameId(GameName);

        //base path
        query1.prepare("SELECT GamePath FROM GamesIsoPath WHERE GameVersion = 'Base'");
        if(query1.exec()){
            while(query1.next()){
                BasePath = query1.value("GamePath").toString();
            }
        }else{
            QString querylasterr = query1.lastError().text();
            dbQueryError(querylasterr);
        }

        //game path
        QSqlQuery query3(conn.dbDongle);
        query3.prepare("SELECT GamePath FROM GamesIsoPath WHERE GameId = :GameId AND GameVersion = :GameVersion");
        query3.bindValue(":GameId",GameId);
        query3.bindValue(":GameVersion",GameVer);

        if(query3.exec()){
            while(query3.next()){
                ImagePath = query3.value("GamePath").toString();
            }
        }else{
            QString querylasterr = query3.lastError().text();
            dbQueryError(querylasterr);
        }

        if(ImagePath != ""){
            GamePath.append(BasePath).append(ImagePath);
        }else{
            QMessageBox::warning(this, "Error", "El campo de Nombre o Version de Juego es vacio");
        }

    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    conn.dbDongle.close();

    ProgressBarChange(25);
    DriveSelected = getDrives();

    //If you want to ask for User password for root privileges before executing the command use the next line
    //str1 = "pkexec dd if=";
    //If you want to execute the command without asking, use the next line
    str1 = "echo \"innovo2018\" | sudo -S -k dd if=";
    str2 = " of=/dev/";
    str3 = " bs=1M  2>&1 && sync && sync";


    if(GamePath != "" && DriveSelected != ""){
        makeIsoDisabled();
        command1.append(str1);                          // sudo dd if=
        command1.append(GamePath.toStdString());        // /sysdongle/...
        command1.append(str2);                          // of=/dev/
        command1.append(DriveSelected.toStdString());   // sd*
        command1.append(str3);                          // bs...

        ProgressBarChange(40);
        output1 = GetStdoutFromCommand(command1);

        sleep(1);
        ProgressBarChange(70);
        QString cmdexecuted = output1.c_str();

        if(cmdexecuted.contains("No se")){
            QMessageBox::warning(this, "Error", "El Archivo a grabar no se encuentra");
        }
        if(cmdexecuted.contains("copied")){
            QMessageBox::information(this, "Make Game Iso", "Operacion finalizada");
            QString ActionContent;
            ActionContent = "Make of Game Iso success for ";
            ActionContent.append("Game: ").append(GameName);
            ActionContent.append(" Version: ").append(GameVer);
            LogAction(ActionContent,UserId, TerminalIp, VmId);
            ProgressBarChange(99);
            ui->button_PrintGameLabel->setEnabled(true);
            GameNameToPrint = GameName;
            GameVersionToPrint = GameVer;
        }
    }
    else{
        QMessageBox::warning(this, "Error", "El campo del Juego o Drive esta vacio");
    }

    ProgressBarHide();
    QApplication::restoreOverrideCursor();
    makeIsoEnabled();
}

/**
 * @brief WriteGameForm::LogAction
 * @param action
 * @param UserId
 * @param TerminalIp_
 * @param VmId_
 */
void WriteGameForm::LogAction(QString action, int UserId, QString TerminalIp_, QString VmId_){
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
/****************************** get and set fuctions ******************************* */
/*********************************************************************************** */


/**
 * @brief WriteGameForm::setGameList
 * llena el combobox con los nombres de los juegos
 */
void WriteGameForm::setGameList(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.setForwardOnly(true);
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
 * @brief WriteGameForm::setGameVerList
 * llena el combobox con las versiones del juego seleccionado
 */
void WriteGameForm::setGameVerList(){
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
 * @brief WriteGameForm::getGameVerList
 * obtiene las versiones del juego seleccionado
 * @param gamename
 * @return
 */
QStringList WriteGameForm::getGameVerList(QString gamename){
    QStringList result2;
    if (conn.dbDongle.open() == true){
        int gameId;
        gameId = getGameId(gamename);
        QSqlQuery query2(conn.dbDongle);
        query2.setForwardOnly(true);
        query2.prepare("SELECT * FROM GamesVersions WHERE GameId =:gameid AND tipo = 'dongle'");
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
 * @brief WriteGameForm::getGameId
 * obtiene el Id del juego escogido
 * @param gamename
 * @return
 */
int WriteGameForm::getGameId(QString gamename){
    int result1 = 0;
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.setForwardOnly(true);
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
 * @brief WriteGameForm::slot_previousIndex
 * identifica el cambio de index
 * @param previousIndex
 */
void WriteGameForm::slot_previousIndex(int previousIndex){
    int currentIndex;
    currentIndex = ui->comboBox_GameName->currentIndex();

    if(currentIndex == previousIndex){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        setGameVerList();
        setDescription();
        QApplication::restoreOverrideCursor();
    }
}

void WriteGameForm::slot_versionChange(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setDescription();
    QApplication::restoreOverrideCursor();
}


/**
 * @brief WriteGameForm::setDrives
 * seteo de drives
 */
void WriteGameForm::setDrives(){
    ui->comboBox_Drives->clear();

    QString checksdb = GetStdoutFromCommand("lsblk /dev/sdb").c_str();
    QString checksdc = GetStdoutFromCommand("lsblk /dev/sdc").c_str();
    QString checksdd = GetStdoutFromCommand("lsblk /dev/sdd").c_str();
    QString checksde = GetStdoutFromCommand("lsblk /dev/sde").c_str();
    QString checksdf = GetStdoutFromCommand("lsblk /dev/sdf").c_str();

    if((checksdb.contains("sdb")) && (checksdb.contains("16M"))){
        //its a dongle, dont add it
    }else if((checksdb.contains("sdb")) && (checksdb.contains("sdb1"))){
        //its a new pendrive
        ui->comboBox_Drives->addItem("sdb");
    }else if((checksdb.contains("sdb")) && (checksdb.contains("sdb1")) && (checksdb.contains("sdb2")) && (checksdb.contains("sdb3")) && (checksdb.contains("sdb4")) ) {
        //its a already-written pendrive
        ui->comboBox_Drives->addItem("sdb");
    }

    if((checksdc.contains("sdc")) && (checksdc.contains("16M"))){
        //its a dongle, dont add it
    }else if((checksdc.contains("sdc")) && (checksdc.contains("sdc1"))){
        //its a new pendrive
        ui->comboBox_Drives->addItem("sdc");
    }else if((checksdc.contains("sdc")) && (checksdc.contains("sdc1")) && (checksdc.contains("sdc2")) && (checksdc.contains("sdc3")) && (checksdc.contains("sdc4")) ) {
        //its a already-written pendrive
        ui->comboBox_Drives->addItem("sdc");
    }

    if((checksdd.contains("sdd")) && (checksdd.contains("16M"))){
        //its a dongle, dont add it
    }else if((checksdd.contains("sdd")) && (checksdd.contains("sdd1"))){
        //its a new pendrive
        ui->comboBox_Drives->addItem("sdd");
    }else if((checksdd.contains("sdd")) && (checksdd.contains("sdd1")) && (checksdd.contains("sdd2")) && (checksdd.contains("sdd3")) && (checksdd.contains("sdd4")) ) {
        //its a already-written pendrive
        ui->comboBox_Drives->addItem("sdd");
    }

    if((checksde.contains("sde")) && (checksde.contains("16M"))){
        //its a dongle, dont add it
    }else if((checksde.contains("sde")) && (checksde.contains("sde1"))){
        //its a new pendrive
        ui->comboBox_Drives->addItem("sde");
    }else if((checksde.contains("sde")) && (checksde.contains("sde1")) && (checksde.contains("sde2")) && (checksde.contains("sde3")) && (checksde.contains("sde4")) ) {
        //its a already-written pendrive
        ui->comboBox_Drives->addItem("sde");
    }

    if((checksdf.contains("sdf")) && (checksdf.contains("16M"))){
        //its a dongle, dont add it
    }else if((checksdf.contains("sdf")) && (checksdf.contains("sdf1"))){
        //its a new pendrive
        ui->comboBox_Drives->addItem("sdf");
    }else if((checksdf.contains("sdf")) && (checksdf.contains("sdf1")) && (checksdf.contains("sdf2")) && (checksdf.contains("sdf3")) && (checksdf.contains("sdf4")) ) {
        //its a already-written pendrive
        ui->comboBox_Drives->addItem("sdf");
    }
}


/**
 * @brief WriteGameForm::GetStdoutFromCommand
 * ejecuta un comando y obtiene su output en string
 * @param cmd
 * @return
 */
std::string WriteGameForm::GetStdoutFromCommand(std::string cmd) {
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

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


/**
 * @brief WriteGameForm::executeCommandNoReturn
 * ejecuta un comando sin retorno de output
 * @param cmd
 */
void WriteGameForm::executeCommandNoReturn(std::string cmd){
    FILE * stream;
    stream = popen(cmd.std::string::c_str(), "r");
    pclose(stream);
}


/**
 * @brief WriteGameForm::getDrives
 * obtiene el drive seleccionado
 * @return
 */
QString WriteGameForm::getDrives(){
    QString driveSel;
    driveSel = ui->comboBox_Drives->currentText();
    return driveSel;
}


void WriteGameForm::setDescription(){
    QString     result;
    QString     gamename_;
    QString     gamever_;
    int         gameid_;
    gamename_   = ui->comboBox_GameName->currentText();
    gamever_    = ui->comboBox_GameVersion->currentText();
    gameid_     = getGameId(gamename_);

    ui->Description_textEdit->clear();

    if(conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.setForwardOnly(true);
        query.prepare("SELECT Descripcion FROM GamesVersions WHERE GameId =:gameid AND GameVersion =:gamever AND tipo = 'dongle' ");
        query.bindValue(":gameid", gameid_);
        query.bindValue(":gamever", gamever_);
        if(query.exec()){
            if(query.next()){
                result = query.value("Descripcion").toString();
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->Description_textEdit->setText(result);
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}


/**
 * @brief WriteGameForm::on_button_UpdateDrive_clicked
 */
void WriteGameForm::on_button_UpdateDrive_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setDrives();
    QApplication::restoreOverrideCursor();
}

void WriteGameForm::on_button_PrintGameLabel_clicked()
{
    QString ReturnValuePrintPs;
    //QString MachineIdToPrint = "";
    std::string DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd.c_str();

    if(printer.WritePsGame(GameNameToPrint, GameVersionToPrint,DayMonthYearToPrint) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        return;
    }else{
        ReturnValuePrintPs = printer.PrintPs();
        if(ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Label Game", "Impresion Completa");
        }else{
            QMessageBox::warning(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
}

/*********************************************************************************** */
/****************************** error message fuctions **************************** */
/*********************************************************************************** */

/**
 * @brief WriteGameForm::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WriteGameForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WriteGameForm::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WriteGameForm::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WriteGameForm::makeIsoDisabled
 */
void WriteGameForm::makeIsoDisabled(){
    ui->button_Makeiso->setEnabled(false);
    ui->comboBox_Drives->setEnabled(false);
    ui->comboBox_GameName->setEnabled(false);
    ui->comboBox_GameVersion->setEnabled(false);
    ui->button_UpdateDrive->setEnabled(false);
    QMessageBox dialog;
    dialog.setIcon(QMessageBox::Information);
    dialog.setWindowTitle("Make Iso");
    dialog.setText("grabando iso en /" + getDrives());
    dialog.exec();
}

/**
 * @brief WriteGameForm::makeIsoEnabled
 */
void WriteGameForm::makeIsoEnabled(){
    ui->button_Makeiso->setEnabled(true);
    ui->comboBox_Drives->setEnabled(true);
    ui->comboBox_GameName->setEnabled(true);
    ui->comboBox_GameVersion->setEnabled(true);
    ui->button_UpdateDrive->setEnabled(true);

}

void WriteGameForm::ProgressBarHide(){
    ui->progressBar->hide();
}

void WriteGameForm::ProgressBarShow(){
    ui->progressBar->show();
}

void WriteGameForm::ProgressBarChange(int i){
    ui->progressBar->setValue(i);
}

void WriteGameForm::ProgressBarReset(){
    ui->progressBar->reset();
}


