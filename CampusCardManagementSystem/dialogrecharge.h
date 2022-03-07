#ifndef DIALOGRECHARGE_H
#define DIALOGRECHARGE_H

#include <QDialog>

namespace Ui {
class DialogRecharge;
}

class DialogRecharge : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRecharge(QWidget *parent = 0,double balance = 0);
    ~DialogRecharge();

    void closeEvent(QCloseEvent *event);//重写关闭窗口事件

signals:
    void updateTempBalance(double balance);

private slots:
    void on_btn_yes_clicked();

    void on_btn_no_clicked();

private:
    Ui::DialogRecharge *ui;

    double tempBalance;
};

#endif // DIALOGRECHARGE_H
