#ifndef CAMPUSCARD_H
#define CAMPUSCARD_H
#include <QString>
#include <QDateTime>
#include "operationlog.h"

class CampusCard
{
public:
    CampusCard();//构造函数

    CampusCard(QString stuNumber,QString stuName,QDate effDate);//开户

    bool operator <(const CampusCard &cc);//重载运算符 <

    bool accountDelete();//销户

    QString cardDistributed(int serialNumber);//发卡

    bool cardReportLoss();//挂失

    bool cardUncouple();//解挂

    bool cardReissue(int serialNumber);//补卡

    bool cardRecharge(qreal money);//充值，账户余额上限为999.99元

    /****************************************************
     * 成员属性:学号                                     *
     * 格式:2020zzxxxX                                  *
     * 格式注解:4位年度,2位专业号,3位专业内流水号,1位校验码 *
     * 特性:唯一性                                       *
     ****************************************************/
    QString studentNumber;

    /***************************************************
     * 成员属性:姓名                                    *
     * 特性:四个字以内                                  *
     ***************************************************/
    QString studentName;

    /***************************************************
     * 成员属性:有效日期                                 *
     * 格式:"yyyyMMdd",8位,可以设定统一的有效日期         *
     ***************************************************/
    QDate effectiveDate;

    /***************************************************
     * 成员属性:校园卡卡号(列表)                         *
     * 格式:"3xxxxxX",7位                               *
     * 格式注解:1位发卡点编号(默认3),5位流水号,1位校验码   *
     * 特性:只有列表的最后一个是有效的                    *
     * 获取:T &QList::last()                            *
     ***************************************************/
    QStringList cardNumber;//卡号链表,只有表尾是当前有效的卡

    bool accountState;//账户状态：false(注销),true(开户正常状态)

    bool isDistributed;//是否发卡：false(未发卡或者),true(已经发卡)

    bool cardState;//当前卡片状态：false(禁用),true(正常状态)

    qreal balance;//余额,qreal等价于double

};

#endif // CAMPUSCARD_H

