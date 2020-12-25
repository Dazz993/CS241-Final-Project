#include "feebarchart.h"
#include <QDebug>

feeBarChart::feeBarChart(QWidget *parent) : QChartView(parent)
{
    db = QSqlDatabase::database();

    barChart = new QChart;
    barSeries = new QBarSeries;
    barChart->addSeries(barSeries);
}

void feeBarChart::displayFeeDistribution(QDate date, QTime start, QTime end, double feespan)
{
    if(barAxisX) delete barAxisX;
    if(barAxisY) delete barAxisY;
    barSeries->clear();
    memset(data, 0, 600 * sizeof(int));
    count = 0;

    barChart->setTitleFont(QFont("Tahoma", 15, 75));
    barChart->setTitle(QString("Fee distribution from %2 to %3 on %1").arg(date.toString("yyyy-MM-dd")).arg(QTime(0, 0).toString("hh:mm")).arg(QTime(23, 59).toString("hh:mm")));

    auto barSet = new QBarSet(QString("Fee distribution from %2 to %3 on %1").arg(date.toString("yyyy-MM-dd")).arg(QTime(0, 0).toString("hh:mm")).arg(QTime(23, 59).toString("hh:mm")));
    barSet->setColor(QColor(99, 192, 135));
    barSeries->append(barSet);

    QStringList cat;

    int beginTimeUnix = QDateTime(date, start).toSecsSinceEpoch();
    int endTimeUnix = QDateTime(date, end).toSecsSinceEpoch();

//    qDebug() << beginTimeUnix << endTimeUnix;

    if(beginTimeUnix >= endTimeUnix) return;

    int maxValue = 0;

    for(int i = 0; i < 600; i++){
        data[i] = 0;
    }

    QSqlQuery query(db);

    auto command = QString("SELECT round(fee / %3), count(round(fee / %3)) \n"
                        "FROM orders \n"
                        "WHERE btime > %1 AND btime < %2 \n"
                        "GROUP BY round(fee / %3) \n"
                        "ORDER BY round(fee / %3) \n").arg(beginTimeUnix).arg(endTimeUnix).arg(QString::number(feespan, 'f', 2));

    auto isOk = query.exec(command);
    if(!isOk){
        qDebug() << "QUERY IN TIME DISTRIBUTION ERROR:\n1. command:" << query.lastQuery() << "\n2. error message:" << query.lastError().text();
    }

    while(query.next()){
        data[query.value(0).toInt()] = query.value(1).toInt();
        count += query.value(1).toInt();
    }

    std::vector<int> t(data, data + 600);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

//    qDebug() << count;

    for(int i = 0; i < 24; i++){
        cat << QString("%1~%2 ").arg(i * feespan).arg((i+1) * feespan);
        *barSet << data[i] * 100.0 / count;
    }

    maxValue = t[0] * 100 / count;

    barAxisX = new QBarCategoryAxis;
    barAxisX->append(cat);
    barAxisX->setLabelsFont(QFont("Helvetica", 12));
    barAxisX->setLabelsAngle(90);
    barAxisX->setGridLineVisible(false);
    barAxisX->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisX->setTitleText("Fee interval");
    barChart->addAxis(barAxisX, Qt::AlignBottom);
    barSeries->attachAxis(barAxisX);

    barAxisY = new QValueAxis;
    barAxisY->setLabelsFont(QFont("Helvetica", 13));
    barAxisY->setLabelFormat("%d");
    barAxisY->setTitleFont(QFont("Tahoma", 12, 75));
    barAxisY->setTitleText("Percentage of fee in the corresponding interval / \%");
    int rangeMax = (maxValue / 5) * 5 + 5;
    barAxisY->setRange(0, rangeMax);
    barAxisY->setTickCount(6);
    barChart->addAxis(barAxisY, Qt::AlignLeft);
    barSeries->attachAxis(barAxisY);

    barChart->legend()->setVisible(false);
//    travelTimeChart->legend()->setAlignment(Qt::AlignBottom);

    this->setChart(barChart);
}
