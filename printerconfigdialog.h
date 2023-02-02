#ifndef PRINTERCONFIGDIALOG_H
#define PRINTERCONFIGDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace Ui {
class PrinterConfigDialog;
}

class PrinterConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrinterConfigDialog(QWidget *parent = 0);
    ~PrinterConfigDialog();


private slots:
    void on_change_pushButton_clicked();
    void on_search_pushButton_clicked();

    void on_close_pushButton_clicked();

private:
    Ui::PrinterConfigDialog *ui;
    int SearchPrinters();
    void UpdateConf();
    QString ReadPrintConf();
    bool WriteConfig();
    bool ShowConfig();
    std::string GetStdoutFromCommand(std::string cmd);
    std::vector<std::string> split_string(std::string str, std::string delimiter);
};

#endif // PRINTERCONFIGDIALOG_H
