#include "printerconfigdialog.h"
#include "ui_printerconfigdialog.h"

PrinterConfigDialog::PrinterConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrinterConfigDialog)
{
    ui->setupUi(this);
    UpdateConf();
    SearchPrinters();
}

PrinterConfigDialog::~PrinterConfigDialog()
{
    delete ui;
}

/* ******************************************************************************* */

void PrinterConfigDialog::UpdateConf(){
    ui->textEdit->clear();
    ui->textEdit->append(ReadPrintConf());
}

/**
 * @brief PrinterConfigDialog::WriteConfig
 * @return
 */
bool PrinterConfigDialog::WriteConfig(){
    bool retval;
    std::string StdString;
    QString String;
    String = ui->comboBox->currentText();
    StdString = String.toStdString();

    std::ofstream conffile ("/home/vmuser/prntconf.txt", std::ios::trunc);
    if (conffile.is_open()){
        conffile << StdString;
        conffile.close();
        retval = true;
    }else{
        qDebug() << "Unable to open file";
        retval = false;
    }
    return retval;
}

/**
 * @brief PrinterConfigDialog::on_change_pushButton_clicked
 */
void PrinterConfigDialog::on_change_pushButton_clicked(){
    QString x = ui->comboBox->currentText();
    if(!x.isEmpty()){
        if(!WriteConfig()){
            QMessageBox::warning(this, "Error", "Problema al escribir archivo de configuracion");
        }
        UpdateConf();
    }
    else QMessageBox::warning(this, "Valor Nulo", "No ha escogido ninguna impresora nueva, Presionar boton buscar");
}

/**
 * @brief PrinterConfigDialog::on_search_pushButton_clicked
 */
void PrinterConfigDialog::on_search_pushButton_clicked()
{
    SearchPrinters();
}

int PrinterConfigDialog::SearchPrinters(){
    ui->comboBox->clear();
    QStringList configlist;
    std::string ConfListStd;
    // awk '{print $3}' in spanish
    // awk '{print $2}' in english
    ConfListStd = GetStdoutFromCommand("lpstat -p | awk '{print $3}'");
    std::vector<std::string> strings = split_string(ConfListStd, "\n");

    for (uint i=0; i<strings.size(); i++) {
      configlist << QString::fromUtf8(strings.at(i).c_str(), strings.at(i).size());
    }

    for(int i=0;i<configlist.size();i++)
    {
        ui->comboBox->addItem(configlist[i]);
    }
}

/**
 * @brief PrinterConfigDialog::ReadPrintConf
 * @return
 */
QString PrinterConfigDialog::ReadPrintConf(){
    QString lineparsed;
    std::string line;
    std::ifstream conffile ("/home/vmuser/prntconf.txt");
    if (conffile.is_open()){
        while (std::getline(conffile,line)){
            lineparsed = line.c_str();
        }
        conffile.close();
    }else QMessageBox::warning(this, "Error", "Problema al leer archivo de configuracion de impresora");
    return lineparsed;
}

/**
 * @brief PrinterConfigDialog::split_string
 * @param str
 * @param delimiter
 * @return
 */
std::vector<std::string> PrinterConfigDialog::split_string(std::string str, std::string delimiter){
    std::vector<std::string> strings;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos){
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    // To get the last substring (or only, if delimiter is not found)
    //strings.push_back(str.substr(prev));

    return strings;
}

/**
 * @brief PrinterConfigDialog::GetStdoutFromCommand
 * @param cmd
 * @return std::string
 */
std::string PrinterConfigDialog::GetStdoutFromCommand(std::string cmd) {
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

void PrinterConfigDialog::on_close_pushButton_clicked()
{
    this->close();
}
