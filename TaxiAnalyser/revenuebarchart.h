#ifndef REVENUEBARCHART_H
#define REVENUEBARCHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

class revenueBarChart : public QChartView
{
    Q_OBJECT
public:
    explicit revenueBarChart(QWidget *parent = nullptr);

    QSqlDatabase db;
    int data[600];
    int count;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

    void displayRevenueDistribution(QDate date, QTime start, QTime end, int timestep);


signals:

};

#endif // REVENUEBARCHART_H
