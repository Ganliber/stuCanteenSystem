#ifndef DIALOGOPENACCOUNT_H
#define DIALOGOPENACCOUNT_H

#include <QDialog>

namespace Ui {
class DialogOpenAccount;
}

class DialogOpenAccount : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOpenAccount(QWidget *parent = 0);
    ~DialogOpenAccount();

    QString stuNumber;
    QString stuName;

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogOpenAccount *ui;
};

#endif // DIALOGOPENACCOUNT_H
