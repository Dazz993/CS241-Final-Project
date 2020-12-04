#include "dataanalysisscene.h"
#include "ui_dataanalysisscene.h"

DataAnalysisScene::DataAnalysisScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataAnalysisScene)
{
    ui->setupUi(this);

    flag_numOfOrders = false;

    for(int i = 0; i < 24 * 15; i++){
        numOfOrders[i] = 0;
    }

    connect(ui->btn_analyse, &QPushButton::clicked, [=](){
//        if(!flag_numOfOrders) preLoadNumOfOrders();
        initAnalyseDemmandPattern();
    });
}

void DataAnalysisScene::initAnalyseDemmandPattern()
{
    if(!chart){
        chart = new QChart;
        chartView = new QChartView(chart);
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

    for(int i = (beginDay - 1) * 24; i < (endDay - 1) * 24; i += ui->spinBox->value() ){
        int sumInPeriod = 0;
        for(int j = i; j < i + ui->spinBox->value(); j++){
            sumInPeriod += numOfOrders[j];
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

    chart->setTitle("曲线图实例");
//    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chartView->setRenderHints(QPainter::Antialiasing);

    ui->verticalLayout_4->addWidget(chartView);
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
