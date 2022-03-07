


#ifndef OPERATIONLOG_H
#define OPERATIONLOG_H

#include <QDateTime>


class OperationLog
{
public:
    OperationLog();

    OperationLog(int cIndex, int nLocation, bool opRes, QDateTime opDateTime, QString sName,
        QString sNumber, QString cNumber,QString wNumber,double bB, double bC, int opT, int opE);

    bool operator <(const OperationLog &log) const;//重载运算符 <


    //标志变量

        int classIndex;//1,2,3->定位到相应的tableView(card1,canteen2,batch3)

        int nameLocation;//0~8->定位到相应的操作名

    //公共变量

        QString stuName;//学生姓名

        QString stuNumber;//学生学号

        QDateTime opTime;//操作时间

        bool opResult;//操作结果

        QStringList opName;//操作名

    //半公共

        QString canteenNumber;//消费卡号

        QString windowNumber;//消费窗口号

    //私类变量

        /*************************************************
         * 成员属性(一)
         *   ***内容(列项分析):
         *      时间,操作名,姓名,学号,涉及卡号,结果,当前余额("形式：369.3(+20.0)")--->7列
         * 开户(0)：
         *   时间，开户，输入信息（学号，姓名），成功/失败
         *
         * 销户(1)：
         *   时间，销户，销户学生（学号，姓名），成功/失败
         *
         * 发卡(2)：
         *   时间，发卡，学号，姓名，卡号，成功/失败
         *
         * 挂失(3)/解挂(4)：
         *   挂失/解挂，卡号，学号，姓名，成功/失败
         *
         * 补卡(5)：
         *   操作功能（补卡），新卡卡号，学号，姓名，成功/失败
         *
         * 充值(6)：
         *   时间,充值，学号，姓名，充值后余额，成功/失败
         *
         * 成员属性(二)
         *   ***内容(列项分析):
         *      时间,操作名,消费窗口号,卡号,余额变化,当前余额,结果 --->7列
         * 消费(7)：
         *   时间，消费，卡号，消费前余额,余额变化，消费后余额,成功/失败
         *
         * 成员属性(三)
         *   ***批量操作本身：
         *      操作命令，有多少项数据，成功项数，异常项数 ---> 4列
         *************************************************/

        double balanceBefore;//余额

        double balanceChange;//余额变化量, balanceChange=bB+bC

        int opTotalNumber;//总个数

        int opExceptionNumber;//异常个数, int opSuccessNumber;成功个数=opT-opE
};

#endif // OPERATIONLOG_H
