#include "predictduringview.h"
#include "ui_predictduringview.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

predictDuringView::predictDuringView(QWidget *parent) :
    QChartView(parent),
    ui(new Ui::predictDuringView)
{
    ui->setupUi(this);

    db = QSqlDatabase::database();

    barChart = new QChart;
    barSeries = new QBarSeries;
    barChart->addSeries(barSeries);
}

void predictDuringView::predictDuringTime(bool useDepatureTime, double dlat, double dlng, double elat, double elng, QTime departureTime)
{
    if(barAxisX) delete barAxisX;
    if(barAxisY) delete barAxisY;
    barSeries->clear();

    auto barSet = new QBarSet("The number of order in a period of time");
    barSet->setColor(QColor(99, 192, 135));
    barSeries->append(barSet);

    QStringList cat;

    double lngDelta = 0.03;
    double latDelta = 0.06;
    double timeDelta = 500;

    QString command;

    const int BASETIME = QDateTime(QDate(2016, 11, 1), QTime(0, 0)).toSecsSinceEpoch();
    const int BASE_BEGIN = QDateTime(QDate(2016, 11, 1), departureTime).toSecsSinceEpoch() - BASETIME;

    QString subitem1 = QString("((btime - %1) % 86400 > %2)").arg(BASETIME).arg(BASE_BEGIN - timeDelta);
    QString subitem2 = QString("((btime - %1) % 86400 < %2)").arg(BASETIME).arg(BASE_BEGIN + timeDelta);

//    if(useDepatureTime){
//        command = QString("SELECT during / 60, count(during / 60) \n"
//                                "FROM orders \n"
//                                "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8 AND btime > %9 AND btime < %10\n"
//                                "GROUP BY during / 60 \n"
//                                "ORDER BY during / 60 \n").arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
//                                                          .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta) \
//                                                          .arg(QString::number(departureTime.toSecsSinceEpoch() - timeDelta, 'f', 0)).arg(QString::number(departureTime.toSecsSinceEpoch() + timeDelta, 'f', 0));
//    } else {
//        command = QString("SELECT during / 60, count(during / 60) \n"
//                                "FROM orders \n"
//                                "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8\n"
//                                "GROUP BY during / 60 \n"
//                                "ORDER BY during / 60 \n").arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
//                                                          .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta);
//    }

    if(useDepatureTime){
        command = QString("SELECT during / 60, count(during / 60) \n"
                                "FROM orders \n"
                                "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8 AND %9 AND %10\n"
                                "GROUP BY during / 60 \n"
                                "ORDER BY during / 60 \n").arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
                                                          .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta) \
                                                          .arg(subitem1).arg(subitem2);
    } else {
        command = QString("SELECT during / 60, count(during / 60) \n"
                                "FROM orders \n"
                                "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8\n"
                                "GROUP BY during / 60 \n"
                                "ORDER BY during / 60 \n").arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
                                                          .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta);
    }

    int maxValue = 0;
    int t[1500] = {0};

    QSqlQuery query(db);
    auto isOk = query.exec(command);

//    qDebug() << command;


    if(!isOk){
        qDebug() << "SQL exec error!\n1. The command:" << query.lastQuery() << "\n2. The error: " << query.lastError().text();
    }

    while(query.next()){
//        qDebug() << query.value(0).toInt() << query.value(1).toInt() << query.value(2).toInt();
        t[query.value(0).toInt()] = query.value(1).toInt();
    }


    int beginIndex = 0, endIndex = 0;
    for(int i = 0; i < 1500; i++){
        if(t[i] == 0) continue;
        if(t[i] > maxValue) maxValue = t[i];
        if(beginIndex == 0) beginIndex = i;
        endIndex = i;
    }

//    qDebug() << beginIndex << endIndex;

    for(int i = beginIndex; i <= endIndex; i++){
        if(i <= 1) continue;
        if(i - beginIndex >= 37) break;
        *barSet << t[i];
        cat << QString("%1mins").arg(i);
    }

    barAxisX = new QBarCategoryAxis;
    barAxisX->append(cat);
    int temp_count = endIndex - beginIndex + 1;
    if(temp_count >= 32){
        barAxisX->setLabelsFont(QFont("Tahoma", 8));
    } else if(temp_count >= 28){
        barAxisX->setLabelsFont(QFont("Tahoma", 8));
    } else if(temp_count >= 24){
        barAxisX->setLabelsFont(QFont("Tahoma", 12));
    }
    barAxisX->setLabelsAngle(90);
    barAxisX->setGridLineVisible(false);
    barAxisX->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisX->setTitleText("Travel time / minutes");
    barChart->addAxis(barAxisX, Qt::AlignBottom);
    barSeries->attachAxis(barAxisX);

    barAxisY = new QValueAxis;
    barAxisY->setLabelsFont(QFont("Tahoma", 13));
    barAxisY->setLabelFormat("%d");
    int maxAxisYValue = 0;

    if(maxValue < 5){
        maxAxisYValue = 5;
    } else if (maxValue < 10){
        maxAxisYValue = 10;
    } else if(maxValue < 30){
        maxAxisYValue = 30;
    } else if(maxValue < 50){
        maxAxisYValue = 50;
    } else if (maxValue < 100){
        maxAxisYValue = 100;
    } else if (maxValue < 300){
        maxAxisYValue = 300;
    } else if (maxValue < 500){
        maxAxisYValue = 500;
    } else if (maxValue < 1000){
        maxAxisYValue = 1000;
    } else {
        maxAxisYValue = (maxValue / 100) * 100 + 100;
    }

//    qDebug() << "MAX VALUE:" << maxValue << "maxAxisValue" << maxAxisYValue;

    barAxisY->setRange(0, maxAxisYValue);
    barAxisY->setTickCount(6);
    barAxisY->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisY->setTitleText("Possibility / %");
    barChart->addAxis(barAxisY, Qt::AlignLeft);
    barSeries->attachAxis(barAxisY);

    barChart->setTitleFont(QFont("Tahoma", 15, 75));
    barChart->setTitle(QString("Prediction of the travel time"));
    barChart->legend()->setVisible(false);
//    barChart->legend()->setAlignment(Qt::AlignBottom);
    this->setRenderHint(QPainter::Antialiasing);
    this->setChart(barChart);


    // Get the result
    // Get the average during
    if(useDepatureTime){
        command = QString("SELECT avg(during) \n"
                              "FROM orders \n"
                              "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8 AND %9 AND %10\n"
                                ).arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
                                                        .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta) \
                                                        .arg(subitem1).arg(subitem2);
    } else {
        command = QString("SELECT avg(during) \n"
                              "FROM orders \n"
                              "WHERE blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 AND elng > %5 AND elng < %6 AND elat > %7 AND elat < %8\n"
                                ).arg(dlng - lngDelta).arg(dlng + lngDelta).arg(dlat - latDelta).arg(dlat + latDelta) \
                                                        .arg(elng - lngDelta).arg(elng + lngDelta).arg(elat - latDelta).arg(elat + latDelta);
    }

    isOk = query.exec(command);
    if(!isOk){
        qDebug() << "SQL exec error!\n1. The command:" << query.lastQuery() << "\n2. The error: " << query.lastError().text();
    }
    query.next();
    double avg = query.value(0).toFloat();

//    command = "select during from orders where (blng - %1) * (blng - %1) + (blat - %2) * (blat - %2) IN \
//                (SELECT min((blng - %1) * (blng - %1) + (blat - %2) * (blat - %2)) from orders)";

    // Get the most similar time



    result = QString("Time cost from the origin to the destination: %1 minutes").arg(QString::number(query.value(0).toFloat() / 60, 'f', 2));
}

predictDuringView::~predictDuringView()
{
    delete ui;
}
