#include "contactodialog.h"
#include "ui_contactodialog.h"

ContactoDialog::ContactoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactoDialog)
{
    ui->setupUi(this);
}

ContactoDialog::~ContactoDialog()
{
    delete ui;
}

void ContactoDialog::on_pushButton_clicked()
{
    this->close();
}
