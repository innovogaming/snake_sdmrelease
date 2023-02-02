#ifndef PRINTER_H
#define PRINTER_H

#include <QDebug>
#include <QFile>
#include <stdio.h>
#include <cups/cups.h>
#include <iostream>
#include <fstream>
#include <string>

class Printer
{
public:
    Printer();
    bool    WritePs(QString gamver, QString gameid, QString machid, QString date);
    bool    WritePsGame(QString _gamename, QString _gamever, QString _date);
    bool    WritePsKit(QString _gamename, QString _gamever, QString _date);
    QString PrintPs();
    QString ReadPrintConf();
    QString AddLogo();
    QString PrinterSelected;
    QString GameVersionToPrint;
    QString MachineIdToPrint;
    QString GameIdToPrint;
    QString DayMonthYearToPrint;
};

#endif // PRINTER_H
