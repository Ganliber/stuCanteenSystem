#include "circlepushbutton.h"

CirclePushButton::CirclePushButton(QWidget *parent) : QPushButton(parent)
{

}
void CirclePushButton::setButtonStyle(QString Img){
    //加载原始图片
    QPixmap pix;
    pix.load(Img);

    //固定按钮的大小
    this->setFixedSize(this->buttonSize);

    //设置图标
    this->setIcon(pix);

    //设置toolTip
    setToolTip(this->toolTip);

    //设置图标大小
    this->setIconSize(this->Iconsize);

    //设置图片样式
    this->setStyleSheet("QPushButton{\
                        border-width: 4px;\
                        background-color:#F4F5F7;\
                        border-radius: 50px;\
                    }\
                    QPushButton:hover{\
                        background-color:#E3E4E8;\
                        border-radius:50px;\
                    }");
}

void CirclePushButton::Initialization(QString normalImg, QString pressImg, QSize Iconsize,
                                  QSize buttonSize,QString tooltip)
{
    this->normalImgPath = normalImg;
    this->pressImgPath = pressImg;
    this->Iconsize = Iconsize;
    this->buttonSize = buttonSize;
    this->toolTip = tooltip;
    //初始化
    this->setButtonStyle(normalImg);
}
