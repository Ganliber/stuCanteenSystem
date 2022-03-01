#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QScrollBar>
#include <QFile>
#include <QSize>
#include <QDateTime>
#include <QPainter>
#include <algorithm>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    /***************************************************
     *  function:Configure the current time            *
     ***************************************************/
    //批量操作顺序号
    this->sequenceNumber=0;

    //实时更新时间
    this->timer = new QTimer();

    //初始化 : 设置当前时间(默认全部发生:当前时间)
    this->globalDateTime=new QDateTime(QDateTime::currentDateTime());

    //更新时间
    updateDateTimeOnLabel();

    /****************************************************
     * function:Configure the basic data of the student *
     * module.                                          *
     ****************************************************/

    //管理对象定义
    this->cardManage = new CampusCardManagement();//在堆区创建校园卡管理系统对象
    this->canteenManage = new CanteenManagement();//在堆区创建食堂管理系统对象

    //信息处理模型

        //学生信息·Model和选择模型
        theModel = new QStandardItemModel(0,FixedColumnNumber,this);//数据模型
        theSelection = new QItemSelectionModel(theModel);//选择模型

        //食堂信息·Model和选择模型
        theModelCanteen = new QStandardItemModel(0,2,this);
        theSelectionCanteen = new QItemSelectionModel(theModelCanteen);

    //操作日志模型

        //单个食堂窗口消费日志·Model和选择模型:时间,卡号,余额变化,消费成功/失败
        theModelWindow = new QStandardItemModel(0,4,this);
        theSelectionWindow = new QItemSelectionModel(theModelWindow);

        //校园卡账户操作日志:时间,操作名,姓名,学号,涉及卡号,当前余额(充值金额),结果
        theModelLogCard = new QStandardItemModel(0,7,this);
        theSelectionLogCard = new QItemSelectionModel(theModelLogCard);

        //充值消费日志:时间,操作名,窗口号,卡号,余额变化,当前余额,结果
        theModelLogCanteen = new QStandardItemModel(0,7,this);
        theSelectionLogCanteen = new QItemSelectionModel(theModelLogCanteen);

        //批量操作日志:操作命令，有多少项数据，成功项数，异常项数
        theModelLogBatch = new QStandardItemModel(0,4,this);
        theSelectionLogBatch = new QItemSelectionModel(theModelLogBatch);

    //TableView与Model的结合

        //信息处理模型

            //学生信息
            ui->tableViewStudents->setModel(theModel); //设置数据模型
            ui->tableViewStudents->setSelectionModel(theSelection);//设置选择模型

            //食堂信息·Model和选择模型
            ui->tbv_win_list->setModel(theModelCanteen);
            ui->tbv_win_list->setSelectionModel(theSelectionCanteen);

        //操作日志模型

            //单个食堂窗口消费日志:时间,卡号,余额变化,消费成功/失败
            ui->tbv_canteen_logs->setModel(theModelWindow);
            ui->tbv_canteen_logs->setSelectionModel(theSelectionWindow);

            //校园卡账户操作日志:时间,操作名,姓名,学号,涉及卡号,结果
            ui->tbv_card_logs->setModel(theModelLogCard);
            ui->tbv_card_logs->setSelectionModel(theSelectionLogCard);

            //充值消费日志:时间,操作名,姓名,学号,消费窗口,卡号,余额变化,当前余额,结果
            ui->tbv_canteen_logs_total->setModel(theModelLogCanteen);
            ui->tbv_canteen_logs_total->setSelectionModel(theSelectionLogCanteen);

            //批量操作日志:操作命令，有多少项数据，成功项数，异常项数
            ui->tbv_batch_logs->setModel(theModelLogBatch);
            ui->tbv_batch_logs->setSelectionModel(theSelectionLogBatch);

    //TableView 信号与槽机制

        //校园卡信息 slot:on_currentChanged
        connect(theSelection,&QItemSelectionModel::currentChanged,this,
                &Widget::on_currentChanged);

        //食堂信息 slot:on_CanteencurrentChanged
        connect(theSelectionCanteen,&QItemSelectionModel::currentChanged,this,
                &Widget::on_canteenCurrentChanged);

    //设置tableView样式

        //校园卡信息
        setStudentTableViewStyle();

        //食堂信息
        setCanteenListStyle();

        //单个窗口消费信息
        setCanteenSingleWindowStyle();

        //校园卡操作日志
        setStudentLog();

        //食堂消费总日志
        setCanteenLog();

        //批量操作总日志
        setBatchLog();


    /****************************************************
     * function:Configure the basic environment of the  *
     *          main window.                            *
     ****************************************************/

    //设置窗口大小
    resize(2160,1215);//16

    //设置背景图片
    setWindowIcon(QIcon(":/image/windowTitle.png"));

    //设置标题
    setWindowTitle("校园卡食堂管理系统");

    //设置左侧边栏myToolbutton的样式
    setLeftToolButtonStyle();

    //设置右上角的myPushButton
    setRightPushButtonStyle();

    //设置分割线的属性
    ui->line_left->setStyleSheet("#line_left{color:#F4F5F7;}");
    ui->line_between_list_info->setStyleSheet("#line_between_list_info{color:#E7EBF1;}");
    ui->line_canteen->setStyleSheet("#line_canteen{color:#E7EBF1;}");
    ui->line_log->setStyleSheet("#line_log{color:#E7EBF1;}");

    //设置page_card
    setPageCardContent();

    //设置page_canteen
    setPageCanteenContent();

    //设置page_batch
    setPageBatchContent();

    //设置搜索框
    setSearchBox();

    //设置groupbox无边框
    setGroupBoxNoBorder();
}

//析构函数
Widget::~Widget()
{
    delete ui;
}

/************************初始化 Model***************************/

//Student Card
void Widget::iniModelFromCardManagement()
{
    int rowCount = this->cardManage->stuNumber;//学生数量
    theModel->setRowCount(rowCount);//设置行数
    QStringList header;
    header<<"学号"<<"姓名"<<"账户"<<"校园卡";
    theModel->setHorizontalHeaderLabels(header);//设置表头文字
    QStringList templist;
    for(int i=0;i<rowCount;i++)
    {
        //存储信息
        CampusCard cc = this->cardManage->campusCardList.at(i);
        templist<<cc.studentNumber<<cc.studentName;

        if(cc.accountState==true)
            templist<<"正常";
        else
            templist<<"注销";

        if(cc.isDistributed==true)
            templist<<"已发卡";
        else
            templist<<"未发卡";

        //显示信息
        for(int j=0;j<FixedColumnNumber;j++)
        {
            //定义
            QStandardItem *aItem = new QStandardItem(templist.at(j));

            theModel->setItem(i,j,aItem);//设置项
        }

        //清空
        templist.clear();
    }
}

//Canteen Information
void Widget::iniModelFromCanteenManagement()
{
    int rowCount = this->canteenManage->windowNumber;//窗口数量
    theModelCanteen->setRowCount(rowCount);//设置行数

    QStringList header;
    header<<"窗口号"<<"记录位置号";
    theModelCanteen->setHorizontalHeaderLabels(header);//设置表头文字

    QStringList templist;
    for(int i=0;i<rowCount;i++)
    {
        //存储信息
        CanteenWindow cw = this->canteenManage->CanteenWindowList.at(i);
        templist<<cw.winNumber<<cw.locationNumber;

        //显示信息
        for(int j=0;j<2;j++)
        {
            //定义
            QStandardItem *aItem = new QStandardItem(templist.at(j));

            theModelCanteen->setItem(i,j,aItem);//设置项
        }

        //清空
        templist.clear();
    }
}

//Single Canteen Window Log
void Widget::iniModelFromSingleCanteenWindowLog()
{
    //准备表格头
    QStringList header;
    header<<"消费卡号"<<"余额变化"<<"当前余额"<<"消费结果";
    theModelWindow->setHorizontalHeaderLabels(header);

    //准备临时数据
    QStringList tempList;

    //获取当前索引
    int id = ui->tbv_win_list->currentIndex().row();

    //累计交易次数
    int sumTimes = 0;

    //累计交易金额
    qreal sumAmount = 0.0;

    //获取当前日期
    QDateTime curDt = *this->globalDateTime;

    //更新当前日期下的消费记录
    std::sort(this->canteenManage->CanteenWindowList[id].tempLog.begin(),
              this->canteenManage->CanteenWindowList[id].tempLog.end());

    //测试代码
    //qDebug()<<"id="<<id;
    //qDebug()<<"logs size="<<this->canteenManage->CanteenWindowList[id].tempLog.size();

    //遍历到当天的日志
    for(int i=0;i<this->canteenManage->CanteenWindowList[id].tempLog.size();i++)
    {
        auto it = &canteenManage->CanteenWindowList[id].tempLog[i];

        qDebug()<<it->opTime.date();

        if(it->opTime.date()<curDt.date())
            continue;
        else if(it->opTime.date()>curDt.date())
            break;
        else
        {
            qreal bc = it->balanceChange;
            qreal bb = it->balanceBefore;
            QString cN = it->canteenNumber;//消费卡号

            if(it->opResult==true)//消费成功
                tempList<<cN<<QString("%1").arg(bc)<<QString("%1").arg(bb+bc);
            else//消费失败
                tempList<<cN<<QString("%1").arg(bc)+"!"<<QString("%1").arg(bb);

            if(it->opResult==true)
                tempList<<"成功";
            else
                tempList<<"失败";

            for(int j=0;j<4;j++)
            {
                //定义
                QStandardItem *aItem = new QStandardItem(tempList.at(j));

                //设置项
                theModelWindow->setItem(sumTimes,j,aItem);

            }
            tempList.clear();//清空
            sumTimes += 1;//当日累计消费次数
            sumAmount += (-1)*bc;
        }
    }

    //更新当前总消费次数和总收金额
    ui->lbl_total_money->setText(QString("%1").arg(sumAmount));
    ui->lbl_total_times_transactions->setText(QString("%1").arg(sumTimes));
}

//Total Student Card Log
void Widget::iniModelFromTotalCardLog()
{
    //准备表格头
    QStringList header;
    header<<"时间"<<"操作名"<<"姓名"<<"学号"<<"涉及卡号"<<"结果"<<"当前余额";//("形式：369.3(+20.0)")--->7列";
    theModelLogCard->setHorizontalHeaderLabels(header);

    //准备临时数据
    QStringList tempList;

    //排序
    std::sort(this->cardManage->Log.begin(),this->cardManage->Log.end());

    for(int i=0;i<this->cardManage->Log.size();i++)
    {
        auto it = &this->cardManage->Log[i];//得到响应指针

        //时间
        tempList<<it->opTime.toString("yyyy-M-d hh:mm");

        switch (it->nameLocation) {
        case 0:
            tempList<<"开户"<<it->stuName<<it->stuNumber<<"-"<<""<<"-";
            break;
        case 1:
            tempList<<"销户"<<it->stuName<<it->stuNumber<<"-"<<""<<"-";
            break;
        case 2:
            tempList<<"发卡"<<it->stuName<<it->stuNumber<<it->canteenNumber<<""<<"-";
            break;
        case 3:
            tempList<<"挂失"<<it->stuName<<it->stuNumber<<it->canteenNumber<<""<<"-";
            break;
        case 4:
            tempList<<"解挂"<<it->stuName<<it->stuNumber<<it->canteenNumber<<""<<"-";
            break;
        case 5:
            tempList<<"补卡"<<it->stuName<<it->stuNumber<<it->canteenNumber+"(新)"<<""<<"-";
            break;
        case 6:
            tempList<<"充值"<<it->stuName<<it->stuNumber<<it->canteenNumber<<""
               <<QString::number(it->balanceBefore+it->balanceChange,'f',2)
                 +"(+"+QString::number(it->balanceChange)+")";//保留两位小数
            break;
        }
        //操作结果
        if(it->opResult==true)
            tempList[5]="成功";
        else
            tempList[5]="失败";

        //显示
        for(int j=0;j<7;j++)
        {
            //定义
            QStandardItem *aItem = new QStandardItem(tempList.at(j));

            //设置项
            theModelLogCard->setItem(i,j,aItem);
        }

        //字符串清空
        tempList.clear();
    }
}

//Total Recharge And Consumption Log
void Widget::iniModelFromTotalConsumeLog()
{

}

//Total Batch Log
void Widget::iniModelFromTotalBatchLog()
{
    //准备表格头
    QStringList header;
    header<<"操作类型"<<"数据项数"<<"成功项数"<<"异常项数";
    theModelLogBatch->setHorizontalHeaderLabels(header);

    //准备临时数据
    QStringList tempList;

    //遍历
    for(int i=0;i<this->globalBatchLog.size();i++)
    {
        //得到相应指针
        auto it = &this->globalBatchLog[i];

        //总项数
        QString opt = QString::number(it->opTotalNumber);

        //异常项数
        QString ope = QString::number(it->opExceptionNumber);

        //成功项数
        QString ops = QString::number(it->opTotalNumber-it->opExceptionNumber);

        //选择
        switch (it->nameLocation) {
        case 0://开户
            tempList<<"开户"<<opt<<opt<<ope;
            break;
        case 2://发卡
            tempList<<"发卡"<<opt<<opt<<ope;
            break;
        case 7://消费
            tempList<<"消费"<<opt<<opt<<ope;
            break;
        case 6://集体充值
            tempList<<"充值"<<opt<<opt<<ope;
            break;
        default:
            tempList<<"卡片操作"<<opt<<opt<<ope;
            break;
        }

        for(int j=0;j<4;j++)
        {
            //定义
            QStandardItem *aItem = new QStandardItem(tempList.at(j));

            //设置项
            theModelLogBatch->setItem(i,j,aItem);
        }

        //清空
        tempList.clear();
    }
}

/**************************************************************/



/******************设置TableView的样式***************************/

//共用样式代码
void Widget::commonStyleCodeOfQTableView(QTableView *tbv)
{
    //设置不可编辑
    tbv->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //tableView自适应宽度
    tbv->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //设置选中时为整行选中
    tbv->setSelectionBehavior(QAbstractItemView::SelectRows);

    //是的行之间切换颜色
    tbv->setAlternatingRowColors(true);

    //设置滚动条宽度
    tbv->verticalScrollBar()->setFixedWidth(33);

    //设置表头高度
    tbv->horizontalHeader()->setMinimumHeight(100);

    //显示边框
    tbv->setShowGrid(true);

    //QSS

        //滚动条样式
        tbv->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{"        //垂直滑块整体
                                                                      "background:#FFFFFF;"  //背景色
                                                                      "padding-top:35px;"    //上预留位置（放置向上箭头）
                                                                      "padding-bottom:35px;" //下预留位置（放置向下箭头）
                                                                      "padding-left:3px;"    //左预留位置（美观）
                                                                      "padding-right:3px;"   //右预留位置（美观）
                                                                      "border-left:1px solid #d7d7d7;}"//左分割线
                                                                      "QScrollBar::handle:vertical{"//滑块样式
                                                                      "background:#dbdbdb;"  //滑块颜色
                                                                      "border-radius:6px;"   //边角圆润
                                                                      "min-height:80px;}"    //滑块最小高度
                                                                      "QScrollBar::handle:vertical:hover{"//鼠标触及滑块样式
                                                                      "background:#d0d0d0;}" //滑块颜色
                                                                      "QScrollBar::add-line:vertical{height:30px;width:30px;border-image:url(:/card_page/image/card_page/down.png);subcontrol-position:bottom;}"
                                                                      "QScrollBar::sub-line:vertical{height:30px;width:30px;border-image:url(:/card_page/image/card_page/up.png);subcontrol-position:top;}"
                                                                  );
}

//校园卡信息
void Widget::setStudentTableViewStyle()
{
    //tableView 表头
    ui->tableViewStudents->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #1aaba8"
                                "color: #4472CA;padding-left: 10px;}");
    //tableView 整体
    ui->tableViewStudents->setStyleSheet("\
    QTableView#tableViewStudents\
    {\
     border: 30px #1aaba8;\
     padding: 30px 30px;\
     border-radius: 15px;\
     background-color: #9dd7d5;\
     alternate-background-color: #fff4c5; /*行交替颜色*/\
     selection-background-color: #1f9792; /*选中行背景颜色*/\
    }");

    //公共效果
    commonStyleCodeOfQTableView(ui->tableViewStudents);
}

//食堂信息
void Widget::setCanteenListStyle()
{
    //tableView 表头
    ui->tbv_win_list->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #cd5554"
                                "color: #fcad84;padding-left: 10px;}");
    //tableView 整体
    ui->tbv_win_list->setStyleSheet("\
    QTableView#tbv_win_list\
    {\
     border: 30px #cd5554;\
     padding: 30px 30px;\
     border-radius: 15px;\
     background-color: #fcad84;\
     alternate-background-color: #feda6a; /*行交替颜色*/\
     selection-background-color: #F37C3D; /*选中行背景颜色*/\
    }");//background-color:#FFA544,#f9b271

    //公共效果
    commonStyleCodeOfQTableView(ui->tbv_win_list);
}

//单个窗口消费信息
void Widget::setCanteenSingleWindowStyle()
{
    //tableView 表头
    ui->tbv_canteen_logs->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #cd5554"
                                "color: #fcad84;padding-left: 10px;}");
    //tableView 整体
    ui->tbv_canteen_logs->setStyleSheet("\
    QTableView#tbv_canteen_logs\
    {\
     border: 30px #cd5554;\
     padding: 30px 30px;\
     border-radius: 15px;\
     background-color: #fcad84;\
     alternate-background-color: #feda6a; /*行交替颜色*/\
     selection-background-color: #F37C3D; /*选中行背景颜色*/\
    }");

    //公共效果
    commonStyleCodeOfQTableView(ui->tbv_canteen_logs);

    //设置表头高度
    ui->tbv_canteen_logs->horizontalHeader()->setMinimumHeight(50);

}

//校园卡操作日志
void Widget::setStudentLog()
{
    //tableView 表头
    ui->tbv_card_logs->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #1aaba8"
                                "color: #4472CA;padding-left: 10px;}");
    //tableView 整体
    ui->tbv_card_logs->setStyleSheet("\
    QTableView#tbv_card_logs\
    {\
     border: 30px #1aaba8;\
     padding: 30px 30px;\
     border-radius: 15px;\
     background-color: #AFEAAA;\
     alternate-background-color: #8DB87C; /*行交替颜色*/\
     selection-background-color: #F5D769; /*选中行背景颜色*/\
    }");//background-color:#D4E8C1(偏暗色)

    //公共效果
    commonStyleCodeOfQTableView(ui->tbv_card_logs);

    //设置表头高度
    ui->tbv_card_logs->horizontalHeader()->setMinimumHeight(50);
}

//食堂消费总日志
void Widget::setCanteenLog()
{
    //tableView 表头
    ui->tbv_canteen_logs_total->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #1aaba8"
                                "color: #4472CA;padding-left: 10px;}");
    //tableView 整体
    ui->tbv_canteen_logs_total->setStyleSheet("\
    QTableView#tbv_canteen_logs_total\
    {\
     border: 30px #1aaba8;\
     padding: 30px 30px;\
     border-radius: 15px;\
                                                  background-color: #AFEAAA;\
                                                  alternate-background-color: #8DB87C; /*行交替颜色*/\
                                                  selection-background-color: #F5D769; /*选中行背景颜色*/\
    }");

    //公共效果
    commonStyleCodeOfQTableView(ui->tbv_canteen_logs_total);

    //设置表头高度
    ui->tbv_canteen_logs_total->horizontalHeader()->setMinimumHeight(50);
}

//批量操作总日志
void Widget::setBatchLog()
{
    //tableView 表头
    ui->tbv_batch_logs->horizontalHeader()
            ->setStyleSheet("QHeaderView::section {"
                                "background-color: #1aaba8"
                                "color: #4472CA;padding-left: 10px;}");
    //tableView 整体
    ui->tbv_batch_logs->setStyleSheet("\
    QTableView#tbv_batch_logs\
    {\
     border: 30px #1aaba8;\
     padding: 30px 30px;\
     border-radius: 15px;\
                                          background-color: #AFEAAA;\
                                          alternate-background-color: #8DB87C; /*行交替颜色*/\
                                          selection-background-color: #F5D769; /*选中行背景颜色*/\
    }");

    //公共效果
    commonStyleCodeOfQTableView(ui->tbv_batch_logs);
}

/***************************************************************/



/***************************Global******************************/

//采取当前时间
void Widget::on_btn_getCurrentTime_clicked()
{
    //和更新时间时刻联系起来
    connect(timer,QTimer::timeout,this,[=](){

        //初始化 : 设置当前时间(默认全部发生:当前时间)
        *this->globalDateTime=QDateTime::currentDateTime();

        //更新时间
        this->updateDateTimeOnLabel();

    });

    //触发时间计时器
    timer->start(1000);//每秒更新一次
}

//点击按钮设置全局时间
void Widget::on_btn_setDateTime_clicked()
{
    dlg_datetime = new DialogsetDateTime(this);//初始化对话框

    dlg_datetime->setAttribute(Qt::WA_DeleteOnClose);//对话框关闭时自动删除

    connect(dlg_datetime,&DialogsetDateTime::dlgSetGlobalDateTime,this,
            &Widget::mysetDateTime);

    dlg_datetime->exec();
}

//传参设置时间槽函数
void Widget::mysetDateTime(QDateTime* newDateTime)
{
    this->globalDateTime = newDateTime;//更新当前时间

    //qDebug()<<newDateTime;//测试代码

    updateDateTimeOnLabel();//更新Label上显示的时间

    //将全局timer暂停
    this->timer->stop();
}

//更新显示全局时间的标签
void Widget::updateDateTimeOnLabel()
{
    ui->lbl_current_date->setText(this->globalDateTime->toString("yyyy年M月d日"));

    ui->lbl_current_time->setText(this->globalDateTime->toString("h:m:s\n(z")+" ms)");
}

//设置左边栏图标
void Widget::setLeftToolButtonStyle()
{
    ui->card->Initialization(QString(":/image/card.png"),QString(":/image/card_clicked.png"),
                             QSize(50,50),QString("账户管理"),QSize(120,120));
    ui->canteen->Initialization(QString(":/image/canteen.png"),QString(":/image/canteen_clicked.png"),
                             QSize(50,50),QString("消费模拟"),QSize(120,120));
    ui->analysis->Initialization(QString(":/image/summary.png"),QString(":/image/summary_clicked.png"),
                             QSize(50,50),QString("汇总分析"),QSize(120,120));
    ui->batch->Initialization(QString(":/image/batch_process.png"),QString(":/image/batch_process_clicked.png"),
                             QSize(50,50),QString("批量处理"),QSize(120,120));
    ui->data_minning->Initialization(QString(":/image/data_minning.png"),QString(":/image/data_minning_clicked.png"),
                             QSize(50,50),QString("数据挖掘"),QSize(120,120));
    connect(ui->card,&QToolButton::clicked,[=](){
        //设置点击选中效果
        ui->card->setClicked();
        ui->canteen->setNormal();
        ui->analysis->setNormal();
        ui->batch->setNormal();
        ui->data_minning->setNormal();

        //将toolButton和StackedWidget关联起来
        ui->stack_global->setCurrentIndex(0);

    });
    connect(ui->canteen,&QToolButton::clicked,[=](){
        ui->card->setNormal();
        ui->canteen->setClicked();
        ui->analysis->setNormal();
        ui->batch->setNormal();
        ui->data_minning->setNormal();

        ui->stack_global->setCurrentIndex(1);
    });
    connect(ui->analysis,&QToolButton::clicked,[=](){
        ui->card->setNormal();
        ui->canteen->setNormal();
        ui->analysis->setClicked();
        ui->batch->setNormal();
        ui->data_minning->setNormal();

        ui->stack_global->setCurrentIndex(2);
    });
    connect(ui->batch,&QToolButton::clicked,[=](){
        ui->card->setNormal();
        ui->canteen->setNormal();
        ui->analysis->setNormal();
        ui->batch->setClicked();
        ui->data_minning->setNormal();

        ui->stack_global->setCurrentIndex(3);
    });
    connect(ui->data_minning,&QToolButton::clicked,[=](){
        ui->card->setNormal();
        ui->canteen->setNormal();
        ui->analysis->setNormal();
        ui->batch->setNormal();
        ui->data_minning->setClicked();

        ui->stack_global->setCurrentIndex(4);
    });
}

//设置右上角图标
void Widget::setRightPushButtonStyle()
{
    ui->top_help->Initialization(
                QString(":/image/topbtn_help.png"),QString(":/image/topbtn_help.png"),
                QSize(48,48),QSize(56,56),QString("帮助"));
    ui->top_help->setStyleSheet("QPushButton{\
                                     background-color:#E3E4E8;\
                                     border-width: 0;\
                                     border-radius: 32px;\
                                 }\
                                 QPushButton:hover{\
                                     background-color:#E7EBF1;\
                                     border-radius:32px;\
                                 }");
    ui->top_info->Initialization(
                QString(":/image/topbtn_info.png"),QString(":/image/topbtn_info.png"),
                QSize(48,48),QSize(56,56),QString("信息"));
    ui->top_info->setStyleSheet("QPushButton{\
                                     background-color:#E3E4E8;\
                                     border-width: 0;\
                                     border-radius: 32px;\
                                 }\
                                 QPushButton:hover{\
                                     background-color:#E7EBF1;\
                                     border-radius:32px;\
                                 }");
    ui->top_setting->Initialization(
                QString(":/image/topbtn_setting.png"),QString(":/image/topbtn_setting.png"),
                QSize(48,48),QSize(56,56),QString("设置"));
    ui->top_setting->setStyleSheet("QPushButton{\
                                     background-color:#E3E4E8;\
                                     border-width: 0;\
                                     border-radius: 32px;\
                                 }\
                                 QPushButton:hover{\
                                     background-color:#E7EBF1;\
                                     border-radius:32px;\
                                 }");
}

//设置groupbox无边框
void Widget::setGroupBoxNoBorder()
{
    //设置groupbox无边框

        //card_page
        ui->gbx_info->setStyleSheet("QGroupBox{border:none}");
        ui->gbx_list->setStyleSheet("QGroupBox{border:none}");

        //canteen_page
        ui->gbx_canteen_info->setStyleSheet("QGroupBox{border:none}");
        ui->gbx_canteen_list->setStyleSheet("QGroupBox{border:none}");

        //batch_page
        ui->gbx_batch->setStyleSheet("QGroupBox{border:none}");

}

/***************************************************************/



/**************************Card_Page****************************/

//设置page_card控件
void Widget::setPageCardContent()
{
    //按比例缩放：
    QPixmap portrait = QPixmap(":/image/usr_portrait_default.jpeg");
    QPixmap temp = portrait.scaled(300,340.5,Qt::KeepAspectRatio);
    ui->lbl_portrait->setPixmap(temp);//设置图片

    //设置小按钮：日期时间按钮，充值按钮
    ui->btn_recharge->Initialization(QString(":/card_page/image/card_page/btn_discharge.png"),QString(":/card_page/image/card_page/btn_discharge.png"),
                                     QSize(30,30),QString("账户充值"),QSize(180,50));
    ui->btn_setDateTime->Initialization(QString(":/image/btn_setTime.png"),QString(":/image/btn_setTime.png"),
                                        QSize(30,30),QString("追忆似水年华"),QSize(220,50));
    ui->btn_getCurrentTime->Initialization(QString(":/image/btn_setTime.png"),QString(":/image/btn_setTime.png"),
                                        QSize(30,30),QString("采取当前时间"),QSize(220,50));
    ui->btn_recharge->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->btn_setDateTime->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->btn_getCurrentTime->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);


    //设置圆形按钮
    ui->btn_loss->Initialization(QString(":/card_page/image/card_page/btn_reportLoss.png"),QString(":/card_page/image/card_page/btn_reportLoss.png"),
                                 QSize(55,55),QSize(100,100),QString("挂失"));
    ui->btn_uncouple->Initialization(QString(":/card_page/image/card_page/btn_uncouple.png"),QString(":/card_page/image/card_page/btn_uncouple.png"),
                                     QSize(55,55),QSize(100,100),QString("解挂"));
    ui->btn_reissue->Initialization(QString(":/card_page/image/card_page/btn_reissue.png"),QString(":/card_page/image/card_page/btn_reissue.png"),
                                    QSize(55,55),QSize(100,100),QString("补卡"));

    //设置矩形按钮
    ui->tbtn_open->Initialization(QString(":/card_page/image/card_page/btn_open.png"),QString(":/card_page/image/card_page/btn_open.png"),
                                  QSize(30,30),QString("新建账户"),QSize(180,50));
    ui->tbtn_distribute->Initialization(QString(":/card_page/image/card_page/btn_distribute.png"),QString(":/card_page/image/card_page/btn_distribute.png"),
                                        QSize(30,30),QString("分配卡号"),QSize(180,50));
    ui->tbtn_delete->Initialization(QString(":/card_page/image/card_page/btn_account_delete.png"),QString(":/card_page/image/card_page/btn_account_delete.png"),
                                    QSize(30,30),QString("注销账户"),QSize(180,50));
    ui->tbtn_batch_process->Initialization(QString(":/card_page/image/card_page/btn_batch_process.png"),QString(":/card_page/image/card_page/btn_batch_process.png"),
                                           QSize(30,30),QString("批量操作"),QSize(180,50));
    ui->tbtn_batch_open->Initialization(QString(":/card_page/image/card_page/btn_batch_distribute.png"),QString(":/card_page/image/card_page/btn_batch_distribute.png"),
                        QSize(30,30),QString("批量开户"),QSize(180,50));
    ui->tbtn_batch_open->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tbtn_batch_process->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tbtn_delete->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tbtn_distribute->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->tbtn_open->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

//实现搜索框以及相应功能
void Widget::setSearchBox()
{
    QPushButton* btn =  ui->btn_search;//按钮
    QLineEdit* ldt =  ui->ldt_search;//编辑框

    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedSize(30, 30);
    btn->setToolTip(QStringLiteral("搜索"));
    btn->setStyleSheet("QPushButton{border-image:url(:/card_page/image/card_page/search_normal.png); background:transparent;} \
                                         QPushButton:hover{border-image:url(:/card_page/image/card_page/search_hover.png)} \
                                         QPushButton:pressed{border-image:url(:/card_page/image/card_page/search_hover.png)}");

    //防止文本框输入内容位于按钮之下
    QMargins margins = ldt->textMargins();
    ldt->setTextMargins(margins.left(), margins.top(), btn->width(), margins.bottom());
    ldt->setPlaceholderText(QStringLiteral("请输入搜索学号"));
    ldt->setStyleSheet("QLineEdit{border-radius: 15px;"
                       "border:6px solid #4CC3D9;"
                       "}");

    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(btn);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setContentsMargins(0, 0, 20, 0);
    ldt->setLayout(pSearchLayout);

    connect(btn, &QPushButton::clicked, this,&Widget::stu_search);
}

//查询槽函数
void Widget::stu_search()
{

}

//批量开户
void Widget::on_tbtn_batch_open_clicked()
{
    QString curPath = QDir::currentPath();//获取当前目录

    QString dlgTitle = "选择文件";

    QString filter = "文本文件(*.txt)";

    //返回选择文件的带路径的完整文件名
    QString aFileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if(!aFileName.isEmpty()){
        //所打开文件非空
        //qDebug()<<aFileName;//测试代码
//        bool flag = this->openBatchOpenFileEvent(aFileName);//响应批量开户时间
        this->openFileStudentAccount(aFileName);//响应批量开户时间
    }
}

//批量开户读取文件操作模块
bool Widget::openFileStudentAccount(QString fileName)
{
    //为批量操作准备
    int total=0;//总数据个数
    int exceptNum=0;//成功项数

    //QFile和QTextStream结合
    QFile aFile(fileName);
    if(!aFile.exists())
        return false;//文件不存在
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;//文件打开失败
    QTextStream aStream(&aFile);//用文本流读取文件
    aStream.setAutoDetectUnicode(true);//自动检测 Unicode,兼容中文字符
    QString info = aStream.readLine();//读取一行文本
    qDebug()<<info;//测试代码
    if(info!="KH")
        return false;//不是批量开户文件，退回
    while(!aStream.atEnd())
    {
        info = aStream.readLine();//读取一行

        //文件分割
        info = info.section(';',0,0);//丢去最后的分号
        QString stuNumber = info.mid(0,10);//十位字符串
        QString stuName = info.mid(11);//取名字
        /*qDebug()<<info;//测试代码
        qDebug()<<stuName<<" "<<stuNumber;*/

        //初始化
        bool res=this->cardManage->addNewAccount(stuNumber,stuName,QDate(2024,7,15));//添加新用户

        //异常项数统计
        if(!res)
            exceptNum += 1;

        //总项数统计
        total += 1;

        //生成单个日志
        OperationLog *l = new OperationLog(1,0,res,QDateTime(QDate(2021,9,1),QTime(8,30)),
                                           stuName,stuNumber,NULL,NULL,0.0,0.0,0,0);
        //加入其中
        this->cardManage->Log.append(*l);
    }

    //生成批量操作的日志
    OperationLog *bl = new OperationLog(3,0,true,QDateTime(QDate(2021,9,1),QTime(8,30)),
                                        NULL,NULL,NULL,NULL,0,0,total,exceptNum);
    //加入其中
    this->globalBatchLog.append(*bl);

    aFile.close();//关闭文件

    iniModelFromCardManagement();//更新显示学生列表

    iniModelFromTotalCardLog();//更新显示卡片操作日志

    iniModelFromTotalBatchLog();//更新显示批量操作日志

    return true;
}

//批量发卡
void Widget::on_tbtn_distribute_clicked()
{
    QString dlgTitle = "分配卡号";
    QString strInfo = "是否对当前列表分配卡号?";
    QMessageBox::StandardButton result;//返回选择的按钮
    result = QMessageBox::question(this,dlgTitle,strInfo,
                                   QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        //分配卡号
        this->cardManage->batch_distributeCard();

        //更新列表
        iniModelFromCardManagement();
    }
}

//当前所选单元格发生变化
void Widget::on_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    //该槽函数将会与项选择模型 theSelection的 currentChanged()信号关联
    //选择单元格变化时的响应
        Q_UNUSED(previous);

        if (current.isValid()) //当前模型索引有效
        {
            int curRow = current.row();
            this->updateInfoOnGroupboxInfo(curRow);
        }
}

//更新gbx_info所显示的信息
void Widget::updateInfoOnGroupboxInfo(int curRow){
    //获取所需要显示的信息
    CampusCard &cc = this->cardManage->campusCardList[curRow];
    ui->acount_number->setText(cc.studentNumber);//设置学号
    ui->lbl_name->setText(cc.studentName);//设置姓名
    ui->lbl_balance->setText(QString("%1").arg(cc.balance));//设置余额
    if(cc.isDistributed==true)
    {
        ui->lbl_number->setText(cc.cardNumber.last());//设置有效卡号
    }
    else
        ui->lbl_number->setText(QString("(未激活)"));
    if(cc.accountState==true)
    {
        ui->lbl_account_state->setText(QString("正常"));
        if(cc.cardState==true)
        {
            ui->lbl_card_state->setText("正常");
        }
        else
        {
            ui->lbl_card_state->setText("禁用");
        }
    }
    else
    {
        ui->lbl_account_state->setText(QString("注销"));
        ui->lbl_account_state->setText("禁用");
    }
}

//挂失
void Widget::on_btn_loss_clicked()
{
    //获取当前账户索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //获取当前账户状态
    bool cardState = this->cardManage->campusCardList[index].cardState;
    bool accountState = this->cardManage->campusCardList[index].accountState;

    if(accountState==true&&cardState==true)
    {
        QString dlgTitle = "校园卡挂失";
        QString strInfo = "是否挂失校园卡?";
        //QMessageBox::StandardButton defaultButton = QMessageBox::No;
        QMessageBox::StandardButton result;//返回选择的按钮
        result = QMessageBox::question(this,dlgTitle,strInfo,
                                       QMessageBox::Yes|QMessageBox::No);
        if(result==QMessageBox::Yes)
        {
            //挂失校园卡
            this->cardManage->campusCardList[index].cardReportLoss();//挂失

            //修改标签
            ui->lbl_card_state->setText("禁用");

            //更新列表
            iniModelFromCardManagement();

            //更新信息表
            this->updateInfoOnGroupboxInfo(index);

            QString stuName = this->cardManage->campusCardList[index].studentName;
            QString studentNumber = this->cardManage->campusCardList[index].studentNumber;
            QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

            //生成日志
            OperationLog *sl = new OperationLog(1,3,true,*this->globalDateTime,stuName,studentNumber,
                                                cNumber,NULL,0,0,0,0);

            //加入
            this->cardManage->Log.append(*sl);

            //显示
            iniModelFromTotalCardLog();
        }
    }
    else
    {
        QString dlgTitle = "提示";
        QString strInfo = "当前校园卡不可用,挂失失败!";
        QMessageBox::StandardButton result;//返回选择的按钮
        result = QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
}

//解挂
void Widget::on_btn_uncouple_clicked()
{
    //获取当前账户索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //获取当前账户状态
    bool accountState = this->cardManage->campusCardList[index].accountState;
    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
    bool cardState = this->cardManage->campusCardList[index].cardState;

    if(accountState==false)
    {
        QString dlgTitle = "提示";
        QString strInfo = "当前账户不可用,解挂失败!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
    else {
        if(isDistributed==true&&cardState==false)
        {
            QString dlgTitle = "校园卡解挂";
            QString strInfo = "是否解挂校园卡?";
            //QMessageBox::StandardButton defaultButton = QMessageBox::No;
            QMessageBox::StandardButton result;//返回选择的按钮
            result = QMessageBox::question(this,dlgTitle,strInfo,
                                           QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::Yes)
            {
                //解挂校园卡
                this->cardManage->campusCardList[index].cardUncouple();

                //设置label
                ui->lbl_card_state->setText("正常");

                //更新列表
                iniModelFromCardManagement();

                //更新信息表
                this->updateInfoOnGroupboxInfo(index);

                //日志
                QString stuName = this->cardManage->campusCardList[index].studentName;
                QString studentNumber = this->cardManage->campusCardList[index].studentNumber;
                QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                //生成日志
                OperationLog *sl = new OperationLog(1,4,true,*this->globalDateTime,stuName,studentNumber,
                                                    cNumber,NULL,0.0,0.0,0,0);

                //加入
                this->cardManage->Log.append(*sl);

                //显示
                iniModelFromTotalCardLog();
            }
        }
        else if(isDistributed==false&&cardState==false)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡食堂消费未激活,解挂失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
        else if(isDistributed==true&&cardState==true)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡状态正常,解挂失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
    }
}

//补卡
void Widget::on_btn_reissue_clicked()
{
    //获取当前账户索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //获取当前账户状态
    bool accountState = this->cardManage->campusCardList[index].accountState;
    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
    bool cardState = this->cardManage->campusCardList[index].cardState;

    if(accountState==false)
    {
        QString dlgTitle = "提示";
        QString strInfo = "当前账户不可用,补卡失败!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
    else {
        if(isDistributed==true&&cardState==false)
        {
            QString dlgTitle = "校园卡补卡";
            QString strInfo = "是否补办校园卡?";
            QMessageBox::StandardButton result;//返回选择的按钮
            result = QMessageBox::question(this,dlgTitle,strInfo,
                                           QMessageBox::Yes|QMessageBox::No);
            if(result==QMessageBox::Yes)
            {
                //解挂校园卡
                QString stuNumber = ui->acount_number->text();
                this->cardManage->reissueCard(stuNumber);//补卡

                //设置label
                ui->lbl_card_state->setText("正常");

                //更新信息表
                this->updateInfoOnGroupboxInfo(index);

                //更新日志
                //日志
                QString stuName = this->cardManage->campusCardList[index].studentName;
                QString studentNumber = this->cardManage->campusCardList[index].studentNumber;
                QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                //生成日志
                OperationLog *sl = new OperationLog(1,5,true,*this->globalDateTime,stuName,studentNumber,
                                                    cNumber,NULL,0.0,0.0,0,0);

                //加入
                this->cardManage->Log.append(*sl);

                //显示
                iniModelFromTotalCardLog();

            }
        }
        else if(isDistributed==false&&cardState==false)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡食堂消费未激活,补卡失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
        else if(isDistributed==true&&cardState==true)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡状态正常,补卡失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
    }
}

//注销账户
void Widget::on_tbtn_delete_clicked()
{
    //获取当前账户索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //获取当前账户状态
    bool accountState = this->cardManage->campusCardList[index].accountState;

    if(accountState==true)
    {
        QString dlgTitle = "注销账户";
        QString strInfo = "确认注销账户(一旦注销无法恢复)?";
        QMessageBox::StandardButton defaultButton = QMessageBox::No;
        QMessageBox::StandardButton result;//返回选择的按钮
        result = QMessageBox::question(this,dlgTitle,strInfo,
                                       QMessageBox::Yes|QMessageBox::No);
        if(result==QMessageBox::Yes)
        {
            //注销账户
            QString stuNumber = ui->acount_number->text();//获取学号
            this->cardManage->deleteExistedAccount(stuNumber);//注销账户
            //更新列表
            iniModelFromCardManagement();
            //更新信息表
            this->updateInfoOnGroupboxInfo(index);
        }
    }
    else
    {
        QString dlgTitle = "提示";
        QString strInfo = "当前账户已注销,注销失败!";
        QMessageBox::StandardButton result;//返回选择的按钮
        result = QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
}

//充值
void Widget::on_btn_recharge_clicked()
{
    //获取当前账户索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //获取当前账户状态
    bool accountState = this->cardManage->campusCardList[index].accountState;
    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
    bool cardState = this->cardManage->campusCardList[index].cardState;

    if(accountState==false)
    {
        QString dlgTitle = "提示";
        QString strInfo = "当前账户不可用,无法充值!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
    }
    else {
        if(isDistributed==true&&cardState==true)
        {
            //获取当前余额
            qreal balance = this->cardManage->campusCardList[index].balance;

            //初始化对话框
            dlg_recharge = new DialogRecharge(this,balance);

            //对话框关闭时自动删除
            dlg_recharge->setAttribute(Qt::WA_DeleteOnClose);

            //信号和槽机制,窗口关闭发送消息更新余额
            connect(dlg_recharge,&DialogRecharge::updateTempBalance,this,
                    &Widget::getRechargeBalance);//公共槽函数和信号

            //模态方式显示对话框
            dlg_recharge->exec();
        }
        else if(isDistributed==false&&cardState==false)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡食堂消费未激活,充值失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
        else if(isDistributed==true&&cardState==false)
        {
            QString dlgTitle = "提示";
            QString strInfo = "当前校园卡状态异常,充值失败!";
            QMessageBox::warning(this,dlgTitle,strInfo,
                                          QMessageBox::Ok);
        }
    }
}

//与充值对话框进行交互的公共接口
void Widget::getRechargeBalance(qreal tempBalance)
{
    //获取当前索引
    int index = this->cardManage->mapStuNumberToCardNumber[ui->acount_number->text()];

    //更新余额
    qreal bB = this->cardManage->campusCardList[index].balance;
    qreal bC = tempBalance - bB;
    this->cardManage->campusCardList[index].balance = tempBalance;

    //更新groupbox_info
    updateInfoOnGroupboxInfo(index);

    //生成日志
    QString stuName = this->cardManage->campusCardList[index].studentName;
    QString studentNumber = this->cardManage->campusCardList[index].studentNumber;
    QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();
    OperationLog *sl = new OperationLog(1,6,true,*this->globalDateTime,stuName,studentNumber,
                              cNumber,NULL,bB,bC,0,0);
    //添加
    this->cardManage->Log.append(*sl);

    //显示
    iniModelFromTotalCardLog();
}

/****************************************************************/



/**************************Canteen_Page**************************/

//设置page_canteen
void Widget::setPageCanteenContent()
{
    //设置头像
    QPixmap canteen_wid_figure = QPixmap(":/canteen_page/image/canteen_page/canteen_portrait.jpeg");
    ui->lbl_win_figure->setStyleSheet("border:10;border-radius:15px;padding:8px 6px;");
    QPixmap temp = this->getRoundRectPixmap(canteen_wid_figure,QSize(600,338),25);
    ui->lbl_win_figure->setPixmap(temp);

    //设置左栏食堂窗口按钮样式

        //批量消费操作
        ui->btn_batch_op->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(30,30),QString("批量消费"),QSize(170,50));
        ui->btn_batch_op->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //导入食堂窗口
        ui->btn_winInfo->Initialization(QString(":/canteen_page/image/canteen_page/uploadWindows_clicked.png"),QString(":/canteen_page/image/canteen_page/uploadWindows_clicked.png"),
                                          QSize(30,30),QString("食堂窗口"),QSize(170,50));
        ui->btn_winInfo->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //单次消费
        ui->btn_consumption->Initialization(QString(":/canteen_page/image/canteen_page/food.png"),QString(":/canteen_page/image/canteen_page/food.png"),
                                            QSize(30,30),QString("用餐消费"),QSize(170,50));
        ui->btn_consumption->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

//图片圆角裁剪
QPixmap Widget::getRoundRectPixmap(QPixmap srcPixMap, const QSize & size, int radius)
{
    //不处理空数据或者错误数据
    if (srcPixMap.isNull()) {
        return srcPixMap;
    }

    //获取图片尺寸
    int imageWidth = size.width();
    int imageHeight = size.height();

    //处理大尺寸的图片,保证图片显示区域完整
    QPixmap newPixMap = srcPixMap.scaled(imageWidth, (imageHeight == 0 ? imageWidth : imageHeight),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap destImage(imageWidth, imageHeight);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    // 抗锯齿
    painter.setRenderHints(QPainter::Antialiasing, true);
    // 图片平滑处理
    painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
    // 将图片裁剪为圆角
    QPainterPath path;
    QRect rect(0, 0, imageWidth, imageHeight);
    path.addRoundedRect(rect, radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, imageWidth, imageHeight, newPixMap);
    return destImage;
}

//添加食堂窗口信息
void Widget::on_btn_winInfo_clicked()
{
    QString curPath = QDir::currentPath();//获取当前目录

    QString dlgTitle = "选择文件";

    QString filter = "文本文件(*.txt)";

    //返回选择文件的带路径的完整文件名
    QString aFileName = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if(!aFileName.isEmpty()){
        //所打开文件非空:读取文件创建窗口
        this->openFileCanteenWindow(aFileName);
    }
}

//响应批量导入食堂窗口
bool Widget::openFileCanteenWindow(QString fileName)
{
    //QFile和QTextStream结合
    QFile aFile(fileName);
    if(!aFile.exists())
        return false;//文件不存在
    if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;//文件打开失败
    QTextStream aStream(&aFile);//用文本流读取文件
    aStream.setAutoDetectUnicode(true);//自动检测 Unicode,兼容中文字符
    QString info = aStream.readLine();//读取一行文本
    qDebug()<<info;//测试代码
    if(info!="WZ")
        return false;//不是批量开户文件，退回
    while(!aStream.atEnd())
    {
        info = aStream.readLine();//读取一行

        //文件分割
        info = info.section(';',0,0);//丢去最后的分号
        QString wNumber = info.section(',',0,0);//编号
        QString wLoc = info.section(',',1,1);//位置号
        int wN = wNumber.toInt();
        if(wN<10)
            wNumber=QString("0") + QString::number(wN);

        //初始化·添加新窗口
        this->canteenManage->addNewCanteenWindow(wNumber,wLoc);
    }
    aFile.close();//关闭文件
    iniModelFromCanteenManagement();//更新显示
    return true;
}

//当前单元格发生变化·食堂信息
void Widget::on_canteenCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    //选择单元格变化时的响应
    Q_UNUSED(previous);

    if(current.isValid())
    {
        int curRow = current.row();//从0开始,恰好对应于所在向量中的位置

        QString wN = this->canteenManage
                ->CanteenWindowList[curRow].winNumber;//窗口号

        QString winLocation = this->canteenManage
                ->CanteenWindowList[curRow].locationNumber;//位置号

        ui->lbl_win_number->setText(wN);//设置窗口号

        ui->lbl_win_location->setText(winLocation);//设置窗口位置号

        ui->lbl_win_curDateTime->setText(this->globalDateTime->toString("yyyy年M月d日"));

        //更新显示
        iniModelFromSingleCanteenWindowLog();
    }
}

//单次消费操作
void Widget::on_btn_consumption_clicked()
{
//    dlg_datetime = new DialogsetDateTime(this);//初始化对话框

//    dlg_datetime->setAttribute(Qt::WA_DeleteOnClose);//对话框关闭时自动删除

//    connect(dlg_datetime,&DialogsetDateTime::dlgSetGlobalDateTime,this,
//            &Widget::mysetDateTime);

//    dlg_datetime->exec();

    dlg_consume = new DialogConsumption(*this->globalDateTime,this);//初始化对话框

    dlg_consume->setAttribute(Qt::WA_DeleteOnClose);//对话框关闭时自动删除

    connect(dlg_consume,&DialogConsumption::dlgSetConsumeInfo,this,
            &Widget::myConsumption);//消费窗口 ---消费信息---> 主页面

    connect(dlg_consume,&DialogConsumption::queryAccount,this,
            &Widget::myQueryBalance);//消费窗口 ---卡号---> 主页面

    connect(this,&Widget::sendBalance,dlg_consume,
            &DialogConsumption::getAccount);//主页面 ---余额---> 消费窗口

    dlg_consume->exec();//模态框形式
}

//传参设置消费槽函数
void Widget::myConsumption(bool closeResult,QString cardNumber,
                   QDateTime dt,qreal amount,bool res)
{
    if(!closeResult)
        return;//消费无效

    //账户索引
    int index = this->cardManage->mapCanteenNumberToCardNumber[cardNumber].first;

    //余额变化量
    qreal balChange = (-1)*amount;

    if(res)
    {
        //原来余额
        qreal balBefore = this->cardManage->campusCardList[index].balance;

        //余额减少
        this->cardManage->campusCardList[index].balance -= amount;

        //记录日志

            //消费时间
            QDate tempd = this->globalDateTime->date();

            //添加日志
            OperationLog *conLog = new OperationLog(2,7,true,QDateTime(tempd,dt.time()),
                                NULL,NULL,cardNumber,ui->lbl_win_number->text(),balBefore,balChange,0,0);

            //添加到当前食堂窗口列表中
            this->canteenManage->CanteenWindowList[index].addConsumptionLog(conLog);

            //更新显示
            iniModelFromSingleCanteenWindowLog();
    }
    else
    {
        //余额不足

            //原来余额
            qreal balBefore = this->cardManage->campusCardList[index].balance;

            //得到当前窗口位置
            ui->tbv_win_list->currentIndex().row();

            //消费时间
            QDate tempd = this->globalDateTime->date();

            //添加日志
            OperationLog *conLog = new OperationLog(2,7,false,QDateTime(tempd,dt.time()),
                    NULL,NULL,cardNumber,ui->lbl_win_number->text(),balBefore,balChange,0,0);

            //添加到当前食堂窗口列表中
            this->canteenManage->CanteenWindowList[index].addConsumptionLog(conLog);

            //更新显示
            iniModelFromSingleCanteenWindowLog();
    }
}

//传参设置查询余额槽函数
void Widget::myQueryBalance(QString cardNumber)
{
    qreal res;

    //返回-1意味着查询失败
    if(!this->cardManage->mapCanteenNumberToCardNumber.contains(cardNumber))
        res = -1;//卡号不存在
    else
    {
//        //卡号状态
//        bool state = this->cardManage->
//                mapCanteenNumberToCardNumber[cardNumber].second;
        //得到索引
        int index = this->cardManage->
                mapCanteenNumberToCardNumber[cardNumber].first;

        //返回-2意味着不是最新的卡
        if(cardNumber!=this->cardManage->campusCardList[index].cardNumber.last())
        {
            res = -2;//(不是最新的卡)卡号已禁用,失效状态
        }
        else
        {
            //判断卡片是否可用
            if(this->cardManage->campusCardList[index].cardState==false)
            {
                res=-3;//卡片是挂失状态
            }
            else
            {
                res = this->cardManage->campusCardList[index].balance;//余额
            }

        }
    }

    //出发信号和控件通信:res>=0(normal),res==-1(卡不存在),res==-2(卡禁用)
    emit sendBalance(res);
}

/*****************************************************************/



/****************************Log_Page*****************************/



/*****************************************************************/



/****************************Batch_Page*****************************/

//设置page_batch
void Widget::setPageBatchContent()
{
    //设置甘特图：
    QPixmap portrait = QPixmap(":/batch_page/image/batch_page/gantt.png");
    //QPixmap temp = portrait.scaled(1800,748.24,Qt::KeepAspectRatio);
    ui->lbl_gantt->setPixmap(portrait);//设置图片

    //设置按钮样式

        //卡片+充值1
        ui->btn_card_rec1->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("09-06 23:59:59"),QSize(340,100));
        ui->btn_card_rec1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //消费1
        ui->btn_con1->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("10-13 23:59:59"),QSize(340,100));
        ui->btn_con1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //消费2
        ui->btn_con2->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("11-03 23:59:59"),QSize(340,100));
        ui->btn_con2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //消费3
        ui->btn_con3->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("11-24 23:59:59"),QSize(340,100));
        ui->btn_con3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //消费4
        ui->btn_con4->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("12-15 23:59:59"),QSize(340,100));
        ui->btn_con4->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //消费5
        ui->btn_con5->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("12-31 23:59:59"),QSize(340,100));
        ui->btn_con5->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //充值2
        ui->btn_rec2->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("10-14 7:00:00"),QSize(340,100));
        ui->btn_rec2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //充值3
        ui->btn_rec3->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("11-04 7:00:00"),QSize(340,100));
        ui->btn_rec3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //充值4
        ui->btn_rec4->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("11-25 7:00:00"),QSize(340,100));
        ui->btn_rec4->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        //充值5
        ui->btn_rec5->Initialization(QString(":/canteen_page/image/canteen_page/batch_consume.png"),QString(":/canteen_page/image/canteen_page/batch_consume.png"),
                                         QSize(60,60),QString("12-16 7:00:00"),QSize(340,100));
        ui->btn_rec5->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    //设置groupBox无边框

}

//批量操作1:卡片操作和充值1
void Widget::on_btn_card_rec1_clicked()
{
    //判断是否之前的已经执行完
    if(sequenceNumber<0)
    {
        QString dlgTitle = "提示";
        QString strInfo = "请先触发之前的操作!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
        return;
    }
    else if(sequenceNumber>0)
    {
        QString dlgTitle = "提示";
        QString strInfo = "请已经触发该操作,不能重复触发!";
        QMessageBox::warning(this,dlgTitle,strInfo,
                                      QMessageBox::Ok);
        return;
    }

    QString dlgTitle = "校园卡解挂";

    QString strInfo = "是否执行截至9.6日的卡片操作和第一次充值操作?";

    QMessageBox::StandardButton result;//返回选择的按钮

    result = QMessageBox::question(this,dlgTitle,strInfo,
                                   QMessageBox::Yes|QMessageBox::No);

    if(result==QMessageBox::Yes)
    {
        //时间点
        QDateTime *begin = new QDateTime(QDate(2021,9,1),QTime(0,0));
        QDateTime *end = new QDateTime(QDate(2021,9,7),QTime(0,0,0));

        //第一次操作
        this->openFileCard(*begin,*end);

        //顺序号增加
        sequenceNumber += 1;
    }
}



//批量处理

    //批量处理卡片数据
    bool Widget::openFileCard(QDateTime begin,QDateTime end)
    {
        //为批量操作准备
        int total=0;//总数据个数
        int exceptNum=0;//成功项数

        //读取文件
        QString fileName =
            "D:\\qt_development_repo\\repo\\campus_card_management_system\\CampusCardManagementSystem\\TestData\\cz002.txt";

        //QFile和QTextStream结合
        QFile aFile(fileName);

        if(!aFile.exists())
            return false;//文件不存在

        if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
            return false;//文件打开失败

        QTextStream aStream(&aFile);//用文本流读取文件

        aStream.setAutoDetectUnicode(true);//自动检测 Unicode,兼容中文字符

        QString info = aStream.readLine();//读取一行文本

        qDebug()<<info;//测试代码

        if(info!="CZ")
            return false;//不是批量卡片操作文件，退回

        while(!aStream.atEnd())
        {
            info = aStream.readLine();//读取一行

            //文件分割
            info = info.section(';',0,0);//丢去最后的分号

            //以逗号分割

                //日期时间字符串
                QString datetime = info.section(',',0,0);//2021 0903 08 31 53 16

                //日期时间
                QDateTime dt = QDateTime::fromString(datetime,"yyyyMMddhhmmssz");

                //测试代码
                qDebug()<<dt;

                //判断日期时间是否超出范围
                if(dt<begin)
                    continue;
                if(dt>end)
                    break;

                //操作名
                QString opName = info.section(',',1,1);

                //学号
                QString studentNumber = info.section(',',2,2);

            //操作结果
            bool res=false;

            //得到相关学号对应学生:存在->对应位置;不存在->-1
            int index = cardManage->queryCampusCard(studentNumber);

            if(index==-1)
            {
                res=false;
            }
            else
            {
                //学生姓名
                QString stuName = this->cardManage->campusCardList[index].studentName;

                //充值时继续读取
                if(opName=="充值")
                {
                    //还有充值金额
                    qreal bC = info.section(',',3,3).toDouble();//转换为qreal类型

                    //获取当前账户状态
                    bool accountState = this->cardManage->campusCardList[index].accountState;
                    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
                    bool cardState = this->cardManage->campusCardList[index].cardState;


                    //判断
                    if(accountState&&isDistributed&&cardState)
                    {
                        //获取之前余额
                        qreal bB = this->cardManage->campusCardList[index].balance;

                        //此时可以充值
                        res = this->cardManage->campusCardList[index].cardRecharge(bC);

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //操作日志
                        OperationLog *sl;

                        if(res==false)
                        {
                            //生成单个日志:余额超限
                            qDebug()<<"充值失败"<<bB<<" "<<"(+"<<bC<<")";
                            sl = new OperationLog(1,6,false,dt,stuName,studentNumber,
                                                      cNumber,NULL,bB,0.0,0,0);
                        }
                        else
                        {
                            sl = new OperationLog(1,6,true,dt,stuName,studentNumber,
                                                      cNumber,NULL,bB,bC,0,0);
                        }

                        //添加日志
                        this->cardManage->Log.append(*sl);
                    }
                    else
                        res=false;
                }
                else if(opName=="挂失")
                {
                    //获取当前账户状态
                    bool cardState = this->cardManage->campusCardList[index].cardState;
                    bool accountState = this->cardManage->campusCardList[index].accountState;

                    if(cardState&&accountState)
                    {
                        //可以进行挂失
                        this->cardManage->campusCardList[index].cardReportLoss();//挂失

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,3,true,dt,stuName,studentNumber,
                                                            cNumber,NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);

                        //状态
                        res=true;
                    }
                    else
                    {
                        //挂失失败

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,3,false,dt,stuName,studentNumber,
                                                            "-",NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);

                        //状态
                        res=false;
                    }
                }
                else if(opName=="解挂")
                {
                    //获取当前账户状态
                    bool accountState = this->cardManage->campusCardList[index].accountState;
                    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
                    bool cardState = this->cardManage->campusCardList[index].cardState;

                    if(accountState==true&&isDistributed==true&&cardState==false)
                    {
                        //更新状态
                        res=true;

                        //解挂校园卡
                        this->cardManage->campusCardList[index].cardUncouple();

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,4,true,dt,stuName,studentNumber,
                                                            cNumber,NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);

                    }
                    else
                    {
                        //更新状态
                        res=false;

                        //解挂校园卡
                        this->cardManage->campusCardList[index].cardUncouple();

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,4,false,dt,stuName,studentNumber,
                                                            cNumber,NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);

                    }
                }
                else if(opName=="销户")
                {
                    res = this->cardManage->campusCardList[index].accountDelete();

                    //生成日志
                    OperationLog *sl = new OperationLog(1,1,res,dt,stuName,studentNumber,
                                                        NULL,NULL,0.0,0.0,0,0);

                    //加入
                    this->cardManage->Log.append(*sl);
                }
                else if(opName=="补卡")
                {
                    //获取当前账户状态
                    bool accountState = this->cardManage->campusCardList[index].accountState;
                    bool isDistributed = this->cardManage->campusCardList[index].isDistributed;
                    bool cardState = this->cardManage->campusCardList[index].cardState;

                    if(accountState==true&&isDistributed==true&&cardState==false)
                    {
                        //补卡
                        this->cardManage->reissueCard(studentNumber);

                        //更新状态
                        res=true;

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,5,true,dt,stuName,studentNumber,
                                                            cNumber,NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);
                    }
                    else
                    {
                        //更新状态
                        res=false;

                        //卡号
                        QString cNumber = this->cardManage->campusCardList[index].cardNumber.last();

                        //生成日志
                        OperationLog *sl = new OperationLog(1,5,false,dt,stuName,studentNumber,
                                                            cNumber,NULL,0.0,0.0,0,0);

                        //加入
                        this->cardManage->Log.append(*sl);

                    }
                }
            }

            //异常项数统计
            if(!res)
                exceptNum += 1;

            //总项数统计
            total += 1;

            //显示进度
            qDebug()<<total<<"  "<<info;
        }

        //生成批量操作的日志
        OperationLog *bl = new OperationLog(3,4,true,*this->globalDateTime,
                                            NULL,NULL,NULL,NULL,0,0,total,exceptNum);
        //加入其中
        this->globalBatchLog.append(*bl);

        aFile.close();//关闭文件

        iniModelFromTotalCardLog();//更新显示卡片操作日志

        iniModelFromTotalBatchLog();//更新显示批量操作日志

        return true;
    }

    //批量处理消费数据
    bool Widget::openFileConsumption(QDateTime begin,QDateTime end)
    {
        //为批量操作准备
        int total=0;//总数据个数
        int exceptNum=0;//成功项数

        //读取文件
        QString fileName =
            "D:\\qt_development_repo\\repo\\campus_card_management_system\\CampusCardManagementSystem\\TestData\\xf014.txt";

        //QFile和QTextStream结合
        QFile aFile(fileName);

        if(!aFile.exists())
            return false;//文件不存在

        if(!aFile.open(QIODevice::ReadOnly|QIODevice::Text))
            return false;//文件打开失败

        QTextStream aStream(&aFile);//用文本流读取文件

        aStream.setAutoDetectUnicode(true);//自动检测 Unicode,兼容中文字符

        QString info = aStream.readLine();//读取一行文本

        qDebug()<<info;//测试代码

        if(info!="XF")
            return false;//不是批量卡片操作文件，退回

        info = aStream.readLine();//读取一行

        while(!aStream.atEnd())
        {
            if(info[0]=="W")
            {
                //测试代码:说明此时是某个窗口的开头
                qDebug()<<info.mid(1);

                //窗口序号
                int winIndex = info.mid(1).toInt();

                //窗口号字符串
                QString wN;

                //定义窗口号
                if(winIndex<10)
                    wN="0"+QString::number(winIndex);
                else
                    wN=QString::number(winIndex);

                //读取开头
                info = aStream.readLine();

                //结果
                bool res = false;

                //直到再出现一个"W"切换窗口
                while(info[0]!="W")
                {
                    //读取本次窗口的信息
                    info = info.section(';',0,0);//取出尾部的分号

                    //卡号
                    QString cNumber = info.section(',',0,0);

                    //日期
                    QDate cDate=QDate::fromString(info.section(',',1,1),"yyyyMMdd");

                    //时间
                    QTime cTime=QTime::fromString(info.section(',',2,2),"hhmmssz");

                    //汇总日期时间
                    QDateTime *datetime = new QDateTime(cDate,cTime);

                    //判断日期时间是否超出范围
                    if(*datetime<begin||*datetime>end)
                    {
                        if(aStream.atEnd())
                            break;
                        else//未位于结尾处
                            info = aStream.readLine();//再读一行
                        continue;
                    }

                    //消费额度
                    qreal money=info.section(',',3,3).toDouble();

                    //翻转
                    qreal balChange = (-1)*money;

                    if(this->cardManage->mapCanteenNumberToCardNumber.contains(cNumber))
                    {
                        //获取索引
                        int index = this->cardManage->mapCanteenNumberToCardNumber[cNumber].first;

                        //指针
                        auto it = &this->cardManage->campusCardList[index];

                        //查询余额
                        qreal balance = it->balance;

                        //判断
                        if(it->accountState&&it->isDistributed&&
                                it->cardState&&it->cardNumber.last()==cNumber)
                        {
                            if(balance>=money)
                            {
                                //成功消费
                                it->balance -= money;

                                //记录日志
                                OperationLog *log = new OperationLog(2,7,true,*datetime,
                                      NULL,NULL,cNumber,wN,balance,balChange,0,0);

                                //加入列表
                                this->canteenManage->CanteenWindowList[winIndex-1].addConsumptionLog(log);

                                //统计
                                total += 1;
                            }
                            else
                            {
                                //余额不足

                                //记录日志
                                OperationLog *log = new OperationLog(2,7,false,*datetime,
                                      NULL,NULL,cNumber,wN,balance,balChange,0,0);

                                //加入列表
                                this->canteenManage->CanteenWindowList[winIndex-1].addConsumptionLog(log);

                                //记数
                                exceptNum += 1;
                                total += 1;
                            }
                        }
                    }

                    if(aStream.atEnd())
                        break;
                    else//未位于结尾处
                        info = aStream.readLine();//再读一行
                }
            }
        }

        //生成批量操作的日志
        OperationLog *bl = new OperationLog(3,7,true,*this->globalDateTime,
                                            NULL,NULL,NULL,NULL,0,0,total,exceptNum);
        //加入其中
        this->globalBatchLog.append(*bl);

        aFile.close();//关闭文件

        iniModelFromTotalConsumeLog();//更新显示食堂操作日志

        iniModelFromTotalBatchLog();//更新显示批量操作日志

        return true;

    }

/*****************************************************************/



