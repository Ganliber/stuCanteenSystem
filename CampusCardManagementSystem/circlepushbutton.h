#ifndef CIRCLEPUSHBUTTON_H
#define CIRCLEPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>

class CirclePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CirclePushButton(QWidget *parent = nullptr);

    //构造之后使用成员函数
    //初始化
    void Initialization(QString normalImg, QString pressImg,QSize Iconsize,
                        QSize buttonSize,QString toolTip);
    //设置按钮风格
    void setButtonStyle(QString Img);

    //成员变量 保存用户传入的默认显示路径
    QString normalImgPath;
    QString pressImgPath;
    QSize Iconsize;
    QSize buttonSize;
    QString toolTip;

signals:

public slots:
};

#endif // CIRCLEPUSHBUTTON_H
