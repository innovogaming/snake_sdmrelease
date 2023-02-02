#ifndef READTINKERFORM_H
#define READTINKERFORM_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QFileSystemWatcher>

#include <iostream>
#include "cryptoauthlib.h"
#include "buspirate.h"
#include "i2c.h"
#include "fram.h"
#include "storage.h"
#include "printer.h"
#include "rtc.h"
#include "tinkerinterface.h"

//#define _DS3231_ADDR_PREFIX 0xD0
//#define _DS3231_ADDRESS 0x68

namespace Fram = Tinker_Fram;
namespace Storage = Tinker_Storage;
namespace Rtc = Tinker_RTC;
using namespace std;


namespace Ui {
class ReadTinkerForm;
}

class ReadTinkerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReadTinkerForm(QWidget *parent = 0);
    ~ReadTinkerForm();

    static void     read_config(ATCAIfaceCfg *);
    static void     gen_random(ATCAIfaceCfg *);

    SystemState     systemState;
    //SystemStateOld  systemStateOld;



private slots:
    void on_Read_Button_clicked();

    void on_PrintLabel_Button_clicked();
    void on_WriteRtc_Button_clicked();

private:
    Ui::ReadTinkerForm *ui;

    //Storage::Class gameStorage;
    //Storage::Class systemStorage;

    unsigned char   buffer[512];
    uint64_t        sequencer;
    uint64_t        sequencer2;

    QString         MachineIdToPrint;
    QString         GameIdToPrint;
    QString         DayMonthYearToPrint;
    int             rtc();
    int             cripto();
    int             verify();
    uint8_t         Check_Sum_Param(uint8_t * datas, uint8_t len,uint8_t idx);
    unsigned long   str_to_long(uint8_t *value);
    int             Get_Vigencia_From_Tinker(uint8_t file10[64], uint64_t & date_of_reg);
    int             read();
    int             eraseTMSP ( DateTime nowDate );
    //bool            getTime( tm & t, bool rtcWrite);
    //int             setTime();
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
    Printer         printer;
};

#endif // READTINKERFORM_H
