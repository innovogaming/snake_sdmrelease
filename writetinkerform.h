#ifndef WRITETINKERFORM_H
#define WRITETINKERFORM_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QFileSystemWatcher>

#include "cryptoauthlib.h"
#include "buspirate.h"
#include "i2c.h"
#include "fram.h"
#include "storage.h"
#include "rtc.h"
#include "conexiones.h"
#include "printer.h"
#include "tinkerinterface.h"

//#define _DS3231_ADDR_PREFIX 0xD0
//#define _DS3231_ADDRESS 0x68

namespace Fram = Tinker_Fram;
namespace Storage = Tinker_Storage;
namespace Rtc = Tinker_RTC;
using namespace std;


namespace Ui {
class WriteTinkerForm;
}

class WriteTinkerForm : public QWidget
{
    Q_OBJECT

public:
    explicit WriteTinkerForm(QWidget *parent = 0);
    ~WriteTinkerForm();

    static void     read_config(ATCAIfaceCfg *);
    static void     gen_random(ATCAIfaceCfg *);

    SystemState     systemState;
    //SystemStateOld  systemStateOld;

    unsigned char   buffer[512];
    uint64_t        sequencer;
    uint64_t        sequencer2;

private slots:
    void on_Grabar_Button_clicked();
    void on_PrintLabel_Button_clicked();
    void Usb0DisconnectedSlot(QString);

private:
    Ui::WriteTinkerForm *ui;


    //int             Periodo;
    QString         ToLogGameId;
    QString         ToLogMachineId;
    QString         ToLogPeriodo;
    QString         ToLogHardSerial;
    QString         MachineIdToPrint;
    QString         GameIdToPrint;
    QString         DayMonthYearToPrint;
    QString         GameName;
    QString         passVersion;
    uint64_t        keyPass;
    int             MachineId;
    int             GameId;
    int             Periodo;
    int             config;
    time_t          convertTimeRTC;
    DateTime        dateNow;


    int             cripto();
    int             verify();
    int             write(int IdGameToUse);
    //DateTime        getTime();
    //int             setTime( DateTime newDate );
    //int             ds3231_read(BP *, unsigned char, unsigned char,unsigned char *);
    //int             ds3231_write(BP *, unsigned char, unsigned char, unsigned char);
    //unsigned int    bcdTodec (unsigned int);
    //unsigned int    decTobcd (unsigned int);
    std::string     GetStdoutFromCommand(std::string cmd);
    int             FtdiLatencyModifier();
    bool            Usb0FolderExists();
    bool            Usb0StartWatch();
    bool            alreadyWatched;
    QFileSystemWatcher watcher;

    void            setValoresPeriodo();
    int             getValoresPeriodo(QString Dias);
    void            setGameList();
    int             getGameId(QString GameName);
    void            setConfigList();
    int             getConfig(QString config_id);

    conexiones      conn;
    Printer         printer;
    void            dbConnectionError(QString dblasterr);
    void            dbQueryError(QString dblasterr);
    void            LogAction(QString action, int UserId, QString TerminalIp_ , QString VmId_);

    void            ProgressBarHide();
    void            ProgressBarShow();
    void            ProgressBarChange(int i);
    void            ProgressBarReset();






};

#endif // WRITETINKERFORM_H
