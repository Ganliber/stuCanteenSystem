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

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    /***************************************************
     *  function:Configure the current time            *
     ***************************************************/

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

    //设置搜索框
    setSearchBox();

    //设置groupbox无边框

        //card_page
        ui->gbx_info->setStyleSheet("QGroupBox{border:none}");
        ui->gbx_list->setStyleSheet("QGroupBox{border:none}");

        //canteen_page
        ui->gbx_canteen_info->setStyleSheet("QGroupBox{border:none}");
        ui->gbx_canteen_list->setStyleSheet("QGroupBox{border:none}");

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
void iniModelFromSingleCanteenWindowLog()
{

}

//Total Student Card Log
void iniModelFromTotalCardLog()
{

}

//Total Recharge And Consumption Log
void iniModelFromTotalConsumeAndRechargeLog()
{

}

//Total Batch Log
void iniModelFromTotalBatchLog()
{

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

/***************************************************************/



/**************************Card_Page****************************/

//设置page_card控件
void Widget::setPageCardContent()
{
    //设置左侧gbx_list


    //设置右侧gbx_info

    //设置用户默认头像

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
        this->cardManage->addNewAccount(stuNumber,stuName,QDate(2024,7,15));//添加新用户
    }
    aFile.close();//关闭文件
    iniModelFromCardManagement();//更新显示
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
            ui->lbl_card_state->setText("禁用");
            //更新列表
            iniModelFromCardManagement();
            //更新信息表
            this->updateInfoOnGroupboxInfo(index);
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
            QMessageBox::StandardButton defaultButton = QMessageBox::No;
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
    this->cardManage->campusCardList[index].balance = tempBalance;

    //更新groupbox_info
    updateInfoOnGroupboxInfo(index);
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

    dlg_consume = new DialogConsumption(this);//初始化对话框

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
void Widget::myConsumption(bool closeResult=false,QString cardNumber=NULL,
                   QTime t=QTime::currentTime(),qreal amount=0,
                   bool res=false)
{
    if(!closeResult)
        return;//消费无效

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



