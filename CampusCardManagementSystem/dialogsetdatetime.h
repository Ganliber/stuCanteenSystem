#ifndef DIALOGSETDATETIME_H
#define DIALOGSETDATETIME_H

#include <QDialog>
#include <QMessageBox>
#include <QDateTime>


namespace Ui {
class DialogsetDateTime;
}

class DialogsetDateTime : public QDialog
{
    Q_OBJECT

public:
    explicit DialogsetDateTime(QWidget *parent = 0);
    ~DialogsetDateTime();

    void closeEvent(QCloseEvent *event);//重写关闭窗口事件

signals:
    void dlgSetGlobalDateTime(QDateTime *dt);//公共信号

private slots:
    void on_btn_confirm_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::DialogsetDateTime *ui;

    QDateTime *usrSetDateTime;
};

#endif // DIALOGSETDATETIME_H
