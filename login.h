#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include "conncalabazachile.h"
#include "conngeckobrasil.h"
#include "connjabulani.h"

namespace Ui {
class login;
}

extern int ServerSelected;
extern int UserId;
extern QString TerminalIp;
extern QString VmId;

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

    ConnCalabazaChile   connCala;
    ConnGeckoBrasil     connGeckoBrasil;
    ConnJabulani        connJabulani;
    bool                LoginSuccessful;

    void        dbConnectionError(QString err);
    void        dbQueryError(QString err);
    void        setServers();

private slots:
    void        on_LoginPushButton_clicked();
    void        openMainWindow();
    void        InitCheck();

signals:
    void        canceled();

private:
    Ui::login *ui;
    std::string GetStdOutFromCommand(std::string cmd);
    QString     GetTerminalIp();
    void        LogActionCala(QString action, int UserId, QString TerminalIp_ , QString VmId_);
    void        LogActionGeckoBrasil(QString action, int UserId, QString TerminalIp_ , QString VmId_);
    void        LogActionJabulani(QString action, int UserId, QString TerminalIp_ , QString VmId_);

    void        InsertNewHardSerialVm(QString hardserial);
    bool        AuthCheckGeckoChile(int idlogin);
    bool        AuthCheckGeckoBrasil(int idlogin);
    bool        HardSerialCheck();
    QString     GetHardSerialVm();

};

#endif // LOGIN_H
