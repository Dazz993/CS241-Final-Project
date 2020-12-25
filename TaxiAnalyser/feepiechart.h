#ifndef FEEPIECHART_H
#define FEEPIECHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QPieSeries>
#include <QPieSlice>
class feePieChart : public QChartView
{
    Q_OBJECT
public:
    explicit feePieChart(QWidget *parent = nullptr);

    QChart * chart = NULL;
    QPieSeries * series = NULL;

    void displayPieChart(int *data, int length, double feespan, int count);

signals:

};

#endif // FEEPIECHART_H
