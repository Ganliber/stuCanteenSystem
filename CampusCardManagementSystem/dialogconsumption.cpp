#include "dialogconsumption.h"
#include "ui_dialogconsumption.h"

DialogConsumption::DialogConsumption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConsumption)
{
    ui->setupUi(this);

    //设置线的宽度
    ui->line->setStyleSheet("#line_left{color:#F4F5F7;}");

    //初始化
    this->curBalance=0;

    this->curCardNumber="-";//空
}

DialogConsumption::~DialogConsumption()
{
    delete ui;
}

//重写关闭窗口事件
void DialogConsumption::closeEvent(QCloseEvent *event)
{
    emit this->dlgSetConsumeInfo();//发出信号
}


void DialogConsumption::on_pushButton_clicked()
{
    QString cN = ui->lineEdit->text();//获取待查询卡号

    emit queryAccount(cN);//发出信号查询余额
}

//接收余额槽函数
void DialogConsumption::getAccount(qreal balance)
{
    if(balance==-1)
    {
        //卡不存在
        QString dlgTitle = "提示";
        QString strInfo = "当前卡号不存在!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);

    }
    else if(balance==-2)
    {
        //卡号已失效
        QString dlgTitle = "提示";
        QString strInfo = "当前卡号已失效！";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
    else if(balance==-3)
    {
        //卡号已挂失
        QString dlgTitle = "提示";
        QString strInfo = "当前卡号已挂失！";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);

    }
    else
    {
        //得到当前有效卡号
        this->curCardNumber = ui->lineEdit->text();

        //得到有效余额
        this->curBalance = balance;//得到所查询的余额

        //更新下方标签显示
        updateBalance();
    }
}

//更新下方标签
void DialogConsumption::updateBalance()
{
    ui->lbl_balance->setText(QString("%1").arg(this->curBalance));//设置余额

    ui->lbl_number->setText(this->curCardNumber);//设置当前学号
}






