#include "dialogopenaccount.h"
#include "ui_dialogopenaccount.h"

DialogOpenAccount::DialogOpenAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpenAccount)
{
    ui->setupUi(this);
}

DialogOpenAccount::~DialogOpenAccount()
{
    delete ui;
}

void DialogOpenAccount::on_pushButton_clicked()
{
    this->stuName = ui->ldt_name->text();

    this->stuNumber = ui->ldt_number->text();
}
