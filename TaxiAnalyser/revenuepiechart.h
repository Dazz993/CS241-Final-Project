#ifndef REVENUEPIECHART_H
#define REVENUEPIECHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QPieSeries>
#include <QPieSlice>

class revenuePieChart : public QChartView
{
    Q_OBJECT
public:
    explicit revenuePieChart(QWidget *parent = nullptr);

    QChart * chart = NULL;
    QPieSeries * series = NULL;

    void displayPieChart(int *data, int length, int timestep, int count, QDate date, QTime start, QTime end);

signals:

};

#endif // REVENUEPIECHART_H
