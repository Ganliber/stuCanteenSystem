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
