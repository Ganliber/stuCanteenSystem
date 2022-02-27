#ifndef RECTANGLEPUSHBUTTON_H
#define RECTANGLEPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>

class RectanglePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RectanglePushButton(QWidget *parent = nullptr);

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

    //重写按钮按下和释放事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

signals:

public slots:
};

#endif // RECTANGLEPUSHBUTTON_H
