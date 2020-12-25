#include "feepiechart.h"

feePieChart::feePieChart(QWidget *parent) : QChartView(parent)
{
    chart = new QChart();
    series = new QPieSeries();
    chart->setFont(QFont("Tahoma", 15, 75));
    chart->setTitle(QString("Fee distribution in Pie chart"));
    chart->addSeries(series);
}

void feePieChart::displayPieChart(int *data, int length, double feespan, int count)
{
    series->clear();

    std::vector<int> t(data, data + length);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    int first_n_count = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < length; j++) {
            if(t[i] == data[j]){
                series->append(QString("%1~%2 : %3").arg(j * feespan).arg((j+1) * feespan).arg(QString::number(t[i] * 100.0 / count, 'f', 2)), t[i] * 100.0 / count);
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

    chart->setTitleFont(QFont("Tahoma", 15, 75));
    chart->setTitle(QString("Fee distribution in Pie chart"));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setContentsMargins(0,0,0,0);
    chart->legend()->setWindowFrameMargins(0,0,0,0);
    chart->legend()->hide();

    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
}
