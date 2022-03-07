#ifndef CANTEENWINDOW_H
#define CANTEENWINDOW_H

#include <QDateTime>
#include <QString>
#include <QPair>
#include <QVector>
#include <QList>
#include <QDate>
#include <QTime>
#include <QDebug>

#include "operationlog.h"

# define MaxConsumeRecordsNumber 3000  //最多6w条消息记录

class CanteenWindow
{
public:
    CanteenWindow();

    CanteenWindow(QString wN,QString lN);

    //消费
    void consume(bool res,QString cardNumber,QDateTime conDt,double balBefore,double amount);

    //添加消费日志
    void addConsumptionLog(OperationLog *log);

    //窗口本身性质
        QString winNumber;//窗口号

        QString locationNumber;//记录位置号

        QVector<OperationLog> tempLog;//当前日志(先用链表存储,如果超过60000条则)
                                    //QList存储原因(不错的API):removeFirst(),removeLast(),toVector()
private:

    //消费性质

        QDate CurrentDate;//当前消费日期(可设置)

        QTime CurrentTime;//当前消费时间(可设置)

};

#endif // CANTEENWINDOW_H
