#include "writegametinkerform.h"
#include "ui_writegametinkerform.h"
#include "login.h"

WriteGameTinkerForm::WriteGameTinkerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WriteGameTinkerForm)
{
    ui->setupUi(this);

    setGameList();
    setDrives();

    ///
    setGameVerList();
    setDescription();

    connect(ui->comboBox_GameVersion, SIGNAL(currentIndexChanged(int)), SLOT(slot_versionChange()));
    connect(ui->comboBox_GameName, SIGNAL(currentIndexChanged(int)), SLOT(slot_gamenameChange()));
    ProgressBarHide();
    ui->Button_PrintGameLabel->setEnabled(false);
}

/**
 * @brief WriteGameTinkerForm::~WriteGameTinkerForm
 */
WriteGameTinkerForm::~WriteGameTinkerForm()
{
    delete ui;
}

void WriteGameTinkerForm::on_Button_MakeGame_clicked()
{
    qDebug() << "on_Button_MakeGame_clicked";
    ui->Button_PrintGameLabel->setEnabled(false);
    ProgressBarReset();
    ProgressBarShow();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    GameName = ui->comboBox_GameName->currentText();
    GameVer = ui->comboBox_GameVersion->currentText();
    QStringList GamePath;
    IdGame = getIdGameVer(GameName);

    makeIsoDisabled(GameName);
    ui->Obs_textEdit->clear();

    ui->Obs_textEdit->setText("Preparing to record");

    qDebug() << "GameName: " << GameName;
    qDebug() << "GameVer: " << GameVer;
    qDebug() << "IdGameVer: " << IdGame;

    //qDebug() << "passContent:" << query2.value("passContent").toString();

    GamePath = SetGamePath(IdGame, GameVer);

    qDebug() << "GamePath: " << GamePath.size() << ", " << GamePath[0] << ", " << GamePath[1] << ", " << GamePath[2];

    QStringList gpassContent = getPassContent(GamePath[1].toInt());

    //ProgressBarChange(2);
    DriveSelected = getDrives();
    ui->Obs_textEdit->setText("Detected driver: " + DriveSelected);

    if(GamePath[2] != "")
    {

        qDebug() << "SyncGame: " << SyncGame(GamePath[2], gpassContent);

        /*if(MakeGame(GamePath[2], DriveSelected))
        {
            //ProgressBarChange(75);
            QString ActionContent;
            ActionContent = "Make of Game Iso success for ";
            ActionContent.append("Game: ").append(GameName);
            ActionContent.append(" Version: ").append(GameVer);
            LogAction(ActionContent,UserId, TerminalIp, VmId);
            //ProgressBarChange(99);
            GameNameToPrint = GameName;
            GameVersionToPrint = GameVer;
            ui->Button_PrintGameLabel->setEnabled(true);
            QMessageBox::information(this, "Make Game Iso", "Operacion finalizada");
        }
        else
        {
            QMessageBox::warning(this, "Error Make Game Iso", "Problema con el Grabado del juego");
        }*/
    }
    else
    {
        QMessageBox::warning(this, "Error", "Juego o Versión no seleccionada.");
    }



    ProgressBarHide();
    QApplication::restoreOverrideCursor();
    makeIsoEnabled();
}

QStringList WriteGameTinkerForm::SetGamePath(int IdGame, QString GameVer )
{
    //QString resgamepath = NULL;
    QString ImagePath;
    QString BasePath;
    QStringList resgamepath;

    if (conn.dbDongle.open() == true)
    {

        QSqlQuery query1(conn.dbDongle);
        if(query1.exec("SELECT GamePath FROM GamesIsoPath WHERE GameVersion = 'Base'"))
        {
            if(query1.next())
            {
                BasePath = query1.value("GamePath").toString();
                resgamepath.append(query1.value("GamePath").toString());

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
        query2.prepare("SELECT GamePath, idGameVer FROM GamesIsoPath WHERE GameVersion = :gameVer AND GameId = :gameId");
        query2.bindValue(":gameVer",GameVer);
        query2.bindValue(":gameId",IdGame);

        if(query2.exec())
        {
            if(query2.next())
            {
                ImagePath = query2.value("GamePath").toString();
                resgamepath.append(query2.value("idGameVer").toString());

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
            QMessageBox::warning(this, "Error", "El campo de Nombre o Version de Juego es vacio");
        }

    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    return resgamepath;
}

int WriteGameTinkerForm::CreateFolder(const char* path)
{
    //const char* path = "/image/rootfs";
    int resultMake = mkdir(path, 0777);
    if (resultMake == 0)
    {
        qDebug()<< "A pasta foi criada com sucesso.";
    }
    else
    {
        qDebug() << "Não foi possível criar a pasta.";
    }

    return resultMake;
}

int WriteGameTinkerForm::MountFolder(const char* source,const char* target)
{
    //const char* source = "/dev/mapper/rootfs";
    //const char* target = "/image/rootfs";
    const char* filesystemtype = "ext4";
    unsigned long mountflags = MS_NOATIME | MS_NODEV | MS_NODIRATIME;
    const void* data = NULL;

    int resultMount = mount(source, target, filesystemtype, mountflags, data);
    if (resultMount == 0)
    {
        qDebug() << "Partição montada com sucesso.";
    }
    else
    {
        qDebug() << "Não foi possível montar a partição.";
    }
    return resultMount;
}

int WriteGameTinkerForm::UmountFolder(const char* partition)
{
    int resultMount = umount(partition);
    if (resultMount == 0)
    {
        qDebug() << "Partição desmontada com sucesso.";
    }
    else
    {
        qDebug() << "Erro ao desmontar partição.";
    }

    return resultMount;
}

int WriteGameTinkerForm::IfMountedFolder(const char* partition)
{
    qDebug() << "Se esta montada: " << partition;
    std::ifstream mounts("/proc/mounts");
    if (mounts.is_open())
    {
        std::string line;
        bool found = false;
        while (std::getline(mounts, line))
        {
          if (line.find(partition) != std::string::npos)
          {
            found = true;
            break;
          }
        }
        mounts.close();
        if (found)
        {
            qDebug() << "A partição está montada." ;
        }
        else
        {
          qDebug() << "A partição NÃO está montada.";
        }
    }
    else
    {
        qDebug() << "Erro ao abrir arquivo /proc/mounts.";
    }
}

int WriteGameTinkerForm::CleanFirst()
{

    qDebug() << "******* CleanFirst ";

    GetStdoutFromCommand("losetup -a");

    /*Crypto::Crypto crypto;
    int exitCrypto = crypto.closeCryptoDrive("rootfs");
    qDebug() << "Exit Crypto: " << exitCrypto;

    const char* path = "/image/rootfs";
    int result = rmdir(path);
    if (result == 0)
    {
        std::cout << "A pasta foi apagada com sucesso." << std::endl;
    }
    else
    {
        std::cout << "Não foi possível apagar a pasta." << std::endl;
    }*/



    return 0;
}

int WriteGameTinkerForm::SyncGame(QString gamepath_, QStringList gpassContent_){

    QString output;
    qDebug() << "SyncGame - gamepath: " << gamepath_ << ", gpassContent: " << gpassContent_.size();
    qDebug() << "gpassContent_ - passContent: " << gpassContent_.at(0) << ", PassData: " << gpassContent_.at(1) << ", intVersion: " << gpassContent_.at(2);
    qDebug() << "matrizImage: " << matrizImage;

    CleanFirst();
    //if( CleanFirst() == 0 )
      //  return 0;

    //resultList.append(query.value("passContent").toString());
    //resultList.append(query.value("PassData").toString());
    //resultList.append(query.value("intVersion").toString());
    output = GetStdoutFromCommand("ls " + matrizImage);
    if(output.contains(matrizImage))
    {
        qDebug() << "YES ";

        Crypto::Crypto crypto;

        QString losetupAvaiable = GetStdoutFromCommand("losetup -f");
        QStringList losetup_ = losetupAvaiable.split('/');
        qDebug() << losetup_.size() << ", " << losetup_.at(2);
        GetStdoutFromCommand("losetup -v " + losetupAvaiable + " " + matrizImage);
        GetStdoutFromCommand("kpartx -av " + losetupAvaiable);
        sleep(1);
        QString tmpBuffer = "/dev/mapper/" + losetup_.at(2) + "p9";
        //std::string device_str = tmpBuffer.toStdString();
        QString tmpPass = gpassContent_.at(1) + gpassContent_.at(2);

        //qDebug() << " tmpBuffer: " << device_str;

        int exitCrypto = crypto.Build(tmpBuffer.toUtf8().constData(), tmpPass.toStdString(),"rootfs");
        qDebug() << "Exit Crypto: " << exitCrypto;
        if( exitCrypto == 0)
        {
            GetStdoutFromCommand("mkfs.ext4 /dev/mapper/rootfs");
            //GetStdoutFromCommand("ls -l /image/rootfs");
            //GetStdoutFromCommand("mkdir -v /image/rootfs");

            CreateFolder("/image/rootfs");

            //GetStdoutFromCommand("mount -v /dev/mapper/rootfs /image/rootfs");

            MountFolder("/dev/mapper/rootfs","/image/rootfs");

        }


        QString contentImage = "/jabulani/images/content_03-08-2023.img";
        QString losetupAvaiable2 = GetStdoutFromCommand("losetup -f");
        QStringList losetup2_ = losetupAvaiable2.split('/');
        qDebug() << losetup2_.size() << ", " << losetup2_.at(2);
        GetStdoutFromCommand("losetup -v " + losetupAvaiable2 + " " + contentImage);
        GetStdoutFromCommand("kpartx -av " + losetupAvaiable2);
        sleep(1);
        QString tmpBuffer2 = "/dev/mapper/" + losetup2_.at(2) + "p1";
        //std::string device_str = tmpBuffer.toStdString();
        QString tmpPass2 = gpassContent_.at(0);
        //openCryptoDrive(const char *path, char* passkey, int passSize, int flag, const char *device_name);
        int exitCrypto2 = crypto.openCryptoDrive(tmpBuffer2.toUtf8().constData(), tmpPass2.toLatin1().data(),tmpPass2.size(), 0, "content");
        qDebug() << "Exit Crypto 2: " << exitCrypto2;
        if( exitCrypto2 == 0)
        {
            GetStdoutFromCommand("mkfs.ext4 /dev/mapper/content");

            CreateFolder("/image/content");

            MountFolder("/dev/mapper/content","/image/content");

        }


        //GetStdoutFromCommand("/image/Tools/crypt " + gpassContent_.at(1) + gpassContent_.at(2));
        //GetStdoutFromCommand("mkdir -v " + matrizDrive + "content");
        //GetStdoutFromCommand("cryptsetup luksOpen /dev/mapper/" + losetup_.at(2) + "p9");
        /*QProcess process;
        process.start("cryptsetup", QStringList() << "luksOpen" << "--key-file=-" << "/dev/mapper/" + losetup_.at(2) + "p9" << "encrypted_device");
        process.waitForStarted(-1);
        process.write("dqcBQOiP0Zprjwa5TsjULnXF43ggobZk\n");
        process.closeWriteChannel();
        process.waitForFinished(-1);
        qDebug() << "Saída do processo:" << process.readAllStandardOutput();*/

         //GetStdoutFromCommand(" " + losetupAvaiable);
        /*QProcess *process = new QProcess();
        QProcess *sync1 = new QProcess();
        QProcess *sync2 = new QProcess();

        process->start(cmd);

        // process.write("ls -l");
        //process.setStandardOutputProcess(&process2);

        //process.start(cmd);
        process->waitForStarted(-1);
        //process->waitForFinished(-1);
        //process2.start("2>&1");
        QByteArray output, errorOutput;
        while (1)
        {
            output = process->readAll();
            errorOutput = process->readAllStandardError();

            //qDebug() << "output: "<< output;
            //qDebug() << "errorOutput: "<< errorOutput << ", output: " << output << ", state: " << process->exitStatus() << ", " << process->processChannelMode() << ", " << process->state() << ", " << process->exitCode() ;
            //sleep(1);
            //qDebug() << "sizeCmd: " << sizeCmd;

            QStringList listTmp = QString::fromLocal8Bit(errorOutput).split(" ");

            if(listTmp.size() > 1 )
            {
                QStringList listSearch = listTmp.at(0).split("\r");

                if(listSearch.size() > 1 )
                {
                    sizeNow = listSearch.at(1).toDouble();
                    //percentageStatus = ( ( sizeNow / sizeCmd ) * 100 );
                    //progressPorcentage = qRound(percentageStatus);

                    qDebug() << "sizeNow: " << sizeNow << ", sizeCmd: " << sizeCmd << ", " << qRound( ( sizeNow / sizeCmd ) * 100 ) << "%";

                    ProgressBarChange(qRound( ( sizeNow / sizeCmd ) * 100 ));
                }
            }

            if( process->state() == QProcess::NotRunning )
                break;
            else
                process->waitForFinished(1000);
        }


        if( sizeCmd > 0)
        {
            ui->Obs_textEdit->clear();
            ui->Obs_textEdit->setText("Wait, synchronizing...");

            QStringList entries = QString::fromLocal8Bit(errorOutput).split('\n');
            foreach(QString entry, entries) {
                qDebug() << "entry: " << entry;
                data += entry;
            }

            process->close();



            sync1->start("sync");
            sync1->waitForFinished(-1);
            qDebug() << "sync 1" << sync1->readAll();
            qDebug() << "sync 1" << sync1->readAllStandardError();
            sync1->close();

            sync2->start("sync");
            sync2->waitForFinished(-1);
            qDebug() << "sync 2" << sync2->readAll();
            qDebug() << "sync 2" << sync2->readAllStandardError();
            sync2->close();

        }
        else
        {
            QStringList entries = QString::fromLocal8Bit(output).split('\n');
            foreach(QString entry, entries) {
                qDebug() << "entry: " << entry;
                data += entry;
            }

            process->close();
        }*/
    }
    else
    {
         qDebug() << "NOT ";
    }

    return 0;
}

/**
 * @brief WriteGameTinkerForm::MakeGame
 * @param gamepath_
 * @param driveselected_
 * @return
 */
int WriteGameTinkerForm::MakeGame(QString gamepath_, QString driveselected_){
    int res = 0;
    QString command1;
    QString command2 = "du -sb ";
    QString output1;
    QString output2;
    QString str1, str2, str3;
    double sizeDrive = 0;

    str1 = " dd if=";
    str2 = " of=/dev/";
    str3 = " bs=1M status=progress";

    if(gamepath_ != "" && driveselected_ != ""){
        command1.append(str1);                          // sudo dd if=
        command1.append(gamepath_);                     // /sysdongle/...
        command1.append(str2);                          // of=/dev/
        command1.append(driveselected_);                // sd*
        command1.append(str3);                          // bs...

       // sizeDrive = stoll(GetStdoutFromCommand(command2.append(gamepath_)));
        output2 = GetStdoutFromCommand(command2.append(gamepath_));
        //qDebug() << "FAYOS output2:" << output2;
        if(output2.contains(gamepath_))
        {
            //qDebug()<<"TEM SAPORRA";
            QStringList pieces = output2.split( "\t" );
            //qDebug()<<"output2: " << output2;
            //qDebug()<<"pieces 0: " << pieces[0];
            sizeDrive = pieces[0].toDouble();
            //ui->Obs_textEdit->setText("Size: " + pieces[0]);
        }
        //qDebug() << "Size: " << sizeDrive;
        //return 0;
        //qDebug() << "FAYOS command1:" << command1;
        double gygabytes = sizeDrive / 1073741824.0;
        ui->Obs_textEdit->setText("File size to be recorded: " + QString::number(gygabytes,'f',2) + " gigabytes");


        output1 = GetStdoutFromCommand(command1,sizeDrive);

        //qDebug() << "FAYOS output1::" << output1;

        sleep(1);

        //QString cmdexecuted = output1;

        //qDebug() << "FAYOS cmdexecuted::" << cmdexecuted;

        if(output1.contains("No se")) res = 0;

        if(output1.contains("copied"))
        {
            res = 1;
            ProgressBarChange(100);
            ui->Obs_textEdit->setText("Well done, remove the driver from the computer!");
        }
    }
    else{
        QMessageBox::warning(this, "Error", "El campo del Juego o Drive esta vacio");
    }

    return res;
}

/**
 * @brief WriteGameTinkerForm::getDrives
 * @return QString DriveSelected
 */
QString WriteGameTinkerForm::getDrives(){
    QString driveSel = NULL;
    driveSel = ui->comboBox_Drives->currentText();
    return driveSel;
}

/**
 * @brief WriteGameTinkerForm::setDrives
 */
void WriteGameTinkerForm::setDrives(){
    ui->comboBox_Drives->clear();

    QString checksdb = GetStdoutFromCommand("lsblk /dev/sdb");
    QString checksdc = GetStdoutFromCommand("lsblk /dev/sdc");
    QString checksdd = GetStdoutFromCommand("lsblk /dev/sdd");
    QString checksde = GetStdoutFromCommand("lsblk /dev/sde");
    QString checksdf = GetStdoutFromCommand("lsblk /dev/sdf");

    if((checksdb.contains("sdb")) && (checksdb.contains("16M"))){ //its a dongle, dont add it
    }else if((checksdb.contains("sdb")) && (checksdb.contains("sdb1"))){ //its a new MicroSD
        ui->comboBox_Drives->addItem("sdb");
    }else if((checksdb.contains("sdb")) && (checksdb.contains("sdb1")) && (checksdb.contains("sdb2")) ) {
        ui->comboBox_Drives->addItem("sdb"); //its a already-written MicroSD
    }

    if((checksdc.contains("sdc")) && (checksdc.contains("16M"))){
    }else if((checksdc.contains("sdc")) && (checksdc.contains("sdc1"))){
        ui->comboBox_Drives->addItem("sdc");
    }else if((checksdc.contains("sdc")) && (checksdc.contains("sdc1")) && (checksdc.contains("sdc2")) ) {
        ui->comboBox_Drives->addItem("sdc");
    }

    if((checksdd.contains("sdd")) && (checksdd.contains("16M"))){
    }else if((checksdd.contains("sdd")) && (checksdd.contains("sdd1"))){
        ui->comboBox_Drives->addItem("sdd");
    }else if((checksdd.contains("sdd")) && (checksdd.contains("sdd1")) && (checksdd.contains("sdd2")) ) {
        ui->comboBox_Drives->addItem("sdd");
    }

    if((checksde.contains("sde")) && (checksde.contains("16M"))){
    }else if((checksde.contains("sde")) && (checksde.contains("sde1"))){
        ui->comboBox_Drives->addItem("sde");
    }else if((checksde.contains("sde")) && (checksde.contains("sde1")) && (checksde.contains("sde2")) ) {
        ui->comboBox_Drives->addItem("sde");
    }

    if((checksdf.contains("sdf")) && (checksdf.contains("16M"))){
    }else if((checksdf.contains("sdf")) && (checksdf.contains("sdf1"))){
        ui->comboBox_Drives->addItem("sdf");
    }else if((checksdf.contains("sdf")) && (checksdf.contains("sdf1")) && (checksdf.contains("sdf2")) ) {
        ui->comboBox_Drives->addItem("sdf");
    }
}


/**
 * @brief WriteGameTinkerForm::on_Button_PrintGameLabel_clicked
 */
void WriteGameTinkerForm::on_Button_PrintGameLabel_clicked()
{
    QString ReturnValuePrintPs;
    //QString MachineIdToPrint = "";
    QString DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd;

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

/**
 * @brief WriteGameTinkerForm::LogAction
 * @param action
 * @param UserId
 * @param TerminalIp_
 * @param VmId_
 */
void WriteGameTinkerForm::LogAction(QString action, int UserId, QString TerminalIp_, QString VmId_){
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
 * @brief WriteGameTinkerForm::setGameList
 */
void WriteGameTinkerForm::setGameList(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        if(query.exec("SELECT DISTINCT Games.GameName FROM Games, GamesVersions WHERE Games.GameId = GamesVersions.GameId AND GamesVersions.tipo = 'tinker' AND Games.Status = 'tinker' ORDER BY GamesVersions.GameId ASC")){
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
           // qDebug() << "Last: " << result[i];
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief WriteGameTinkerForm::setGameVerList
 */
void WriteGameTinkerForm::setGameVerList()
{
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
 * @brief WriteGameTinkerForm::getGameVerList
 * @param gamename
 * @return
 */
QStringList WriteGameTinkerForm::getGameVerList(QString gamename){
    QStringList result2;
    if (conn.dbDongle.open() == true){
        int gameId;
        gameId = getGameId(gamename);

        qDebug() << "gameId1:" << gameId;

        QSqlQuery query2(conn.dbDongle);
        query2.setForwardOnly(true);
        query2.prepare("SELECT * FROM GamesVersions WHERE GameId =:gameid AND tipo = 'tinker'");
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

    qDebug() << "getGameVerList: " << result2;

    return result2;
}

/**
 * @brief WriteGameTinkerForm::getGameId
 * @param gamename
 * @return
 */
int WriteGameTinkerForm::getGameId(QString gamename)
{
    int result1 = 0;
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT GameId FROM Games WHERE GameName =:gamename");
        query.bindValue(":gamename",gamename);

        if(query.exec())
        {
            if(query.next())
            {
                 result1 = query.value("GameId").toInt();
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return result1;
}

int WriteGameTinkerForm::getIdGameVer(QString gamename)
{
    int result1 = 0;
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT gameId FROM Games WHERE GameName =:gamename");
        query.bindValue(":gamename",gamename);

        if(query.exec())
        {
            if(query.next())
            {
                 result1 = query.value("gameId").toInt();
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    qDebug() << "getIdGameVer:" << result1;
    return result1;
}

QStringList WriteGameTinkerForm::getPassContent(int idGameVer)
{
    QStringList resultList;

    //QString passContent;
    //QString PassData;
    //QString passInt;
    if (conn.dbDongle.open() == true)
    {
        /*QSqlQuery query2(conn.dbDongle);
        query2.setForwardOnly(true);
        query2.prepare("SELECT * FROM GamesVersions WHERE GameId =:gameid AND tipo = 'tinker'");
        query2.bindValue(":gameid",gameId);*/

        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT GamesVersions.passContent, PassVersions.PassData, PassVersions.intVersion FROM GamesVersions LEFT JOIN PassVersions ON PassVersions.PassVersion = GamesVersions.PassVersion WHERE GamesVersions.idGameVer =:idGameVer");
        query.bindValue(":idGameVer",idGameVer);

        if(query.exec())
        {
            if(query.next())
            {
                 //passContent = query.value("passContent").toString();

                 //ImagePath = query2.value("GamePath").toString();
                 resultList.append(query.value("passContent").toString());
                 resultList.append(query.value("PassData").toString());
                 resultList.append(query.value("intVersion").toString());
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }

    qDebug() << "resultList: " << resultList.size();

    return resultList;
}

/**
 * @brief WriteGameTinkerForm::slot_gamenameChange
 */
void WriteGameTinkerForm::slot_gamenameChange(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setGameVerList();
    setDescription();
    QApplication::restoreOverrideCursor();
}

/**
 * @brief WriteGameTinkerForm::slot_versionChange
 */
void WriteGameTinkerForm::slot_versionChange(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setDescription();
    QApplication::restoreOverrideCursor();
}

/**
 * @brief WriteGameTinkerForm::GetStdoutFromCommand
 * @param cmd
 * @return
 */
QString WriteGameTinkerForm::GetStdoutFromCommand(QString cmd, double sizeCmd) {
    QString data;
    //FILE * stream;
    //const int max_buffer = 64;
    //char buffer[max_buffer];
    double sizeNow = 0;

    QString bufferSplit;

    //qDebug() << "size:" << gygabytes;
    //std::array<char, 80> bufferTmp;
    //stream = popen(cmd.std::string::c_str(), "r");

    QProcess *process = new QProcess();
    QProcess *sync1 = new QProcess();
    QProcess *sync2 = new QProcess();


    process->start(cmd);

    // process.write("ls -l");
    //process.setStandardOutputProcess(&process2);

    //process.start(cmd);
    process->waitForStarted(-1);
    //process->waitForFinished(-1);
    //process2.start("2>&1");
    QByteArray output, errorOutput;
    while (1)
    {
        output = process->readAll();
        errorOutput = process->readAllStandardError();

        //qDebug() << "output: "<< output;
        //qDebug() << "errorOutput: "<< errorOutput << ", output: " << output << ", state: " << process->exitStatus() << ", " << process->processChannelMode() << ", " << process->state() << ", " << process->exitCode() ;
        //sleep(1);
        //qDebug() << "sizeCmd: " << sizeCmd;

        QStringList listTmp = QString::fromLocal8Bit(errorOutput).split(" ");

        if(listTmp.size() > 1 )
        {
            QStringList listSearch = listTmp.at(0).split("\r");

            if(listSearch.size() > 1 )
            {
                sizeNow = listSearch.at(1).toDouble();
                //percentageStatus = ( ( sizeNow / sizeCmd ) * 100 );
                //progressPorcentage = qRound(percentageStatus);

                qDebug() << "sizeNow: " << sizeNow << ", sizeCmd: " << sizeCmd << ", " << qRound( ( sizeNow / sizeCmd ) * 100 ) << "%";

                ProgressBarChange(qRound( ( sizeNow / sizeCmd ) * 100 ));
            }
        }

        if( process->state() == QProcess::NotRunning )
            break;
        else
            process->waitForFinished(1000);
    }


    if( sizeCmd > 0)
    {
        ui->Obs_textEdit->clear();
        ui->Obs_textEdit->setText("Wait, synchronizing...");

        QStringList entries = QString::fromLocal8Bit(errorOutput).split('\n');
        foreach(QString entry, entries)
        {
            qDebug() << "Entry: " << entry;

            data += entry;

        }

        process->close();

        sync1->start("sync");
        sync1->waitForFinished(-1);
        qDebug() << "sync 1" << sync1->readAll();
        qDebug() << "sync 1" << sync1->readAllStandardError();
        sync1->close();

        sync2->start("sync");
        sync2->waitForFinished(-1);
        qDebug() << "sync 2" << sync2->readAll();
        qDebug() << "sync 2" << sync2->readAllStandardError();
        sync2->close();

    }
    else
    {
        QStringList entries = QString::fromLocal8Bit(output).split('\n');
        foreach(QString entry, entries) {
            qDebug() << "entry: " << entry;
            if( entry.contains("img_lab.img"))
            {
                //qDebug() << "TEM ALGO AQUI";
                QStringList searchLoop = entry.split(':');

                qDebug() << "loop: " << searchLoop.at(0);

                if( IfMountedFolder(searchLoop.at(0).toUtf8().constData()) )
                {
                    qDebug() << "Ta montada, desmonta primeiro";
                }

                QProcess *kpartxProcess = new QProcess();
                kpartxProcess->start("kpartx -d " + searchLoop.at(0));
                kpartxProcess->waitForFinished(-1);
                qDebug() << "kpartx: " << kpartxProcess->readAllStandardOutput();
                qDebug() << "kpartx: " << kpartxProcess->readAllStandardError();
                kpartxProcess->close();

                QProcess *losetupProcess = new QProcess();
                losetupProcess->start("losetup -d " + searchLoop.at(0));
                losetupProcess->waitForFinished(-1);
                qDebug() << "losetup: " << losetupProcess->readAllStandardOutput();
                qDebug() << "losetup: " << losetupProcess->readAllStandardError();
                losetupProcess->close();


            }

            data += entry;
        }

        process->close();
    }

    qDebug() << "Make output:" << data;




    /*if (stream)
    {
        while (!feof(stream))
        {
            if (fgets(buffer, max_buffer, stream) != NULL)
            {
                //qDebug() << "Debug: " << buffer;
                data.append(buffer);

                bufferSplit = QString::fromLocal8Bit(buffer);
                qDebug() << "exit: " << bufferSplit;
                QStringList pieces = bufferSplit.split( " bytes" );
                //char *ptr = std::strtok(buffer, " ");
                qDebug() << "Debug: " << pieces.first();
                memset(buffer, 0, sizeof buffer);
            }
        }
        pclose(stream);
    }
    else{
        QMessageBox::warning(this, "Error", "Problema al ejecutar Sub Proceso");
    }*/
    return data;
}

/**
 * @brief WriteGameTinkerForm::setDescription
 */
void WriteGameTinkerForm::setDescription()
{
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
        query.prepare("SELECT Descripcion FROM GamesVersions WHERE GameId =:gameid AND GameVersion =:gamever AND tipo = 'tinker' ");
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

void WriteGameTinkerForm::on_button_UpdateDrive_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setDrives();
    QApplication::restoreOverrideCursor();

}

/* ******************************************************************************* */
/* ******************************************************************************* */
/* ******************************************************************************* */

/**
 * @brief WriteGameTinkerForm::dbConnectionError
 * @param dblasterr
 */
void WriteGameTinkerForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}


/**
 * @brief WriteGameTinkerForm::dbQueryError
 * @param dblasterr
 */
void WriteGameTinkerForm::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}


/**
 * @brief WriteGameTinkerForm::makeIsoDisabled
 */
void WriteGameTinkerForm::makeIsoDisabled(QString gamename_){
    ui->Button_MakeGame->setEnabled(false);
    ui->comboBox_GameName->setEnabled(false);
    ui->comboBox_GameVersion->setEnabled(false);
    ui->button_UpdateDrive->setEnabled(false);

    QMessageBox dialog;
    dialog.setIcon(QMessageBox::Information);
    dialog.setWindowTitle("Make Iso");
    dialog.setText("Grabando imagen de juego: " + gamename_ + "\nEn memoria microSD - TinkerBoard");
    dialog.exec();
}

/**
 * @brief WriteGameTinkerForm::makeIsoEnabled
 */
void WriteGameTinkerForm::makeIsoEnabled(){
    ui->Button_MakeGame->setEnabled(true);
    ui->comboBox_GameName->setEnabled(true);
    ui->comboBox_GameVersion->setEnabled(true);
    ui->button_UpdateDrive->setEnabled(true);
}

/**
 * @brief WriteGameTinkerForm::ProgressBarHide
 */
void WriteGameTinkerForm::ProgressBarHide(){
    ui->progressBar->hide();
}

/**
 * @brief WriteGameTinkerForm::ProgressBarShow
 */
void WriteGameTinkerForm::ProgressBarShow(){
    ui->progressBar->show();
}

/**
 * @brief WriteGameTinkerForm::ProgressBarChange
 * @param i
 */
void WriteGameTinkerForm::ProgressBarChange(int i){
    ui->progressBar->setValue(i);
}

/**
 * @brief WriteGameTinkerForm::ProgressBarReset
 */
void WriteGameTinkerForm::ProgressBarReset(){
    ui->progressBar->reset();
}



