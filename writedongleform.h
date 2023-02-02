#ifndef WRITEDONGLEFORM_H
#define WRITEDONGLEFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QProgressBar>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "dongle.h"
#include "conexiones.h"
#include "registerdongledialog.h"
#include "deletedongledialog.h"
#include "printer.h"

namespace Ui {
class WriteDongleForm;
}

class WriteDongleForm : public QWidget
{
    Q_OBJECT

public:
    explicit WriteDongleForm(QWidget *parent = 0);
    ~WriteDongleForm();

    QString labelMachineId;
    QString labelGameId;
    QString labelVersion;
    QString labelFecha;

    QString getlabelMachineId(){
        return labelMachineId;
    }
    QString getlabelGameId(){
        return labelGameId;
    }
    QString getlabelVersion(){
        return labelVersion;
    }
    QString getlabelFecha(){
        return labelFecha;
    }

private slots:
    void on_button_MakeDongle_clicked();
    void slot_previousIndex(int previousIndex);
    void on_button_PrintDongle_clicked();

    void on_button_RegisterDongle_clicked();
    void on_button_DeleteDongle_clicked();
    void slot_RegisterDongle();
    void slot_DeleteDongle();
    void slot_SupplierSelectedRegister();
    void slot_SupplierSelectedDelete();

private:
    Ui::WriteDongleForm *ui;
    QString     GameVersionToPrint;
    QString     MachineIdToPrint;
    QString     GameIdToPrint;
    QString     DayMonthYearToPrint;
    QString     GameName;
    QString     GameVer;
    int         gameId;

    conexiones  conn;
    Printer     printer;

    RegisterDongleDialog*   registrardongle;
    DeleteDongleDialog*     deletedongle;

    void        setGameList();
    QStringList getGameVerList(QString GameName);
    void        setGameVerList();
    int         getGameId(QString GameName);
    void        setValoresPeriodo();
    int         getValoresPeriodo(QString Dias);
    void        setSupplierListReg();
    void        setSupplierListDel();
    void        DonglePrintEnabled();
    void        DonglePrintDisabled();
    void        LogAction(QString action, int UserId, QString TerminalIp_ , QString VmId_);
    std::string GetStdoutFromCommand(std::string cmd);

    void        dbConnectionError(QString dblasterr);
    void        dbQueryError(QString dblasterr);
    void        MakeDongleDisabled();
    void        MakeDongleEnabled();
    void        SetWarningLabel(QString labelmessage);
    void        SetInfoLabel(QString labelmessage);
    void        ProgressBarHide();
    void        ProgressBarShow();
    void        ProgressBarChange(int i);
    void        ProgressBarReset();

    /*********************  Inherited Dongle variables ********************** */

    struct Street_Dongle_{
        int             MachineId;
        int             GameId;
        QString         CreationDate;
        u_int64_t       mIn;
        u_int64_t       mOut;
        u_int64_t       creditIn;
        u_int64_t       creditOut;
        u_int64_t       moneyInWhole;
        u_int64_t       moneyInCents;
        u_int64_t       moneyOutWhole;
        u_int64_t       moneyOutCents;
        int             period;
        QString         expirationDate;
        QString         lastUpdate;
        int             valid;
    };

    struct Dongle_Handle_{
        QString         HardSerial;
        QString         Supplier;
        int             Activa;
        int             Operativa;
        QString         TimeStamp;
        QString         VerPass;
        int             UserId;
        QByteArray      Pass;
        Street_Dongle_   data;
        int             valid;
    };

    struct Volatile_Data_{
        int             UserId;
        QString         MakingDongle;
        QString         CreationDate;
        int             MachineId;
        int             err;
    };

    struct Sesion_
    {
        int             id;
        QString         tipo;
        QString         nombre;
        int             user_status;
        bool            sesion_status;
        QString         error;
        unsigned int    tabs;
    };

    struct App_Data_List_{
        QStringList     GameId;
        QStringList     GameVer;
        QStringList     AppName;
        QStringList     AppId;
        QStringList     Type;
        QStringList     Attr;
        QStringList     TimeStamp;
        QStringList     UserId;
        int             count;
    };

    struct App_Data_{
        QString         GameId;
        QString         GameVer;
        QString         AppName;
        QString         AppId;
        QString         Type;
        QString         Attr;
        QString         TimeStamp;
        int             UserId;
        QByteArray      Data;
    };

    Sesion_         LocalSesion;
    Dongle          dongle;
    bool            dongleIsFree;

    void            makedongle();
    bool            Check_Dongle_Serial(QString serial);
    Dongle_Handle_  Get_Dongle_Handle(QString HardSerial);
    QString         Get_GameName_by_Id(int gameId);
    int             getGameIdFromGameName(QString gameName1);
    bool            Reserve_Volatile(Volatile_Data_ &data);
    void            Free_Request_Make_Dongle(int UserId);
    Volatile_Data_  Status_Request_Make_Dongle();
    App_Data_List_  Get_Apps_List(int GameId, QString GameVer);
    bool            Get_App(App_Data_ & App);
    QString         Get_PassVer_From_GameId_And_GameVer(int GameId, QString GameVer);
    u_int32_t       Get_GameVersion_Id(int GameId, QString GameVersion);
    bool            Update_Street_Dongle(QString Machine_Id, int GameId, QString GameVersion, QString VerPass, u_int32_t idGameVer , int Periodo);
    bool            Register_Calasys(QString MachineId, QString GameId);
    QByteArray      Get_Pass_Version(QString PassVersion);
    int             Store_New_Street_Dongle(int MachineId, int GameId,QString HardSerial,QString gameVer, u_int32_t idGameVer,QString VerPass);
    QByteArray      get_SupplierPassData(QString SupplierName,QString PassVersion);
    bool            Store_New_Dongle(QString hardSerial, QString Supplier, QString PassVersion,int UserId);


};

#endif // WRITEDONGLEFORM_H
