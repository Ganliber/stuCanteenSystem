#ifndef DIALOGCONSUMPTION_H
#define DIALOGCONSUMPTION_H

#include <QDialog>
#include <QDateTime>
#include <QTime>
#include <QLineEdit>
#include <QString>
#include <QMessageBox>

namespace Ui {
class DialogConsumption;
}

class DialogConsumption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConsumption(QWidget *parent = 0);
    ~DialogConsumption();

    void closeEvent(QCloseEvent *event);//重写关闭窗口事件

    void updateBalance();//更新下方标签

signals:
    //与主窗口交互的公共信号:closeResult为此次消费是否有效,res为此次消费是否成功,均有默认参数
    void dlgSetConsumeInfo(bool closeResult=false,QString cardNumber=NULL,
                           QTime t=QTime::currentTime(),qreal amount=0,
                           bool res=false);

    //查询余额
    void queryAccount(QString cardNumber);

public slots:

    //接收余额槽函数
    void getAccount(qreal balance);

private slots:
    void on_pushButton_clicked();


private:
    Ui::DialogConsumption *ui;

    //当前查询所得余额
    qreal curBalance;

    //当前查询卡号
    QString curCardNumber;
};

#endif // DIALOGCONSUMPTION_H
