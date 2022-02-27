#include "dialogsetdatetime.h"
#include "ui_dialogsetdatetime.h"

DialogsetDateTime::DialogsetDateTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogsetDateTime)
{
    ui->setupUi(this);

    //设置窗口
    this->setWindowTitle("设置日期时间");

    //初始化当前时间(默认)
    this->usrSetDateTime = new QDateTime(QDateTime::currentDateTime());
}

DialogsetDateTime::~DialogsetDateTime()
{
    delete ui;
}

//重写关闭窗口事件
void DialogsetDateTime::closeEvent(QCloseEvent *event)
{
    emit dlgSetGlobalDateTime(this->usrSetDateTime);//出发信号
}

//确认
void DialogsetDateTime::on_btn_confirm_clicked()
{
    QDate qd = ui->input_dateEdit->date();
    QTime qt = ui->input_timeEdit->time();
    *this->usrSetDateTime = QDateTime(qd,qt);//更新成员属性
    QMessageBox::StandardButton result = QMessageBox::question(this,QString("取消充值"),
                                QString("确认设置全局时间为:\n")+
                                this->usrSetDateTime->toString("yyyy-M-d h:m:s:z")+"?",
                                QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();//关闭窗口
    }
}

//按钮
void DialogsetDateTime::on_btn_cancel_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(this,QString("取消充值"),
                                QString("确认取消设置时间?"),QMessageBox::Yes|
                                                   QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();//关闭窗口
    }
}
