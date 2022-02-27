#include "dialogrecharge.h"
#include "ui_dialogrecharge.h"
#include <QMessageBox>
#include <QWidget>
#include <QCloseEvent>

DialogRecharge::DialogRecharge(QWidget *parent,qreal balance) :
    QDialog(parent),
    ui(new Ui::DialogRecharge)
{
    ui->setupUi(this);

    //获取主窗口内的当前余额信息
    this->tempBalance = balance;

    //设置余额
    ui->lbl_balance->setText(QString("%1").arg(balance));//设置余额

    //设置金额输入框
    ui->dsb_recharge->setValue(0.0);//初始化

    //设置窗口
    this->setWindowTitle("校园卡充值");
}

DialogRecharge::~DialogRecharge()
{
    delete ui;
}

void DialogRecharge::on_btn_yes_clicked()
{
    qreal money = ui->dsb_recharge->value();//获取框内所显示值
    if(money<0)
    {
        QMessageBox::warning(this,QString("警告"),QString("充值金额不可为负!"),
                             QMessageBox::Ok);
    }
    else if(money+this->tempBalance>999.9)
    {
        //超出上限
        QMessageBox::warning(this,QString("警告"),QString("充值金额超出限额!"),
                             QMessageBox::Ok);
    }
    else
    {
        //充值成功
        QMessageBox::information(this,QString("提示"),QString("充值成功!"),
                                 QMessageBox::Ok);
        this->tempBalance += money;//更新目前余额
        ui->lbl_balance->setText(QString("%1").arg(this->tempBalance));//更新显示
        ui->dsb_recharge->setValue(0.0);//清零
        this->close();//关闭窗口
    }
}

//重写关闭窗口事件
void DialogRecharge::closeEvent(QCloseEvent *event)
{
    //发射信号,传递参数时信号函数参数与槽函数参数一一对应或者信号函数参数数目多余槽函数,多余的被抛弃
    emit updateTempBalance(this->tempBalance);
}

void DialogRecharge::on_btn_no_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(this,QString("取消充值"),
                                QString("确认取消充值?"),QMessageBox::Yes|
                                                   QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();//关闭窗口
    }
}
