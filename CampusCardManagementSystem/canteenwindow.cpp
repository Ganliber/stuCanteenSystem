#include "canteenwindow.h"

CanteenWindow::CanteenWindow()
{

}

CanteenWindow::CanteenWindow(QString wN, QString lN)
{
    //必须参数:窗口号和位置号
    this->winNumber=wN;
    this->locationNumber=lN;

    //其余默认
    this->CurrentDate=QDate::currentDate();
    this->CurrentTime=QTime::currentTime();
}

//消费日志
void CanteenWindow::addConsumptionLog(OperationLog *log)
{
    //添加日志
    this->tempLog.append(*log);

    //循环保存
    if(tempLog.size()>MaxConsumeRecordsNumber)
    {
        tempLog.removeFirst();//去掉头部
    }
}

//消费
void CanteenWindow::consume(bool res,QString cardNumber,QDateTime conDt,
                            qreal balBefore,qreal amount)
{
    if(res==true)
    {
        //消费:amount是负的
        OperationLog *log = new OperationLog(2,7,true,conDt,
                        NULL,NULL,cardNumber,this->winNumber,balBefore,amount,0,0);

        //添加消费日志
        this->addConsumptionLog(log);
    }
    else
    {
        //消费:amount是负的
        OperationLog *log = new OperationLog(2,7,false,conDt,
                        NULL,NULL,cardNumber,this->winNumber,balBefore,0.0,0,0);

        //添加消费日志
        this->addConsumptionLog(log);
    }
}











