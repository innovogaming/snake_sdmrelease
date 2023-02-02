#ifndef DELETEDONGLEDIALOG_H
#define DELETEDONGLEDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteDongleDialog;
}

class DeleteDongleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteDongleDialog(QWidget *parent = 0);
    ~DeleteDongleDialog();

    QString Supplier;
    QString PassVersion;

    QString getSupplier();
    QString getPassVersion();
    void setSupplier();
    void setPassVersion();

private slots:
    void on_button_DeleteDongle_clicked();
    void slot_CurrentSupplier();

private:
    Ui::DeleteDongleDialog *ui;

signals:
    void DeleteDongleOk();
    void SupplierChanged();
};

#endif // DELETEDONGLEDIALOG_H
