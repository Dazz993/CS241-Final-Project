#ifndef TRAVELTIMEPIECHART_H
#define TRAVELTIMEPIECHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QPieSeries>
#include <QPieSlice>

class travelTimePieChart : public QChartView
{
    Q_OBJECT
public:
    explicit travelTimePieChart(QWidget *parent = nullptr);

    QChart * chart = NULL;
    QPieSeries * series = NULL;

    void displayPieChart(int *data, int length, int timestep, int count);
signals:

};

#endif // TRAVELTIMEPIECHART_H
