#ifndef WRITEGAMETINKERFORM_H
#define WRITEGAMETINKERFORM_H

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
#include <iostream>
#include <cstring>

namespace Ui {
class WriteGameTinkerForm;
}

class WriteGameTinkerForm : public QWidget
{
    Q_OBJECT

public:
    explicit WriteGameTinkerForm(QWidget *parent = 0);
    ~WriteGameTinkerForm();

    conexiones  conn;
    void        dbConnectionError(QString dblasterr);
    void        dbQueryError(QString dblasterr);

    void        setDescription();
    void        setGameList();

    QStringList getGameVerList(QString GameName);
    void        setGameVerList();

    int         getGameId(QString GameName);
    int         getIdGameVer(QString GameName);

    QString GetStdoutFromCommand(QString cmd, double sizeCmd = 0);

    void        setDrives();
    QString     getDrives();

    QString     SetGamePath(int, QString);
    int         MakeGame(QString, QString);

    void        makeIsoDisabled(QString);
    void        makeIsoEnabled();

    void        LogAction(QString action, int UserId, QString TerminalIp_ , QString VmId_);

    void        ProgressBarHide();
    void        ProgressBarShow();
    void        ProgressBarChange(int i);
    void        ProgressBarReset();

public slots:
    void slot_gamenameChange();
    void slot_versionChange();

private slots:
    void on_Button_MakeGame_clicked();

    void on_Button_PrintGameLabel_clicked();

    void on_button_UpdateDrive_clicked();

private:
    Ui::WriteGameTinkerForm *ui;
    QString GameName;
    QString GameVer;
    QString GamePath;
    QString BasePath;
    QString ImagePath;
    int IdGame;
    //double sizeDrive;
    QString DriveSelected;

    Printer printer;

    QString GameVersionToPrint;
    QString GameNameToPrint;
    QString DayMonthYearToPrint;
};

#endif // WRITEGAMETINKERFORM_H
