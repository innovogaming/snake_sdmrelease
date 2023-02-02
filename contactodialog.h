#ifndef CONTACTODIALOG_H
#define CONTACTODIALOG_H

#include <QDialog>

namespace Ui {
class ContactoDialog;
}

class ContactoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactoDialog(QWidget *parent = 0);
    ~ContactoDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ContactoDialog *ui;
};

#endif // CONTACTODIALOG_H
