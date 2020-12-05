#include "dataanalysisscene.h"
#include "ui_dataanalysisscene.h"
#include <QSizePolicy>

DataAnalysisScene::DataAnalysisScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataAnalysisScene)
{
    ui->setupUi(this);

    this->resize(1600, 3000);

    flag_numOfOrders = false;

    for(int i = 0; i < 24 * 15; i++){
        numOfOrders[i] = 0;
    }
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 24 * 15; j++){
            numOfOrdersPerGridPerHour[i][j] = 0;
        }
    }


    connect(ui->btn_analyse, &QPushButton::clicked, [=](){
//        if(!flag_numOfOrders) preLoadNumOfOrders();
        initAnalyseDemmandPattern();
    });

    ui->label->setPixmap(QPixmap(":/figs/final_map2.png").scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label_19->setPixmap(QPixmap(":/figs/final_map2.png").scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    pixmap_choosen = QPixmap(":/figs/tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap_not_choosen = QPixmap(":/figs/not_tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    for(int i = 0; i < 100; i++){
        int gridX = i % 10;
        int gridY = i / 10;
        int x = gridX * 54 + 231 - 40;
        int y = 712 - gridY * 54 - 54 - 5;

        if(gridX >= 6){
            x += gridX / 2 - 3;
        }
        if(gridY <= 2){
            y += 2 - gridY / 2;
        }

        iconLabel[i] = new QLabel(this);
        iconLabel[i]->move(x, y);
        iconLabel[i]->setAlignment(Qt::AlignCenter);
        iconLabel[i]->setPixmap(pixmap_choosen);
//        iconLabel[i]->setVisible(false);
    }

//    auto label = new QLabel(this);
//    label->setPixmap(pixmap_not_choosen);
//    label->move(500, 500);

//    auto pub = new QPushButton(this);
//    auto icon_choosen = new QIcon(QPixmap(":/figs/tick.png"));
//    auto icon_not_choosen = new QIcon(QPixmap(":/figs/not_tick.png"));
//    pub->setIcon(*icon_choosen);
//    pub->setText("mode");
//    pub->resize(100, 100);
//    pub->move(200,100);
//    pub->setFlat(true);

//    connect(pub, &QPushButton::clicked, [=](){
//        auto returnPixmap = label->pixmap(Qt::ReturnByValue);
//        if(returnPixmap == pixmap_choosen){
//            label->setPixmap(pixmap_not_choosen);
//        } else {
//            label->setPixmap(pixmap_choosen);
//        }
//    });

    initAnalyseDemmandPattern();

    connect(ui->pushButton_check_all, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_not_choosen){
                iconLabel[i]->setPixmap(pixmap_choosen);
            }
        }
    });

    connect(ui->pushButton_check_none, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_choosen){
                iconLabel[i]->setPixmap(pixmap_not_choosen);
            }
        }
    });

    ui->timeEdit_start_time->setTime(QTime(0, 0));
    ui->timeEdit_end_time->setTime(QTime(23, 59));

    barChart = new QChart;
    barChartView = new QChartView(barChart);
    barSeries = new QBarSeries;
    barSet = new QBarSet("number");
    barCatAxis = new QBarCategoryAxis;

    *barSet << 1 << 2 << 3;

    barSeries->append(barSet);

    barChart->addSeries(barSeries);

    chart->setTitle("Simple barchart example");
//    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList cat;
    for(int i = 0; i < 124; i++){
        if(i % 10 == 0){
            cat << QString::number(i);
        } else {
            cat << "";
        }

    }
//    cat << "a" << "b" << "c" << "d";
    auto barAxisX = new QBarCategoryAxis;
    barAxisX->append(cat);
    barChart->addAxis(barAxisX, Qt::AlignBottom);
    barSeries->attachAxis(barAxisX);

    auto barAxisY = new QValueAxis;
    barAxisY->setRange(0, 300);
    barChart->addAxis(barAxisY, Qt::AlignLeft);
    barSeries->attachAxis(barAxisY);

//    barChart->legend()->setVisible(true);
//    barChart->legend()->setAlignment(Qt::AlignBottom);

    barChartView->setRenderHint(QPainter::Antialiasing);
//    ui->horizontalLayout_18->addWidget(barChartView);
    ui->graphicsView->setChart(barChart);

    barSeries->clear();

    barSet = new QBarSet("number");

//    *barSet << 5 << 6 << 7 << 8;

    for(int i = 0; i < 144; i++){
        *barSet << i;
    }

    barSeries->append(barSet);
}

bool DataAnalysisScene::event(QEvent *e)
{
    //如果是鼠标按下 ，在event事件分发中做拦截操作
    if(e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent * ev  = static_cast<QMouseEvent *>(e);

        if(ev->x() >= 232 && ev->x() <= 770 && ev->y() <= 712 && ev->y() >= 173){
            int gridX = (ev->x() - 231) / 54;
            int gridY = (712 - ev->y()) / 54;
            int gridID = gridX+ gridY * 10;

            QString str = QString( "Event函数中：：鼠标按下了 x = %1   y = %2  globalX = %3 globalY = %4 " ).arg(ev->x()).arg(ev->y()).arg(ev->globalX()).arg(ev->globalY());
            qDebug() << str << "gridID:" << gridID;

            auto returnPixmap = iconLabel[gridID]->pixmap(Qt::ReturnByValue);
            if(returnPixmap == pixmap_choosen){
                iconLabel[gridID]->setPixmap(pixmap_not_choosen);
            } else {
                iconLabel[gridID]->setPixmap(pixmap_choosen);
            }


            return true; //true代表用户自己处理这个事件，不向下分发
        }
    }


    //其他事件 交给父类处理  默认处理
    return QMainWindow::event(e);
}

void DataAnalysisScene::initAnalyseDemmandPattern()
{
    if(!chart){
        chart = new QChart;
        chartView = new QChartView(chart);
        chartView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    } else {
        delete series;
        delete axisX;
        delete axisY;
        chart->removeAllSeries();
    }

    series = new QSplineSeries;

    int beginDay = ui->dateEdit_begin1->date().day();
    int endDay = ui->dateEdit_end1->date().day() + 1;
    int timeStep = ui->spinBox->value() * 3600;

    int beginUTime = timeToUnixTime(beginDay);
    int endUTime = timeToUnixTime(endDay);

    QSqlQuery query;
    int step_begin = beginUTime;
    int step_end = beginUTime + timeStep;

    int choosen[100] = {-1};
    for(int i = 0; i < 100; i++){
        auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
        if(pixmapReturn == pixmap_choosen){
            choosen[i] = 1;
        } else {
            choosen[i] = 0;
        }
    }

    for(int i = (beginDay - 1) * 24; i < (endDay - 1) * 24; i += ui->spinBox->value() ){
        int sumInPeriod = 0;
        for(int j = i; j < i + ui->spinBox->value(); j++){
//            sumInPeriod += numOfOrders[j];
            for(int k = 0; k < 100; k++){
                if(choosen[k]){
                    sumInPeriod += numOfOrdersPerGridPerHour[k][j];
                }
            }
        }
        qDebug() << unixTimeToTime(step_begin) << unixTimeToTime(step_end) << sumInPeriod;

        series->append(unixTimeToTime(step_begin).toMSecsSinceEpoch(), sumInPeriod);

        step_begin = step_end;
        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
    }

//    while(true){
//        QString command = QString("select count(*) from orders where btime >= %1 and etime < %2").arg(step_begin).arg(step_end);
//        query.exec(command);
//        if(query.next()){
//            auto _begin_datetime = unixTimeToTime(step_begin);
//            auto _end_datetime = unixTimeToTime(step_end);

//            qDebug() << _begin_datetime << unixTimeToTime(step_end) << query.value(0).toInt();

//            series->append(_begin_datetime.toMSecsSinceEpoch(), query.value(0).toInt());
//        } else {
//            qDebug() << "Error!";
//        }

//        if(step_end == endUTime) break;

//        step_begin = step_end;
//        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
//    }

    axisX = new QDateTimeAxis;
    axisX->setRange(QDateTime(QDate(2016, 11, beginDay), QTime(0, 0)), QDateTime(QDate(2016, 11, endDay), QTime(0, 0)));
    axisX->setFormat("MM-dd");
    axisX->setGridLineVisible(true);
    axisX->setTickCount(endDay - beginDay + 1);
//    axisX->setMinorTickCount(1);
    axisX->setTitleText("X");

    axisY = new QValueAxis;
    axisY->setRange(0,300000);
//    axisY->setLabelFormat("%d");
//    axisY->setGridLineVisible(true);
//    axisY->setTickCount(11);
//    axisY->setMinorTickCount(1);
//    axisY->setTitleText("Y");

    series->setPen(QPen(Qt::green, 3 ,Qt::SolidLine));
//    for(int i = 0; i < )



//    series->setPointLabelsFont(QFont("微软雅黑", 11));
    series->setPointLabelsFormat("@yPoint");
    series->setPointsVisible();
    series->setPointLabelsVisible();
    series->setPointLabelsClipping(false);

    chart->setTitle("曲线图实例");
//    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chartView->setRenderHints(QPainter::Antialiasing);

    ui->horizontalLayout_8->addWidget(chartView);
}

void DataAnalysisScene::displayInBarSet()
{


}

void DataAnalysisScene::preLoadNumOfOrders(){
    flag_numOfOrders = true;
    int beginDay = 1;
    int endDay = 16;
    int timeStep = 3600;

    int beginUTime = timeToUnixTime(beginDay);
    int endUTime = timeToUnixTime(endDay);

    QSqlQuery query;
    int step_begin = beginUTime;
    int step_end = beginUTime + timeStep;

    int hourIdx = 0;

    while(true){
        QString command = QString("select count(*) from orders where btime >= %1 and etime < %2").arg(step_begin).arg(step_end);
        query.exec(command);
        if(query.next()){
            auto _begin_datetime = unixTimeToTime(step_begin);
            auto _end_datetime = unixTimeToTime(step_end);

            qDebug() << _begin_datetime << unixTimeToTime(step_end) << query.value(0).toInt();

            numOfOrders[hourIdx++] = query.value(0).toInt();
        } else {
            qDebug() << "Error!";
        }

        if(step_end == endUTime) break;

        step_begin = step_end;
        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
    }
}

QDateTime DataAnalysisScene::unixTimeToTime(int uTime)
{
    //    QDate unixStart(1970, 1, 1);
    //    QDate datasetStart(2016, 11, 1);
    //    int daysTo = unixStart.daysTo(datasetStart);
    //    int secondsTo = daysTo * 24 * 3600;
    int secondsTo = 1477958400;

    int dTime = uTime + 8 * 3600;

    int day = (dTime - secondsTo) / 3600 / 24 + 1;
    int hour = (dTime - secondsTo) / 3600 % 24;
    int min = ((dTime - secondsTo) - (day - 1) * 3600 * 24 - hour * 3600) / 60;
    int second = ((dTime - secondsTo) - (day - 1) * 3600 * 24 - hour * 3600) % 60;

    return QDateTime(QDate(2016, 11, day), QTime(hour, min, second));
}

int DataAnalysisScene::timeToUnixTime(int day, int hour, int min, int second)
{
    //    QDate unixStart(1970, 1, 1);
    //    QDate datasetStart(2016, 11, 1);
    //    int daysTo = unixStart.daysTo(datasetStart);
    //    int secondsTo = daysTo * 24 * 3600;
    int secondsTo = 1477958400;

    int uTime = secondsTo + (day - 1) * 3600 * 24 + hour * 3600 + min * 60 + second - 8 * 3600;
    return uTime;
}


DataAnalysisScene::~DataAnalysisScene()
{
    delete ui;
}
