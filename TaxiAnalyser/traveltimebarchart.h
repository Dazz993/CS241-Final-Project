#ifndef TRAVELTIMEBARCHART_H
#define TRAVELTIMEBARCHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#define NUM_DATA 600

class travelTimeBarChart : public QChartView
{
    Q_OBJECT
public:
    explicit travelTimeBarChart(QWidget *parent = nullptr);

    QSqlDatabase db;
    int data[NUM_DATA];
    int count;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

    void displayTravelTimeDistribution(QDate date, QTime start, QTime end, int timestep);
signals:

};

#endif // TRAVELTIMEBARCHART_H
