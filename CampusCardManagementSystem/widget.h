#ifndef WIDGET_H
#define WIDGET_H

#include <QTimer>
#include <QWidget>
#include <QObject>
#include <QMessageBox>
#include <QDateTime>
#include <QTableView>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include "campuscard.h"
#include "campuscardmanagement.h"
#include "canteenwindow.h"
#include "canteenmanagement.h"
#include "operationlog.h"

#include "dialogrecharge.h"
#include "dialogconsumption.h"
#include "dialogsetdatetime.h"
#include "mypushbutton.h"
#include "mytoolbutton.h"

#define FixedColumnNumber 4

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void setToolButtonStyle();
    ~Widget();

    //成员属性 1 : 全局时间
        QDateTime *globalDateTime;//全局时间
        QTimer *timer;//计时器:可开始可终止

    //成员属性 2 (Key) : 数据管理对象
        CampusCardManagement *cardManage;//校园卡数据管理对象
        CanteenManagement *canteenManage;//食堂数据管理对象
        QVector<OperationLog> globalBatchLog;//批量操作日志

    //成员属性 3 : 校园卡
        QStandardItemModel  *theModel;//校园卡数据模型
        QItemSelectionModel *theSelection;//校园卡Item选择模型

    //成员属性 4 : 食堂信息
        QStandardItemModel *theModelCanteen;//食堂窗口单元模型
        QItemSelectionModel *theSelectionCanteen;//食堂窗口

    //成员属性 5 : 食堂窗口操作日志
        QStandardItemModel *theModelWindow;//单个窗口消费日志
        QItemSelectionModel *theSelectionWindow;//单个窗口选择模型

    //成员属性 6 : 操作日志(3种)
        //校园卡操作日志
        QStandardItemModel *theModelLogCard;
        QItemSelectionModel *theSelectionLogCard;

        //食堂消费操作日志
        QStandardItemModel *theModelLogCanteen;
        QItemSelectionModel *theSelectionLogCanteen;

        //批量操作日志
        QStandardItemModel *theModelLogBatch;
        QItemSelectionModel *theSelectionLogBatch;

    //成员属性 7 : 批量操作标志
        //防止先出发后面的时间点
        int sequenceNumber;

    //TableView 样式

        //共用样式代码
        void commonStyleCodeOfQTableView(QTableView *tbv);

        //校园卡信息
        void setStudentTableViewStyle();

        //食堂信息
        void setCanteenListStyle();

        //单个窗口消费信息
        void setCanteenSingleWindowStyle();

        //校园卡操作日志
        void setStudentLog();

        //食堂消费总日志
        void setCanteenLog();

        //批量操作总日志
        void setBatchLog();

    //设置左侧边栏图标
    void setLeftToolButtonStyle();

    //设置右上角图标
    void setRightPushButtonStyle();

    //设置图片圆角显示
    QPixmap getRoundRectPixmap(QPixmap srcPixMap, const QSize & size, int radius);

    //设置page_card
    void setPageCardContent();

    //设置page_canteen
    void setPageCanteenContent();

    //设置page_batch
    void setPageBatchContent();

    //设置搜索框
    void setSearchBox();

    //设置groupbox无边框
    void setGroupBoxNoBorder();

    //批量数据

        //响应批量开户
        bool openFileStudentAccount(QString fileName);

        //响应批量导入食堂窗口
        bool openFileCanteenWindow(QString fileName);

        //批量处理卡片数据
        bool openFileCard(QDateTime begin,QDateTime end);

        //批量处理消费数据
        bool openFileConsumption(QDateTime begin,QDateTime end);

signals:
    //发送余额信息
    void sendBalance(qreal balance);

public slots:
    //传参设置充值槽函数
    void getRechargeBalance(qreal tempBalance);

    //传参设置时间槽函数
    void mysetDateTime(QDateTime *newDateTime);

    //传参设置消费槽函数
    void myConsumption(bool closeResult,QString cardNumber,
                       QDateTime dt,qreal amount,bool res);

    //传参设置查询余额槽函数
    void myQueryBalance(QString cardNumber);

private slots:
    //批量开户槽函数
    void on_tbtn_batch_open_clicked();

    //批量发卡槽函数
    void on_tbtn_distribute_clicked();

    //当前所选单元格发生变化

        //校园卡
        void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);

        //食堂信息
        void on_canteenCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

    //挂失按钮槽函数
    void on_btn_loss_clicked();

    //解挂按钮槽函数
    void on_btn_uncouple_clicked();

    //补卡按钮槽函数
    void on_btn_reissue_clicked();

    //注销按钮槽函数
    void on_tbtn_delete_clicked();

    //充值按钮槽函数
    void on_btn_recharge_clicked();

    //查询框槽函数
    void stu_search();

    //设置时间交互页面槽函数
    void on_btn_setDateTime_clicked();

    //设置导入食堂信息槽函数
    void on_btn_winInfo_clicked();

    //设置时间流转模式为当前日期时间槽函数
    void on_btn_getCurrentTime_clicked();

    //设置消费按钮槽函数
    void on_btn_consumption_clicked();

    void on_btn_card_rec1_clicked();

private:
    //初始化 Model

        //Basic Information

            //Student Card
            void iniModelFromCardManagement();

            //Canteen Information
            void iniModelFromCanteenManagement();

        //Log

            //Single Canteen Window Log
            void iniModelFromSingleCanteenWindowLog();

            //Total Student Card Log
            void iniModelFromTotalCardLog();

            //Total Recharge And Consumption Log
            void iniModelFromTotalConsumeLog();

            //Total Batch Log
            void iniModelFromTotalBatchLog();


    //更新gbx_info所显示的信息
    void updateInfoOnGroupboxInfo(int curRow);

    //更新时间
    void updateDateTimeOnLabel();

    //充值对话框(自定义)
    DialogRecharge *dlg_recharge = NULL;

    //消费对话框(自定义)
    DialogConsumption *dlg_consume = NULL;

    //设置全局时间对话框(自定义)
    DialogsetDateTime *dlg_datetime = NULL;

    Ui::Widget *ui;
};

#endif // WIDGET_H
