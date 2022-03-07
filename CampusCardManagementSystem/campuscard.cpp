#include "campuscard.h"

//默认构造函数
CampusCard::CampusCard()
{}

//有参构造函数
CampusCard::CampusCard(QString stuNumber,QString stuName,
           QDate effDate=QDate(2024,7,15))
{
    //初始化开户
    this->studentNumber = stuNumber;
    this->studentName = stuName;
    this->effectiveDate = effDate;
    this->balance=0;

    //状态变量
    this->accountState = true;
    this->cardState = false;
    this->isDistributed=false;
}

//运算符重载:学号从小到大的顺序
bool CampusCard::operator <(const CampusCard &cc)
{
    //本身就全局了(friend)不需要加作用域
    if(this->studentNumber<cc.studentNumber)
        return true;
    else
        return false;
}

//销户
bool CampusCard::accountDelete()
{
    if(this->accountState==true){
        this->accountState = false;
        return true;//成功销户
    }
    return false;
}

//发卡
QString CampusCard::cardDistributed(int serialNumber)
{
    //发卡:参数为流水号
    //计算卡号的校验码X
    int temp = 3;
    int tempSerialNumber = serialNumber;//临时储存流水号

    for(int j=0;j<5;j++){
        temp += serialNumber%10;
        serialNumber /= 10;
    }

    int X = 9 - temp%10;//校验码
    serialNumber = tempSerialNumber*10+X;

    //数字转字符串
    QString newCardNumber = QString("3")+QString::number(serialNumber);

    //分配
    this->cardNumber.append(newCardNumber);//添加到卡号的list中

    //更新发卡状态
    this->isDistributed=true;

    //更新卡片状态
    this->cardState=true;

    //返回新卡号用于形成映射
    return newCardNumber;
}

//挂失
bool CampusCard::cardReportLoss()
{
    //挂失
    this->cardState = false;

    return true;
}

//解挂
bool CampusCard::cardUncouple()
{
    //解挂
    if(this->cardState==false&&this->isDistributed==true)
    {
        this->cardState = true;
        return true;//成功解挂
    }
    return false;
}

//补卡
bool CampusCard::cardReissue(int serialNumber)
{
    //补卡:参数为流水号
    if(this->isDistributed==true&&this->cardState==false)
    {
        //计算卡号的校验码X
        int temp = 3;
        int tempSerialNumber = serialNumber;//临时储存流水号
        for(int j=0;j<5;j++){
            temp += serialNumber%10;
            serialNumber /= 10;
        }
        int X = 9 - temp%10;//校验码
        serialNumber = tempSerialNumber*10+X;

        //数字转字符串
        QString newCardNumber = QString("3")+QString::number(serialNumber);

        //分配
        this->cardNumber.append(newCardNumber);//添加到卡号的list中

        //卡片状态变为正常
        this->cardState=true;

        return true;
    }

    return false;
}

//校园卡充值
bool CampusCard::cardRecharge(double money)
{
    //充值，账户余额上限为999.99元
    //可以充值
        if(this->balance+money<1000)
        {
            this->balance = this->balance + money;

            return true;//充值成功
        }
    return false;
}




