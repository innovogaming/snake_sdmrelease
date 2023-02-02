#include "printkitdialog.h"
#include "ui_printkitdialog.h"

PrintKitDialog::PrintKitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintKitDialog)
{
    ui->setupUi(this);
    setGameList();
}

PrintKitDialog::~PrintKitDialog()
{
    delete ui;
}
/**
 * @brief PrintKitDialog::on_pushButtonOk_clicked
 */
void PrintKitDialog::on_pushButtonOk_clicked()
{
    QString ReturnValuePrintPs;
    std::string DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd.c_str();
    GameNameToPrint = ui->comboBoxGameName->currentText();
    Details = ui->lineEditDetails->text();

    if(printer.WritePsKit(GameNameToPrint, DayMonthYearToPrint, Details) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        return;
    }else{
        ReturnValuePrintPs = printer.PrintPs();
        if(ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Label Kit", "Impresion Completa");
        }else{
            QMessageBox::warning(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
    this->close();
}


/**
 * @brief PrintKitDialog::setGameList
 * llena el combobox con los nombres de los juegos
 */
void PrintKitDialog::setGameList(){
    if (conn.dbDongle.open() == true){
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Games");
        if(query.exec()){
            while(query.next()){
                result.append(query.value("GameName").toString());
            }
        }else{
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBoxGameName->clear();
        for(int i = 0; i < result.size(); i++){
            ui->comboBoxGameName->addItem(result[i]);
        }
    }else{
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief PrintKitDialog::GetStdoutFromCommand
 * ejecuta un comando y obtiene su output en string
 * @param cmd
 * @return
 */
std::string PrintKitDialog::GetStdoutFromCommand(std::string cmd) {
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

/*********************************************************************************** */
/****************************** error message fuctions **************************** */
/*********************************************************************************** */

/**
 * @brief PrintKitDialog::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void PrintKitDialog::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief PrintKitDialog::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void PrintKitDialog::dbQueryError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}


