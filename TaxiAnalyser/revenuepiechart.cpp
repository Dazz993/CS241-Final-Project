#include "revenuepiechart.h"

revenuePieChart::revenuePieChart(QWidget *parent) : QChartView(parent)
{
    chart = new QChart();
    series = new QPieSeries();
    chart->setFont(QFont("Tahoma", 15, 75));
    chart->addSeries(series);
}

void revenuePieChart::displayPieChart(int *data, int length, int timestep, int count, QDate date, QTime start, QTime end)
{
    series->clear();

    std::vector<int> t(data, data + length);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    int beginTimeUnix = QDateTime(date, start).toSecsSinceEpoch();
    int endTimeUnix = QDateTime(date, end).toSecsSinceEpoch();
    timestep *= 60;

    int first_n_count = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < length; j++) {
            if(t[i] == data[j]){
                QTime beginTime = QDateTime::fromSecsSinceEpoch(beginTimeUnix + j * timestep).time();
                QTime endTime = QDateTime::fromSecsSinceEpoch(std::min(endTimeUnix, beginTimeUnix + (j+1) * timestep)).time();
                series->append(QString("%1~%2 : %3").arg(beginTime.toString("hh:mm")).arg(endTime.toString("hh:mm")).arg(QString::number(t[i])), t[i] * 100.0 / count);
                first_n_count += t[i];
                break;
            }
        }
//        QPieSlice *slice = series->slices().at(i);
//        slice->setExploded();
//        slice->setLabelVisible();
    }

    series->append("Other", (count - first_n_count) * 100.0 / count);

    QPieSlice *slice = series->slices().at(0);
    slice->setExploded();
    slice->setLabelVisible();
//    QPieSlice *slice_1 = series->slices().at(1);
//    slice_1->setExploded();
//    slice_1->setLabelVisible();

    chart->setTitleFont(QFont("Tahoma", 15, 75));
    chart->setTitle(QString("Revenue distribution in Pie chart"));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setContentsMargins(0,0,0,0);
    chart->legend()->setWindowFrameMargins(0,0,0,0);
    chart->legend()->hide();

    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
}
