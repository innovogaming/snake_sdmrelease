#include "printer.h"

Printer::Printer()
{

}

bool Printer::WriteRPI_RP2(QString appname, QString appver, QString content){
    bool retval;
    std::string StdStringPs;
    QString PsString;
    QString LogoString = AddLogo();
    PsString = "";
    PsString.append("%!PS-Adobe-3.0").append("\n");
    PsString.append("%cupsJobTicket: media=custom_17.02x53.85mm_17.02x53.85mm sides=one-sided").append("\n");
    PsString.append("/Times-Roman findfont 12 scalefont setfont").append("\n");
    PsString.append("20 10 moveto 90 rotate ").append("(").append(appname).append("_").append(appver).append(") show 90 neg rotate").append("\n");
    PsString.append("35 10 moveto 90 rotate ").append("(").append(content).append(") show 90 neg rotate").append("\n");
    PsString.append(" ").append("\n");
    PsString.append("showpage").append("\n");

    StdStringPs = PsString.toStdString();
    std::ofstream psfile ("/home/vmuser/sdmlabel.ps", std::ios::trunc);
    if (psfile.is_open()){
        psfile << StdStringPs;
        psfile.close();
        retval = true;
    }else{
        qDebug() << "Unable to open file";
        retval = false;
    }

    return retval;
}

/**
 * @brief Printer::WritePs
 * @param gamver
 * @param gameid
 * @param machid
 * @param date
 * @return bool return value
 */
bool Printer::WritePs(QString gamver, QString gameid, QString machid, QString date){
    bool retval;
    std::string StdStringPs;
    QString PsString;
    QString LogoString = AddLogo();
    PsString = "";
    PsString.append("%!PS-Adobe-3.0").append("\n");
    PsString.append("%cupsJobTicket: media=custom_17.02x53.85mm_17.02x53.85mm sides=one-sided").append("\n");
    PsString.append("/Times-Roman findfont 10 scalefont setfont").append("\n");
    PsString.append("10 10 moveto").append("\n");
    PsString.append("(").append(machid).append(") show").append("\n");
    PsString.append("10 20 moveto").append("\n");
    PsString.append("(").append(gameid).append(") show").append("\n");
    PsString.append("10 30 moveto").append("\n");
    PsString.append("(").append(gamver).append(") show").append("\n");
    PsString.append("/Times-Roman findfont 8 scalefont setfont").append("\n");
    PsString.append("10 40 moveto").append("\n");
    PsString.append("(").append(date).append(") show").append("\n");
    PsString.append(" ").append("\n");
    PsString.append("gsave").append("\n");
    PsString.append("14 80 translate").append("\n");
    PsString.append(LogoString);
    PsString.append(" ").append("\n");
    PsString.append("showpage").append("\n");

    StdStringPs = PsString.toStdString();
    std::ofstream psfile ("/home/vmuser/sdmlabel.ps", std::ios::trunc);
    if (psfile.is_open()){
        psfile << StdStringPs;
        psfile.close();
        retval = true;
    }else{
        qDebug() << "Unable to open file";
        retval = false;
    }

    return retval;
}

bool Printer::WritePsGame(QString _gamename, QString _gamever, QString _date){
    bool retval;
    std::string StdStringPs;
    QString PsString;
    QString LogoString = AddLogo();
    PsString = "";
    PsString.append("%!PS-Adobe-3.0").append("\n");
    PsString.append("%cupsJobTicket: media=custom_17.02x53.85mm_17.02x53.85mm sides=one-sided").append("\n");
    PsString.append("/Times-Roman findfont 10 scalefont setfont").append("\n");
    PsString.append("15 10 moveto 90 rotate ").append("(").append(_gamename).append(") show 90 neg rotate").append("\n");
    PsString.append("25 10 moveto 90 rotate ").append("(").append(_gamever).append(") show 90 neg rotate").append("\n");
    PsString.append("35 10 moveto 90 rotate ").append("(").append(_date).append(") show 90 neg rotate").append("\n");
    PsString.append(" ").append("\n");
    PsString.append("gsave").append("\n");
    PsString.append("37 90 translate").append("\n");
    PsString.append("90 rotate").append("\n");
    PsString.append(LogoString);
    PsString.append(" ").append("\n");
    PsString.append("showpage").append("\n");

    StdStringPs = PsString.toStdString();
    std::ofstream psfile ("/home/vmuser/sdmlabel.ps", std::ios::trunc);
    if (psfile.is_open()){
        psfile << StdStringPs;
        psfile.close();
        retval = true;
    }else{
        qDebug() << "Unable to open file";
        retval = false;
    }

    return retval;
}

bool Printer::WritePsKit(QString _gamename, QString _gamever, QString _date){
    bool retval;
    std::string StdStringPs;
    QString PsString;
    PsString = "";
    PsString.append("%!PS-Adobe-3.0").append("\n");
    PsString.append("%cupsJobTicket: media=custom_17.02x53.85mm_17.02x53.85mm sides=one-sided").append("\n");
    PsString.append("/Times-Roman findfont 10 scalefont setfont").append("\n");
    PsString.append("15 10 moveto 90 rotate ").append("(").append(_gamename).append(") show 90 neg rotate").append("\n");
    PsString.append("25 10 moveto 90 rotate ").append("(").append(_gamever).append(") show 90 neg rotate").append("\n");
    PsString.append("35 10 moveto 90 rotate ").append("(").append(_date).append(") show 90 neg rotate").append("\n");
    PsString.append("showpage").append("\n");

    StdStringPs = PsString.toStdString();
    std::ofstream psfile ("/home/vmuser/sdmlabel.ps", std::ios::trunc);
    if (psfile.is_open()){
        psfile << StdStringPs;
        psfile.close();
        retval = true;
    }else{
        qDebug() << "Unable to open file";
        retval = false;
    }

    return retval;
}

/**
 * @brief Printer::PrintPs
 * @return QString error or complete return value
 */
QString Printer::PrintPs(){
    PrinterSelected = ReadPrintConf();
    int job_id = 0;
    int num_options = 0;
    cups_option_t *options = NULL;
    QString ReturnString;
    std::string strto;

    //media=custom_17.02x53.85mm_17.02x53.85mm sides=one-sided

    num_options = cupsAddOption(CUPS_SIDES, CUPS_SIDES_ONE_SIDED, num_options, &options);
    num_options = cupsAddOption(CUPS_ORIENTATION, CUPS_ORIENTATION_PORTRAIT, num_options, &options);
    num_options = cupsAddOption(CUPS_MEDIA, "custom_17x54mm_17x54mm" , num_options, &options);
    strto = PrinterSelected.toStdString();
    const char *c = strto.c_str();
    job_id = cupsPrintFile(c, "/home/vmuser/sdmlabel.ps",
                            "Print desde QL820", num_options, options);
    qDebug() << job_id;
    if (job_id == 0){
      qDebug() << cupsLastErrorString();
      ReturnString = cupsLastErrorString();
    }else{
        qDebug() << "Complete";
        ReturnString = "complete";
    }

    return ReturnString;
}


QString Printer::ReadPrintConf(){
    QString lineparsed;
    std::string line;
    std::ifstream conffile ("/home/vmuser/prntconf.txt");
    if(conffile.is_open()){
        while(std::getline(conffile,line)){
            lineparsed = line.c_str();
        }
        conffile.close();
    }else lineparsed = "error";

    return lineparsed;
}

QString Printer::AddLogo(){
    QFile file("/home/vmuser/snakelogops.txt");
    QString line;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line.append(stream.readLine()+"\n");
        }
    }else line = "error";

    file.close();

    return line;
}

