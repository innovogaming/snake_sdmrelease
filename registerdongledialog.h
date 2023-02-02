#ifndef REGISTERDONGLEDIALOG_H
#define REGISTERDONGLEDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDongleDialog;
}

class RegisterDongleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDongleDialog(QWidget *parent = 0);
    ~RegisterDongleDialog();

    QString Supplier;
    QString PassVersion;

    QString getSupplier();
    QString getPassVersion();
    void setSupplier();
    void setPassVersion();

private slots:
    void on_button_RegisterDongle_clicked();
    void slot_CurrentSupplier();

private:
    Ui::RegisterDongleDialog *ui;

signals:
    void RegisterDongleOk();
    void SupplierChanged();
};

#endif // REGISTERDONGLEDIALOG_H
