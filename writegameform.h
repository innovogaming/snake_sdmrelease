#ifndef WRITEGAMEFORM_H
#define WRITEGAMEFORM_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QProgressBar>

#include "conexiones.h"
#include "printer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

namespace Ui {
class WriteGameForm;
}

class WriteGameForm : public QWidget
{
    Q_OBJECT

public:
    explicit WriteGameForm(QWidget *parent = 0);
    ~WriteGameForm();

    conexiones  conn;
    void        dbConnectionError(QString dblasterr);
    void        dbQueryError(QString dblasterr);

    void        setDescription();
    void        setGameList();

    QStringList getGameVerList(QString GameName);
    void        setGameVerList();

    int         getGameId(QString GameName);

    std::string GetStdoutFromCommand(std::string cmd);
    void        executeCommandNoReturn(std::string cmd);

    void        setDrives();
    QString     getDrives();

    void        makeIsoDisabled();
    void        makeIsoEnabled();

    void        LogAction(QString action, int UserId, QString TerminalIp_ , QString VmId_);

    void        ProgressBarHide();
    void        ProgressBarShow();
    void        ProgressBarChange(int i);
    void        ProgressBarReset();

public slots:
    void slot_previousIndex(int previousIndex);
    void slot_versionChange();

private slots:
    void on_button_Makeiso_clicked();
    void on_button_UpdateDrive_clicked();

    void on_button_PrintGameLabel_clicked();

private:
    Ui::WriteGameForm *ui;
    QString GameName;
    QString GamePath;
    QString GameVer;
    QString BasePath;
    QString ImagePath;
    QString DriveSelected;

    Printer printer;

    QString GameVersionToPrint;
    QString GameNameToPrint;
    QString DayMonthYearToPrint;
};

#endif // WRITEGAMEFORM_H
