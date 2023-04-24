#include "writepipicoform.h"
#include "ui_writepipicoform.h"
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <dirent.h>

WritePiPicoForm::WritePiPicoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WritePiPicoForm)
{
    ui->setupUi(this);

    setAplicationList();
    setAppVerList();
    setDescription();
    connect(ui->comboBox_Aplication, SIGNAL(currentIndexChanged(int)), SLOT(slot_previousIndex(int)));
    connect(ui->comboBox_Version, SIGNAL(currentIndexChanged(int)), SLOT(slot_versionChange()));
    ui->PrintLabel_Button->setEnabled(false);
}

WritePiPicoForm::~WritePiPicoForm()
{
    delete ui;
}

/*********************************************************************************** */
/****************************** get and set fuctions ******************************* */
/*********************************************************************************** */

/**
 * @brief WritePiPicoForm::setAplicationList
 * llena el combobox con los nombres de los juegos
 */
void WritePiPicoForm::setAplicationList(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.setForwardOnly(true);
        query.prepare("SELECT DISTINCT Aplications.AppName FROM Aplications, AplicationsVersion WHERE Aplications.AppId = AplicationsVersion.AppId"); // AND AplicationsVersion.tipo = 'dongle' AND Games.Status = 'dongle' ");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("AppName").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_Aplication->clear();
        for(int i = 0; i < result.size(); i++){
            ui->comboBox_Aplication->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief WritePiPicoForm::setAppVerList
 * llena el combobox con las versiones del juego seleccionado
 */
void WritePiPicoForm::setAppVerList(){
    ui->comboBox_Version->clear();
    QStringList appvers;
    QString appCurrent;
    appCurrent = ui->comboBox_Aplication->currentText();
    appvers = getAppVerList(appCurrent);
    for(int i=0;i<appvers.size();i++)
    {
        ui->comboBox_Version->addItem(appvers[i]);
    }
}

/**
 * @brief WritePiPicoForm::getAppVerList
 * obtiene las versiones del juego seleccionado
 * @param gamename
 * @return
 */
QStringList WritePiPicoForm::getAppVerList(QString appname){
    QStringList result2;
    if (conn.dbDongle.open() == true){
        int appId;
        appId = getAppId(appname);
        QSqlQuery query2(conn.dbDongle);
        query2.setForwardOnly(true);
        query2.prepare("SELECT * FROM AplicationsVersion WHERE AppId =:appid"); // AND tipo = 'dongle'");
        query2.bindValue(":appid",appId);

        if(query2.exec()){
            while(query2.next()){
                result2.append(query2.value("AppVersion").toString());
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
 * @brief WritePiPicoForm::getAppId
 * obtiene el Id del juego escogido
 * @param gamename
 * @return
 */
int WritePiPicoForm::getAppId(QString appname){
    int result1 = 0;
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.setForwardOnly(true);
        query.prepare("SELECT AppId FROM Aplications WHERE AppName =:appname");
        query.bindValue(":appname",appname);

        if(query.exec()){
            if(query.next()){
                 result1 = query.value("AppId").toInt();
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

void WritePiPicoForm::setDescription(){
    QString     result;
    QString     appname_;
    QString     appver_;
    int         appid_;
    appname_   = ui->comboBox_Aplication->currentText();
    appver_    = ui->comboBox_Version->currentText();
    appid_     = getAppId(appname_);

    ui->description_textEdit ->clear();

    if(conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.setForwardOnly(true);
        query.prepare("SELECT Description FROM AplicationsVersion WHERE AppId =:appid AND AppVersion =:appver"); //ever AND tipo = 'dongle' ");
        query.bindValue(":appid", appid_);
        query.bindValue(":appver", appver_);
        if(query.exec()){
            if(query.next()){
                result = query.value("Description").toString();
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->description_textEdit->setText(result);
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

QStringList WritePiPicoForm::SetAppPath(int AppId, QString AppVer)
{
    //QString resgamepath = NULL;
    QString ImagePath;
    QString BasePath;
    QStringList resgamepath;

    if (conn.dbDongle.open() == true)
    {

        QSqlQuery query1(conn.dbDongle);
        if(query1.exec("SELECT AppPath FROM AplicationsPath WHERE AppVersion = 'Base'"))
        {
            if(query1.next())
            {
                BasePath = query1.value("AppPath").toString();
                resgamepath.append(query1.value("AppPath").toString());

                qDebug() << "query1 :" << BasePath << ", " << resgamepath.size();
            }
        }
        else
        {
            QString querylasterr = query1.lastError().text();
            dbQueryError(querylasterr);
        }

        //game path
        QSqlQuery query2(conn.dbDongle);
        query2.prepare("SELECT AppPath, idAppVer FROM AplicationsPath WHERE AppVersion = :AppVer AND AppId = :AppId");
        query2.bindValue(":AppVer",AppVer);
        query2.bindValue(":AppId",AppId);

        if(query2.exec())
        {
            if(query2.next())
            {
                ImagePath = query2.value("AppPath").toString();
                resgamepath.append(query2.value("idAppVer").toString());

                qDebug() << "query2 :" << ImagePath << ", " << resgamepath.size();
            }
        }
        else
        {
            QString querylasterr = query2.lastError().text();
            dbQueryError(querylasterr);
        }

        if(ImagePath != "")
        {
            QString tmpPath = BasePath;
            tmpPath.append(ImagePath);
            resgamepath.append(tmpPath);

            qDebug() << "tmpPath :" << tmpPath << ", " << resgamepath.size();

        }
        else
        {
            QMessageBox::warning(this, "Error", "El campo de Nombre o Version de App es vacio");
        }

    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

        qDebug() << "Applications Path: " << resgamepath;
    return resgamepath;
}

/**
 * @brief WritePiPicoForm::on_Grabar_Button_clicked
 */
void WritePiPicoForm::on_Grabar_Button_clicked()
{
    qDebug() << "on_Grabar_Button_clicked";
    ui->PrintLabel_Button->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    AppName = ui->comboBox_Aplication->currentText();
    AppVer = ui->comboBox_Version->currentText();
    QStringList AppPath;
    AppId = getAppId(AppName);
    qDebug() << "App Id: " << AppId;
    //ui->description_textEdit->clear();
    ui->description_textEdit->setText("\nPreparando para grabar RPI-RP2");
    AppPath = SetAppPath(AppId, AppVer);
    //qDebug() << "App Path: " << AppPath;
    //qDebug() << "AppName: " << AppName;

    QString ActContent;

    std::string USER = GetStdoutFromCommand("echo $USER");
    USER = USER.substr(0,USER.find("\n"));
    qDebug() << "User: " << USER.c_str();
    std::string tmp = "mkdir /media/" + USER + "/RPI-RP2";
    GetStdoutFromCommand(tmp);
    std::string command1("dmesg | tail -2");
    QString drive = GetStdoutFromCommand(command1).c_str();
    qDebug() << "drive: " << drive;
    //mando a montar nuevamente el dispositivo.
    if(drive.contains(": sd"))
    {
        tmp = drive.toStdString().substr(drive.toStdString().find(":")+2, 4);
        qDebug() << "drive: " << tmp.c_str();
        tmp = "mount /dev/" + tmp +  " /media/" + USER + "/RPI-RP2";
        qDebug() << "command: " << tmp.c_str();
        GetStdoutFromCommand(tmp);
        std::string command3 = " /media/"+USER+"/RPI-RP2/NEW.UF2";

        std::string command = "rsync -ah --progress " + AppPath[2].toStdString() + command3;
        qDebug() << "Command: " << command.c_str();
        std::string result = GetStdoutFromCommand(command);
        qDebug() << "result: " << result.c_str();

        tmp = "umount /media/" + USER + "/RPI-RP2";
        qDebug() << "command: " << tmp.c_str();
        GetStdoutFromCommand(tmp);

        command = "dmesg | tail -4";
        std::string delimiter("SerialNumber: ");

        do
        {

        drive = GetStdoutFromCommand(command).c_str();
        //qDebug() << "drive: " << drive;
        }while(!drive.contains(delimiter.c_str()));

        result = GetStdoutFromCommand(command);
        Id = result.substr((result.find(delimiter) + 14), 16);

        content = QString("ID: %1").arg(Id.c_str());
        qDebug() << "content: " << content;
        //ui->description_textEdit->clear();
        //ui->description_textEdit->setAlignment(Qt::AlignHCenter);
        ui->description_textEdit->setText("\n" + content);

        std::string tmp = "rmdir /media/" + USER + "/RPI-RP2";
        GetStdoutFromCommand(tmp);

        QApplication::restoreOverrideCursor();
        ui->PrintLabel_Button->setEnabled(true);

        ActContent = "Flash RPI-RP2 Sucess: ";
        ActContent.append("AppName: ").append(AppName);
        ActContent.append(", AppVer: ").append(AppVer);
        ActContent.append(", BoardId: ").append(Id.c_str());
        qDebug() << ActContent;
        LogAction(ActContent, UserId , TerminalIp, VmId);
        QMessageBox::information(this, "Flash RPI-RP2", "Escritura correcta de RPI-RP2");
    }
    else
    {
        //QMessageBox::warning(this, "Message", "Connect RPI PICO in BOOT mode to flash it");
        QMessageBox::warning(this, "Atencion!", "Conecte RPI-RP2 en modo BOOT para grabar");
    }

}

/**
 * @brief WritePiPicoForm::LogAction
 * @param action
 * @param UserId
 * @param TerminalIp_
 * @param VmId_
 */
void WritePiPicoForm::LogAction(QString action, int UserId, QString TerminalIp_, QString VmId_){
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

/**
 * @brief WritePiPicoForm::on_PrintLabel_Button_clicked
 */
void WritePiPicoForm::on_PrintLabel_Button_clicked()
{
    QString ReturnValuePrintPs;
    //std::string DayMonthYearToPrintStd;
    //DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    QString DayMonthYearToPrint = GetStdoutFromCommand("date +\"%d/%m/%y\"").c_str();

    if(printer.WriteRPI_RP2(AppName, AppVer, content) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        //ui->PrintLabel_Button->setEnabled(false);
        return;
    }else{
        ReturnValuePrintPs = printer.PrintPs();
        if (ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Etiqueta Tinker", "Impresion Completa");
        }else{
            QMessageBox::critical(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
}


/**
 * @brief WritePiPicoForm::slot_previousIndex
 * identifica el cambio de index
 * @param previousIndex
 */
void WritePiPicoForm::slot_previousIndex(int previousIndex){
    int currentIndex;
    currentIndex = ui->comboBox_Aplication->currentIndex();

    if(currentIndex == previousIndex){
        QApplication::setOverrideCursor(Qt::WaitCursor);
        setAppVerList();
        setDescription();
        QApplication::restoreOverrideCursor();
    }
}

void WritePiPicoForm::slot_versionChange(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setDescription();
    QApplication::restoreOverrideCursor();
}

/*********************************************************************************** */
/****************************** error message fuctions **************************** */
/*********************************************************************************** */

/**
 * @brief WritePiPicoForm::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WritePiPicoForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WritePiPicoForm::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WritePiPicoForm::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

std::string WritePiPicoForm::GetStdoutFromCommand(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.std::string::c_str(), "r");
    if (stream)
    {
        while (!feof(stream))
        {
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        }
        pclose(stream);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Problema al ejecutar Sub Proceso");
    }
    return data;
}
