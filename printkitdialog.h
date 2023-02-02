#ifndef PRINTKITDIALOG_H
#define PRINTKITDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>

#include "conexiones.h"
#include "printer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

namespace Ui {
class PrintKitDialog;
}

class PrintKitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintKitDialog(QWidget *parent = 0);
    ~PrintKitDialog();

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::PrintKitDialog *ui;

    void        setGameList();
    std::string GetStdoutFromCommand(std::string cmd);

    void        dbConnectionError(QString dblasterr);
    void        dbQueryError(QString dblasterr);

    conexiones  conn;
    Printer     printer;
    QString     GameNameToPrint;
    QString     DayMonthYearToPrint;
    QString     Details;
};

#endif // PRINTKITDIALOG_H
