#include "registerdongledialog.h"
#include "ui_registerdongledialog.h"

RegisterDongleDialog::RegisterDongleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDongleDialog)
{
    ui->setupUi(this);
    setWindowTitle("Registrar Dongle");
    connect(ui->cbox_Supplier, SIGNAL(currentIndexChanged(QString)), SLOT(slot_CurrentSupplier()));
}

RegisterDongleDialog::~RegisterDongleDialog()
{
    delete ui;
}

void RegisterDongleDialog::on_button_RegisterDongle_clicked()
{
    setPassVersion();
    setSupplier();
    emit RegisterDongleOk();
    this->close();
}

QString RegisterDongleDialog::getPassVersion(){
    return PassVersion;
}

QString RegisterDongleDialog::getSupplier(){
    return Supplier;
}

void RegisterDongleDialog::setPassVersion(){
    PassVersion = ui->cbox_PassVersion->currentText();
}

void RegisterDongleDialog::setSupplier(){
    Supplier = ui->cbox_Supplier->currentText();
}

void RegisterDongleDialog::slot_CurrentSupplier(){
    emit SupplierChanged();
}
