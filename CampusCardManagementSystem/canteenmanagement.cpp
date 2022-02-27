#include "canteenmanagement.h"

CanteenManagement::CanteenManagement()
{
    this->windowNumber=0;//窗口设置为0
}

void CanteenManagement::addNewCanteenWindow(QString wN, QString lN)
{
    //必须参数:窗口号和位置号
    //winNumber=wN;
    //locationNumber=lN;
    CanteenWindow *cW = new CanteenWindow(wN,lN);//新建食堂窗口
    this->CanteenWindowList.push_back(*cW);//添加
    this->windowNumber += 1;//食堂窗口数目加一
}
