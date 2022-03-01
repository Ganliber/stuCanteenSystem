#include "operationlog.h"

OperationLog::OperationLog()
{

}

OperationLog::OperationLog(int cIndex,int nLocation,bool opRes,
    QDateTime opDateTime,QString sName=NULL,QString sNumber=NULL,
    QString cNumber=NULL,QString wNumber=NULL,
    qreal bB=0.0,qreal bC=0.0,int opT=0,int opE=0)
{
    //前面几个参数是必须的，后面的几个参数是可选的
    this->classIndex=cIndex;
    this->nameLocation=nLocation;
    this->opResult=opRes;
    this->opTime=opDateTime;

    //常量
    this->opName.append("开户");//0
    this->opName.append("销户");//1
    this->opName.append("发卡");//2
    this->opName.append("挂失");//3
    this->opName.append("解挂");//4
    this->opName.append("补卡");//5
    this->opName.append("充值");//6
    this->opName.append("食堂消费");//7

    //可选参数
    this->stuName=sName;
    this->stuNumber=sNumber;
    this->canteenNumber=cNumber;
    this->windowNumber=wNumber;
    this->balanceBefore=bB;
    this->balanceChange=bC;
    this->opTotalNumber=opT;
    this->opExceptionNumber=opE;
}

//运算符重载
bool OperationLog::operator <(const OperationLog &log)
{
    if(this->opTime<log.opTime)
        return true;
    else
        return false;
}
