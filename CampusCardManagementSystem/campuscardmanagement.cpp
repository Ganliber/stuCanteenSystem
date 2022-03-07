#include "campuscardmanagement.h"
#include <QDateTime>

//构造函数
CampusCardManagement::CampusCardManagement()
{
    this->stuNumber = 0;
    this->stuIssuedNumber = 0;
}

//添加新用户
bool CampusCardManagement::addNewAccount(const QString stuNumber,const QString stuName,
                                         QDate effDate= QDate(2024,7,15))
{
    //添加新用户,开户的API,函数参数默认值在声明和实现中只能有一个

    //查重学号
    if(this->mapStuNumberToCardNumber.contains(stuNumber)||campusCardList.size()>1e4){
        //日志:开户失败，学号重复或容量超标
        return false;
    }

    //开户(为根据学号进行排列)
    CampusCard *newOne = new CampusCard(stuNumber,stuName,effDate);

    this->campusCardList.push_back(*newOne);//参数是引用类型

    //构造当前映射(后期操作会造成改动,此处是为了避免未发卡就销户的bug),所有操作都要更新映射关系!
    this->mapStuNumberToCardNumber[stuNumber] = this->stuNumber;

    //更新学生开户人数
    this->stuNumber += 1;

    //开户成功
    return true;
}

//查询用户
int CampusCardManagement::queryCampusCard(QString stuNumber)
{
    //用户不存在
    if(!this->mapStuNumberToCardNumber.contains(stuNumber))
        return -1;

    //查找在QVector中的索引
    int index = this->mapStuNumberToCardNumber[stuNumber];

    //返回其引用
    return index;
}

//销户
bool CampusCardManagement::deleteExistedAccount(QString stuNumber)
{
    int index = this->queryCampusCard(stuNumber);
    if(index == -1)
        return false;
    CampusCard& cc = this->campusCardList[index];
    cc.accountDelete();//销户
    return true;
}

//(批量)发卡
void CampusCardManagement::batch_distributeCard()
{
    //对当前未发卡的账户进行发卡

    //当前未发卡批次关于校园卡号从小到大顺序排序
    //std::sort(this->campusCardList.begin(),this->campusCardList.end());

    //发卡
    for(int i=0;i<this->campusCardList.size();i++){

        if(this->campusCardList[i].isDistributed==false)
        {
            //流水号
            int serialNumber = 12346 + this->stuIssuedNumber;

            //发卡
            QString canteenNumber = this->campusCardList[i].cardDistributed(serialNumber);

            //形成"消费卡号->学生位置,卡号最终状态"的映射
            this->mapCanteenNumberToCardNumber[canteenNumber]=QPair<int,bool>(i,true);

            //更新已经发卡的人数
            this->stuIssuedNumber += 1;
        }
    }
}

//补卡
bool CampusCardManagement::reissueCard(QString stuNumber){
    //补卡
    int index = this->queryCampusCard(stuNumber);

    if(index!=-1)
    {
        int serialNumber = this->stuIssuedNumber + 12346;//流水号

        //更新已经发卡
        this->stuIssuedNumber += 1;

        //补卡
        bool flag = this->campusCardList[index].cardReissue(serialNumber);


        if(this->campusCardList[index].cardNumber.isEmpty())
            return false;//判断非空

        if(flag==true){
            CampusCard &tmp = this->campusCardList[index];

            //新卡号
            QString cardNumberNew = tmp.cardNumber.last();

            //旧卡号
            QString cardNumberBefore = tmp.cardNumber[tmp.cardNumber.length()-2];

            //修改之前映射
            this->mapCanteenNumberToCardNumber[cardNumberBefore].second = false;

            //新映射
            this->mapCanteenNumberToCardNumber[cardNumberNew]=QPair<int,bool>(index,true);
        }
    }
    return false;
}

//充值
bool CampusCardManagement::rechargeCard(QString stuNumber,double money){
    //充值，账户余额上限为999.99元
    int index = this->queryCampusCard(stuNumber);

    bool flag = campusCardList[index].cardRecharge(money);

    return flag;
}
















