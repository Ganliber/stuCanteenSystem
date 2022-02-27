#include "rectanglepushbutton.h"

RectanglePushButton::RectanglePushButton(QWidget *parent) : QPushButton(parent)
{

}

void RectanglePushButton::setButtonStyle(QString Img){
    //加载原始图片
    QPixmap pix;
    pix.load(Img);

    //固定按钮的大小
    this->resize(this->buttonSize);

    //设置图片样式
    this->setStyleSheet("QPushButton{\
                        background-color:#E3E4E8;\
                        border-width: 0;\
                        border-radius: 2px;\
                    }\
                    QPushButton:hover{\
                        background-color:#E7EBF1;\
                        border-radius:2px;\
                    }");

    //设置图标
    this->setIcon(pix);

    //设置toolTip
    setToolTip(this->toolTip);

    //设置图标大小
    this->setIconSize(this->Iconsize);
}

void RectanglePushButton::Initialization(QString normalImg, QString pressImg, QSize Iconsize,
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

void RectanglePushButton::mousePressEvent(QMouseEvent *e)
{
    //传入图片不为空
    if(this->pressImgPath!=""){
        this->setButtonStyle(this->pressImgPath);//设置Icon以及样式
    }

    return QPushButton::mousePressEvent(e);
}

void RectanglePushButton::mouseReleaseEvent(QMouseEvent *e)
{
    //更改样式
    this->setButtonStyle(this->normalImgPath);

    return QPushButton::mousePressEvent(e);
}
