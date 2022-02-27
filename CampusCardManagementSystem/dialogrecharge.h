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
    explicit DialogRecharge(QWidget *parent = 0,qreal balance = 0);
    ~DialogRecharge();

    void closeEvent(QCloseEvent *event);//重写关闭窗口事件

signals:
    void updateTempBalance(qreal balance);

private slots:
    void on_btn_yes_clicked();

    void on_btn_no_clicked();

private:
    Ui::DialogRecharge *ui;

    qreal tempBalance;
};

#endif // DIALOGRECHARGE_H
