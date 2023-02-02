#include "deletedongledialog.h"
#include "ui_deletedongledialog.h"

/* ******************
 * Esta funcion necesita ser probada,
 * se dejo de lado por un bug que bloqueaba las dongles
 * al ser borradas. se necesitan mas pruebas
 * para dejar en release, pero la logica del codigo
 * es correcta, falta analizar los pasos y
 * probar bien el tema de que servidor se escoge al
 * momento de borrar una dongle(se supone que se debe escoger China).
 * */

DeleteDongleDialog::DeleteDongleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDongleDialog)
{
    ui->setupUi(this);
    setWindowTitle("Eliminar Dongle");
    connect(ui->cbox_Supplier, SIGNAL(currentIndexChanged(QString)), SLOT(slot_CurrentSupplier()));
}

DeleteDongleDialog::~DeleteDongleDialog()
{
    delete ui;
}

void DeleteDongleDialog::on_button_DeleteDongle_clicked()
{
    setPassVersion();
    setSupplier();
    emit DeleteDongleOk();
    this->close();
}

QString DeleteDongleDialog::getPassVersion(){
    return PassVersion;
}

QString DeleteDongleDialog::getSupplier(){
    return Supplier;
}

void DeleteDongleDialog::setPassVersion(){
    PassVersion = ui->cbox_PassVersion->currentText();
}

void DeleteDongleDialog::setSupplier(){
    Supplier = ui->cbox_Supplier->currentText();
}

void DeleteDongleDialog::slot_CurrentSupplier(){
    emit SupplierChanged();
}
