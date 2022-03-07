#ifndef CANTEENMANAGEMENT_H
#define CANTEENMANAGEMENT_H

#include <QVector>
#include "operationlog.h"
#include "canteenwindow.h"


class CanteenManagement
{
public:
    CanteenManagement();

    //成员属性

        //窗口数目
        int windowNumber;

        //食堂窗口列表
        QVector<CanteenWindow> CanteenWindowList;

        //总操作日志归并排序得到
        QVector<OperationLog> totalLog;

    //成员函数

        //添加新的食堂窗口
        void addNewCanteenWindow(QString wN, QString lN);

        //添加新日志
        void addNewCanteenLog(OperationLog *log);
};

#endif // CANTEENMANAGEMENT_H
