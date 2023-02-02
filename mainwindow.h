#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QLabel>
#include <QWidget>

#include "writegametinkerform.h"
#include "writegameform.h"
#include "writedongleform.h"
#include "readdongleform.h"
#include "contactodialog.h"
#include "printerconfigdialog.h"
#include "printkitdialog.h"
#include "writetinkerform.h"
#include "readtinkerform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool SubWindowActive;
    int ServerLoaded;

    bool getSubWindowActive(){ return SubWindowActive; }
    void setSubWindowActive(bool SubWindowAct){ SubWindowActive = SubWindowAct; }

private slots:
    void on_actionSalir_triggered();

    void on_actionGrabar_Juego_triggered();

    void on_actionGrabar_Dongle_triggered();

    void on_actionLeer_Dongle_triggered();

    void on_actionContacto_triggered();

    void on_actionSeleccionar_Impresora_triggered();

    void on_actionEtiqueta_de_kit_triggered();

    void on_actionGrabar_TinkerBoard_triggered();

    void on_actionLeer_TinkerBoard_triggered();

    void on_actionGrabar_Juego_Tinker_triggered();

private:
    Ui::MainWindow *ui;

    void loadSubWIndow(QWidget *widget);
    void addDatabaseServerText();

};

#endif // MAINWINDOW_H
