#ifndef DIALOGCONSUMPTION_H
#define DIALOGCONSUMPTION_H

#include <QDialog>
#include <QDateTime>
#include <QTime>
#include <QLineEdit>
#include <QString>
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>

namespace Ui {
class DialogConsumption;
}

class DialogConsumption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConsumption(QDateTime dt,QWidget *parent = 0);//构造函数

    ~DialogConsumption();//析构函数

    void updateBalance();//更新下方标签

    qreal acc;//当前累计消费金额

signals:

    //与主窗口交互的公共信号:closeResult为此次消费是否有效,res为此次消费是否成功,均有默认参数
    void dlgSetConsumeInfo(bool closeResult=false,QString cardNumber=NULL,
                           QDateTime t=QDateTime::currentDateTime(),qreal amount=0,
                           bool res=false);//amount是单次消费金额

    //查询余额
    void queryAccount(QString cardNumber);

public slots:

    //接收余额槽函数
    void getAccount(qreal balance);

private slots:
    void on_pushButton_clicked();


    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DialogConsumption *ui;

    //当前日期时间
    QDateTime dateTime;

    //是否设置了有效时间
    bool isTimeEfficient;

    //当前查询所得余额
    qreal curBalance;

    //当前查询卡号
    QString curCardNumber;
};

#endif // DIALOGCONSUMPTION_H
