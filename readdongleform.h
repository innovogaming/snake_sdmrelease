#ifndef READDONGLEFORM_H
#define READDONGLEFORM_H

#include <QWidget>
#include <QStandardItem>
#include <QMessageBox>

#include <QTextEdit>
#include "dongle.h"
#include "conexiones.h"
#include "printer.h"

namespace Ui {
class ReadDongleForm;
}

class ReadDongleForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReadDongleForm(QWidget *parent = 0);
    ~ReadDongleForm();

private slots:
    void on_button_PrintLabel_clicked();
    void on_button_LeerDongle_clicked();

private:
    Ui::ReadDongleForm *ui;

    void getDongleData();
    Dongle      dongle;
    conexiones  conn;
    Printer     printer;

    QString GameVersionToPrint;
    QString MachineIdToPrint;
    QString GameIdToPrint;
    QString DayMonthYearToPrint;

    QString Get_GameName_by_Id(int gameId);
    std::string GetStdoutFromCommand(std::string cmd);

    void dbConnectionError(QString dblasterr);
    void dbQueryError(QString dblasterr);
    void readingDongleEnabled();
    void readingDongleDisabled();
};

#endif // READDONGLEFORM_H
