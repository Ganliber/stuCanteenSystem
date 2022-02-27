#ifndef MYTOOLBUTTON_H
#define MYTOOLBUTTON_H

#include <QWidget>
#include <QToolButton>

class myToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit myToolButton(QWidget *parent = nullptr);

    //构造之后使用成员函数
    void Initialization(
            QString normalImg, QString pressImg,QSize iconSize,
            QString text,QSize buttonSize);

    //未选中状态下的样式
    void setNormal();

    //选中状态的样式
    void setClicked();

    //成员变量
    QString normalImg;
    QString pressImg;
    QSize iconSize;
    QString text;
    QSize buttonSize;

signals:

public slots:
};

#endif // MYTOOLBUTTON_H
