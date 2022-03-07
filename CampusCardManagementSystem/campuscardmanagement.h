#ifndef CAMPUSCARDMANAGEMENT_H
#define CAMPUSCARDMANAGEMENT_H
#include "campuscard.h"
#include <QVector>
#include <QList>
#include <QPair>
#include "operationlog.h"
#include <QStringList>

static const quint32 MAX_CARD_NUM = 1e4+10;//支持一万人的校园卡管理

class CampusCardManagement
{
public:
    CampusCardManagement();

    /*成员属性*/

        //存储管理校园卡的向量
        QVector<CampusCard> campusCardList;

        //从学号到当前卡号于QVector中索引的映射,换卡时注意
        QMap<QString,int> mapStuNumberToCardNumber;

        //消费卡号到QVector的映射,QPair<int,bool>:first->QVector索引,second->是否为最新卡
        QMap<QString,QPair<int,bool>> mapCanteenNumberToCardNumber;

        //操作日志
        QVector<OperationLog> Log;

        //已经发卡的学生数量
        int stuIssuedNumber;

        //账户数,流水号从12346开始
        int stuNumber;

    /*成员函数*/

        //添加新用户
        bool addNewAccount(const QString stuNumber,const QString stuName,
                       QDate effDate);

        //销户
        bool deleteExistedAccount(QString stuNumber);

        //查询用户
        int queryCampusCard(QString stuNumber);

        //(批量)发卡
        void batch_distributeCard();

        //补卡
        bool reissueCard(QString stuNumber);

        //充值，账户余额上限为999.99元
        bool rechargeCard(QString stuNumber,double money);
};

#endif // CAMPUSCARDMANAGEMENT_H
