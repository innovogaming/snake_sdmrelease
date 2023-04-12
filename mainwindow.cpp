#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->mdiArea);
    addDatabaseServerText();

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_actionSalir_triggered
 */
void MainWindow::on_actionSalir_triggered()
{
    this->close();
}

/**
 * @brief MainWindow::on_actionGrabar_Juego_triggered
 */
void MainWindow::on_actionGrabar_Juego_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new WriteGameForm(this));
    }else{
        loadSubWIndow(new WriteGameForm(this));
    }
}

/**
 * @brief MainWindow::on_actionGrabar_Dongle_triggered
 */
void MainWindow::on_actionGrabar_Dongle_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new WriteDongleForm(this));
    }else{
        loadSubWIndow(new WriteDongleForm(this));
    }
}

/**
 * @brief MainWindow::on_actionLeer_Dongle_triggered
 */
void MainWindow::on_actionLeer_Dongle_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new ReadDongleForm(this));
    }else{
        loadSubWIndow(new ReadDongleForm(this));
    }
}

/**
 * @brief MainWindow::on_actionContacto_triggered
 */
void MainWindow::on_actionContacto_triggered()
{
    QWidget *widget = new ContactoDialog(this);
    widget->show();
}

/**
 * @brief MainWindow::on_actionSeleccionar_Impresora_triggered
 */
void MainWindow::on_actionSeleccionar_Impresora_triggered()
{
    QWidget *widget = new PrinterConfigDialog(this);
    widget->show();
}

/**
 * @brief MainWindow::on_actionEtiqueta_de_kit_triggered
 */
void MainWindow::on_actionEtiqueta_de_kit_triggered()
{
    QWidget *widget = new PrintKitDialog(this);
    widget->show();
}

void MainWindow::on_actionGrabar_TinkerBoard_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new WriteTinkerForm(this));
    }else{
        loadSubWIndow(new WriteTinkerForm(this));
    }
}

void MainWindow::on_actionLeer_TinkerBoard_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new ReadTinkerForm(this));
    }else{
        loadSubWIndow(new ReadTinkerForm(this));
    }
}

void MainWindow::on_actionGrabar_Juego_Tinker_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new WriteGameTinkerForm(this));
    }else{
        loadSubWIndow(new WriteGameTinkerForm(this));
    }
}

void MainWindow::on_actionGrabar_Firmware_PiPico_triggered()
{
    if(getSubWindowActive()){
        ui->mdiArea->closeAllSubWindows();
        loadSubWIndow(new WritePiPicoForm(this));
    }else{
        loadSubWIndow(new WritePiPicoForm(this));
    }
}


/*
 * ****************************************************************************
 * ****************************************************************************
*/

void MainWindow::loadSubWIndow(QWidget *widget){
    auto window = ui->mdiArea->addSubWindow(widget);
    window->setWindowTitle(widget->windowTitle());
    window->setContextMenuPolicy(Qt::NoContextMenu);
    window->setWindowFlags(Qt::CustomizeWindowHint);
    window->showMaximized();
    window->show();
    setSubWindowActive(true);
}

void MainWindow::addDatabaseServerText(){
    QString servertext;
    int i = ServerSelected;
    switch (i) {
    case 1:
        servertext = "Calabaza Chile";
        break;
    case 2:
        servertext = "Gecko Brasil";
        break;
    case 3:
        servertext = "Jabulani";
        break;
    }

    QLabel *label = new QLabel;
    label->setText("Conectado al servidor: "+ servertext);
    ui->statusBar->addPermanentWidget(label);
}
