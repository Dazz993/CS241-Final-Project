#include "revenuebarchart.h"

revenueBarChart::revenueBarChart(QWidget *parent) : QChartView(parent)
{
    db = QSqlDatabase::database();

    barChart = new QChart;
    barSeries = new QBarSeries;
    barChart->addSeries(barSeries);
}

void revenueBarChart::displayRevenueDistribution(QDate date, QTime start, QTime end, int timestep)
{
    const int NUM_DATA = 600;

    if(barAxisX) delete barAxisX;
    if(barAxisY) delete barAxisY;
    barSeries->clear();

    memset(data, 0, NUM_DATA * sizeof(int));
    count = 0;

    barChart->setTitleFont(QFont("Tahoma", 15, 75));
    barChart->setTitle(QString("Revenue distribution from %2 to %3 on %1").arg(date.toString("yyyy-MM-dd")).arg(QTime(0, 0).toString("hh:mm")).arg(QTime(23, 59).toString("hh:mm")));

    auto barSet = new QBarSet(QString("Revenue distribution from %2 to %3 on %1").arg(date.toString("yyyy-MM-dd")).arg(QTime(0, 0).toString("hh:mm")).arg(QTime(23, 59).toString("hh:mm")));
    barSet->setColor(QColor(99, 192, 135));
    barSeries->append(barSet);

    QStringList cat;

    int beginTimeUnix = QDateTime(date, start).toSecsSinceEpoch();
    int endTimeUnix = QDateTime(date, end).toSecsSinceEpoch();
    timestep *= 60;

    if(beginTimeUnix >= endTimeUnix) return;

    int maxValue = 0;

    for(int i = 0; i < NUM_DATA; i++){
        data[i] = 0;
    }

    QSqlQuery query(db);

    QString subItem = QString("(btime - %1) / %2").arg(beginTimeUnix).arg(timestep);

    auto command = QString("SELECT %1, count(fee) \n"
                            "FROM orders \n"
                            "WHERE btime > %2 AND btime < %3 \n"
                            "GROUP BY %1 \n"
                            "ORDER BY %1 \n").arg(subItem).arg(beginTimeUnix).arg(endTimeUnix);

    auto isOk = query.exec(command);
    if(!isOk){
        qDebug() << "QUERY IN TIME DISTRIBUTION ERROR:\n1. command:" << query.lastQuery() << "\n2. error message:" << query.lastError().text();
    }

    while(query.next()){
//        qDebug() << query.value(0).toInt() << query.value(1).toInt();
        data[query.value(0).toInt()] = query.value(1).toInt();
        count += query.value(1).toInt();
    }

    std::vector<int> t(data, data + NUM_DATA);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    for(int i = 0; i < (endTimeUnix - beginTimeUnix) / timestep + 1; i++){
        QTime beginTime = QDateTime::fromSecsSinceEpoch(beginTimeUnix + i * timestep).time();
        QTime endTime = QDateTime::fromSecsSinceEpoch(std::min(endTimeUnix, beginTimeUnix + (i+1) * timestep)).time();

        if(beginTime == endTime) break;

        cat << QString("%1~%2").arg(beginTime.toString("hh:mm")).arg(endTime.toString("hh:mm"));
        *barSet << data[i];
//        qDebug() << "Inserting:" << QString("%1~%2").arg(beginTime.toString("hh:mm")).arg(endTime.toString("hh:mm")) << data[i];
    }

    maxValue = t[0];

    barAxisX = new QBarCategoryAxis;
    barAxisX->append(cat);
    barAxisX->setLabelsFont(QFont("Helvetica", 12));
    barAxisX->setLabelsAngle(90);
    barAxisX->setGridLineVisible(false);
    barAxisX->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisX->setTitleText("Revenue distribution analysis time period / minutes");
    barChart->addAxis(barAxisX, Qt::AlignBottom);
    barSeries->attachAxis(barAxisX);

    barAxisY = new QValueAxis;
    barAxisY->setLabelsFont(QFont("Helvetica", 13));
    barAxisY->setLabelFormat("%d");
    barAxisY->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisY->setTitleText("Revenue of time period");
    int rangeMax = 0;
    if(maxValue < 5000){
        rangeMax = 5000;
    } else if(maxValue < 10000){
        rangeMax = 10000;
    } else if(maxValue < 20000){
        rangeMax = 20000;
    }
    barAxisY->setRange(0, rangeMax);
    barAxisY->setTickCount(6);
    barChart->addAxis(barAxisY, Qt::AlignLeft);
    barSeries->attachAxis(barAxisY);

    barChart->legend()->setVisible(false);
//    travelTimeChart->legend()->setAlignment(Qt::AlignBottom);

    this->setChart(barChart);
}
