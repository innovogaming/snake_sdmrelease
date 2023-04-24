#ifndef WRITEPIPICOFORM_H
#define WRITEPIPICOFORM_H

#include "conexiones.h"
#include "printer.h"
#include "login.h"
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QMessageBox>
//#include <QFile>
//#include <QFileSystemWatcher>

using namespace std;

namespace Ui {
class WritePiPicoForm;
}

class WritePiPicoForm : public QWidget
{
    Q_OBJECT
public:
    explicit WritePiPicoForm(QWidget *parent = nullptr);
    ~WritePiPicoForm();

    void        setAplicationList();
    void        dbQueryError(QString dblasterr);
    void        dbConnectionError(QString dblasterr);
    void        setDescription();
    void        setAppVerList();
    void        LogAction(QString action, int UserId, QString TerminalIp_ , QString VmId_);
    QStringList getAppVerList(QString AppName);
    QStringList SetAppPath(int AppId, QString AppVer );
    int         getAppId(QString appname);

    conexiones  conn;
signals:

public slots:
    void slot_previousIndex(int previousIndex);
    void slot_versionChange();

private slots:
    void on_Grabar_Button_clicked();
    void on_PrintLabel_Button_clicked();

private:
    Ui::WritePiPicoForm *ui;
    std::string     GetStdoutFromCommand(std::string cmd);

    std::string Id;
    int         AppId;
    QString     AppVer;
    QString     AppName;
    QString     content;
    Printer     printer;

};

#endif // WRITEPIPICOFORM_H
