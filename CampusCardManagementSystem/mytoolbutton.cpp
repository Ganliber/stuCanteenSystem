#pragma execution_character_set("utf-8")
#include "mytoolbutton.h"

//默认构造函数不要动ta！
myToolButton::myToolButton(QWidget *parent) : QToolButton(parent)
{}

//构造之后使用成员函数
void myToolButton::Initialization(QString normalImg, QString pressImg,QSize iconSize,
                                  QString text="",QSize buttonSize=QSize(128,128))
{
    //保存成员变量
    this->normalImg = normalImg;
    this->pressImg = pressImg;
    this->iconSize = iconSize;
    this->text = text;
    this->buttonSize = buttonSize;
    //初始化
    this->setNormal();
}

//未选中状态下的样式
void myToolButton::setNormal()
{
    //加载原始图片
    QPixmap pix;
    pix.load(this->normalImg);

    //固定按钮的大小
    this->setFixedSize(this->buttonSize);

    //设置图片样式: 白色背景，无边框，圆角；悬浮时灰色背景，圆角
    this->setStyleSheet("QToolButton{\
                        font:10pt;\
                        font-family:\"Microsoft YaHei\";\
                        background-color:rgb(255, 255, 255);\
                        border-width: 0;\
                        border-radius: 5px;\
                    }\
                    QToolButton:hover{\
                        background-color:#E7EBF1;\
                        border-radius:10px;\
                    }");

    //设置图标
    this->setIcon(pix);

    //设置图标大小
    this->setIconSize(this->iconSize);

    //设置文本
    this->setText(this->text);

    //设置文字在按钮下方
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}

//选中状态的样式
void myToolButton::setClicked()
{
    //加载原始图片
    QPixmap pix;
    pix.load(this->pressImg);

    //固定按钮的大小
    this->setFixedSize(this->buttonSize);

    //设置图片样式:字体蓝色 图标蓝色
    this->setStyleSheet("QToolButton{\
                        font:10pt;\
                        font-family:\"Microsoft YaHei\";\
                        color:rgb(7, 126, 254);\
                        background-color:#E7EBF1;\
                        border-width: 0;\
                        border-radius: 5px;}");

    //设置图标
    this->setIcon(pix);

    //设置文本
    this->setText(this->text);

    //设置图标大小
    this->setIconSize(this->iconSize);

    //设置文字在按钮下方
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
}
