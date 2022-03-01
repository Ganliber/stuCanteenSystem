#include "dialogconsumption.h"
#include "ui_dialogconsumption.h"

DialogConsumption::DialogConsumption(QDateTime dt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConsumption)
{
    ui->setupUi(this);

    //设置线的宽度
    ui->line->setStyleSheet("#line_left{color:#F4F5F7;}");

    //设置时间默认为当前时间
    this->dateTime=dt;

    //设置当前时间无效
    this->isTimeEfficient=false;

    //设置时间标签
    ui->lbl_curTime->setText(dt.time().toString("h时m分s秒"));

    //设置当前时段累计消费金额为0
    ui->lbl_accumulation->setText(QString("%1").arg(0));

    //当前余额
    this->curBalance=0;

    //当前累计消费金额
    this->acc=0;

    //当前卡号
    this->curCardNumber="-";//空
}

DialogConsumption::~DialogConsumption()
{
    delete ui;
}

//查询卡号
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

//设置当前消费时间
void DialogConsumption::on_pushButton_3_clicked()
{
    //得到当前消费时间
    QTime t = ui->timeEdit->time();

    //得到之前的消费时间
    QTime tBefore = this->dateTime.time();


    if((t>=QTime(7,0)&&t<QTime(9,0))||
            (t>=QTime(11,0)&&t<QTime(13,0))||
            (t>=QTime(17,0)&&t<QTime(19,0)))
    {
        //设置单次消费时间
        ui->lbl_curTime->setText(t.toString("h时m分s秒"));

        //设置当前时间
        this->dateTime.setTime(t);

        //设置当前时间的有效性
        this->isTimeEfficient=true;

        //更新当前时段的消费积累
        if(t.hour()-tBefore.hour()>=2||t.hour()-tBefore.hour()<=-2)
        {
            ui->lbl_accumulation->setText(QString("%1").arg(0));
        }
    }
    else
    {
        //弹出时间不符窗口
        QString dlgTitle = "提示";
        QString strInfo = "当前不在饭点哦~";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
}

//消费
void DialogConsumption::on_pushButton_2_clicked()
{
    //前提判断
    if(this->curCardNumber=="-")
    {
        //未确定卡号
        QString dlgTitle = "提示";
        QString strInfo = "请输入卡号！";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
        return;
    }

    //消费金额判断
    if(this->isTimeEfficient==false)
    {
        //当前时间无效
        QString dlgTitle = "提示";
        QString strInfo = "请重新确认消费时间！";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
        return;
    }

    //得到消费金额
    qreal consumeMoney = ui->doubleSpinBox->value();

    //判断是否超出余额
    if(consumeMoney>this->curBalance)
    {
        //弹出输入余额不足窗口
        QString dlgTitle = "提示";
        QString strInfo = "余额不足！";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
        emit dlgSetConsumeInfo(true,this->curCardNumber,
                               this->dateTime,0,false);//消费失败:余额不足
        return;
    }

    //判断累计消费金额是否超过20元
    if(this->acc+consumeMoney>20)
    {
        //弹出输入密码确认窗口
        QString dlgTitle = "提示";
        QString strInfo = "请输入校园卡密码";
        QString password = QInputDialog::getText(this,dlgTitle,strInfo,
                                                 QLineEdit::Password);
        if(password=="8888")
        {
            this->acc=0;//单次累计余额归零
            ui->lbl_accumulation->setText(QString("%1").arg(0));

            this->curBalance -= consumeMoney;//余额减少
            ui->lbl_balance->setText(QString("%1").arg(this->curBalance));

            emit dlgSetConsumeInfo(true,this->curCardNumber,
                                   this->dateTime,consumeMoney,true);//消费成功
        }
        else
        {
            //弹出输入密码确认窗口
            QString dlgTitle = "提示";
            QString strInfo = "密码错误！";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
    }
    else
    {
        this->acc+=consumeMoney;//加上消费的金额
        ui->lbl_accumulation->setText(QString("%1").arg(this->acc));

        this->curBalance -= consumeMoney;//余额减少
        ui->lbl_balance->setText(QString("%1").arg(this->curBalance));

        emit dlgSetConsumeInfo(true,this->curCardNumber,
                               this->dateTime,consumeMoney,true);//消费成功
    }
}
