#include "readdongleform.h"
#include "ui_readdongleform.h"

ReadDongleForm::ReadDongleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadDongleForm)
{
    ui->setupUi(this);
    ui->button_PrintLabel->setEnabled(false);

}

ReadDongleForm::~ReadDongleForm()
{
    delete ui;
}

void ReadDongleForm::on_button_LeerDongle_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    readingDongleDisabled();

    getDongleData();

    readingDongleEnabled();
    QApplication::restoreOverrideCursor();

}

void ReadDongleForm::getDongleData(){
    QStandardItemModel *model = new QStandardItemModel(1,2,this);
    QStandardItem *firstRow;
    QByteArray version;
    QByteArray GameVersion;
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Item Name")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Item Value")));

    dongle.Detect_Dongle();

    Dongle::dongle_data_ dongle_status;
    dongle_status = dongle.Get_Dongle_Data();

    if(!dongle_status.status){
        QMessageBox::critical(this,"Error","Conecte una Dongle");
        return;
    }else{
        Dongle::Dongle_In_Value_ local_in;
        Dongle::Dongle_Out_Value_ local_out;

        model->setColumnCount(2);

        ui->tableView->setModel(model);
        ui->tableView->setColumnWidth(1,200);
        ui->tableView->setColumnWidth(0,200);

        version = dongle.Get_Pass_Version();
        qDebug() << "version" << version;

        GameVersion = dongle.Get_Game_Version();
        qDebug() << "GameVersion" << GameVersion;
        GameVersionToPrint = GameVersion;

        local_in = dongle.Get_In();
        local_out = dongle.Get_Out();

        //IN VALUE
        firstRow = new QStandardItem("In Value");
        model->setItem(0,0,firstRow);
        if(local_in.valid){
            firstRow = new QStandardItem(local_in.In_Str);
        }
        else{
            firstRow = new QStandardItem("Valor Invalido");
        }
        model->setItem(0,1,firstRow);

        //OUT VALUE
        firstRow = new QStandardItem("Out Value");
        model->setItem(1,0,firstRow);
        if(local_out.valid){
            firstRow = new QStandardItem(local_out.Out_Str);
        }
        else{
            firstRow = new QStandardItem("Valor Invalido");
        }
        model->setItem(1,1,firstRow);

        //HARDSERIAL VALUE
        firstRow = new QStandardItem("HardSerial");
        model->setItem(2,0,firstRow);
        firstRow = new QStandardItem(dongle_status.hardserial);
        model->setItem(2,1,firstRow);

        //MACHINEID VALUE
        QString MachineId;
        dongle.Get_Machine_Serial(MachineId);
        qDebug()<< "MachineId:" << MachineId;
        firstRow = new QStandardItem("Machine Id");
        model->setItem(3,0,firstRow);
        firstRow = new QStandardItem(MachineId);
        model->setItem(3,1,firstRow);
        MachineIdToPrint = MachineId;

        //GAMEID VALUE
        QString GameId;
        dongle.Get_Id(GameId);
        firstRow = new QStandardItem("GameId");
        model->setItem(4,0,firstRow);
        firstRow = new QStandardItem(GameId);
        model->setItem(4,1,firstRow);
        GameIdToPrint = GameId;

        //GAMENAME VALUE
        QString GameName;
        GameName = Get_GameName_by_Id(GameId.toInt());
        firstRow = new QStandardItem("GameName");
        model->setItem(5,0,firstRow);
        firstRow = new QStandardItem(GameName);
        model->setItem(5,1,firstRow);

        //MARRIED VALUE
        QString DongleMarried;
        dongle.Read_Marriage(DongleMarried);
        firstRow = new QStandardItem("Married");
        model->setItem(6,0,firstRow);
        if(!DongleMarried.isEmpty()){
            firstRow = new QStandardItem("Married");
        }
        else{
            firstRow = new QStandardItem("Not Married");
        }
        model->setItem(6,1,firstRow);

        //PASSVERSION VALUE
        firstRow = new QStandardItem("PassVersion");
        model->setItem(7,0,firstRow);
        firstRow = new QStandardItem(QString(version));
        model->setItem(7,1,firstRow);

        //GAMEVERSION VALUE
        firstRow = new QStandardItem("GameVersion");
        model->setItem(8,0,firstRow);
        firstRow = new QStandardItem(QString(GameVersion));
        model->setItem(8,1,firstRow);

        unsigned char shutdown_status=0;
        unsigned int shutdown_times=0;

        dongle.Get_Err_Flag(shutdown_status);
        dongle.Get_Bad_Shutdown(shutdown_times);

        //ERROR_FLAG VALUE
        firstRow = new QStandardItem("Corrupted Data");
        model->setItem(9,0,firstRow);
        if(shutdown_status & 0x01){
            firstRow = new QStandardItem("Flag Active");
        }
        else{
            firstRow = new QStandardItem("Flag NOT Active");
        }
        model->setItem(9,1,firstRow);

        //SYSTEM SHUTDOWN VALUE
        firstRow = new QStandardItem("System Shutdown");
        model->setItem(10,0,firstRow);
        if(shutdown_status & 0x02){
            firstRow = new QStandardItem("Flag Active");
        }
        else{
            firstRow = new QStandardItem("Flag NOT Active");
        }
        model->setItem(10,1,firstRow);

        //BAD SHUTDOWN TIMES VALUE
        firstRow = new QStandardItem("Shutdown Times");
        model->setItem(11,0,firstRow);
        firstRow = new QStandardItem(QString::number(shutdown_times));
        model->setItem(11,1,firstRow);

        ui->button_PrintLabel->setEnabled(true);
    }
}

void ReadDongleForm::on_button_PrintLabel_clicked()
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
        if(ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Label Dongle", "Impresion Completa");
        }else{
            QMessageBox::warning(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
}

std::string ReadDongleForm::GetStdoutFromCommand(std::string cmd) {
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

QString ReadDongleForm::Get_GameName_by_Id(int gameId){
    QString result;
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT * FROM Games WHERE GameId=:gameid");
        query.bindValue(":gameid",gameId);
        if(query.exec()){
            if(query.next()){
                result = query.value("GameName").toString();
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
            result = QString("");
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        result = QString("");
    }
    return result;
}

/*********************************************************************************** */
/****************************** error message fuctions ***************************** */
/*********************************************************************************** */

/**
 * @brief ReadDongleForm::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr qstring
 */
void ReadDongleForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief ReadDongleForm::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr qstring
 */
void ReadDongleForm::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief ReadDongleForm::readingDongleEnabled
 */
void ReadDongleForm::readingDongleEnabled(){
    ui->button_LeerDongle->setEnabled(true);
}

/**
 * @brief ReadDongleForm::readingDongleDisabled
 */
void ReadDongleForm::readingDongleDisabled()
{
    ui->button_LeerDongle->setEnabled(false);
    ui->button_PrintLabel->setEnabled(false);
    QMessageBox dialog;
    dialog.setIcon(QMessageBox::Information);
    dialog.setWindowTitle("Dongle");
    dialog.setText("Cargando Valores");
    dialog.exec();
}





