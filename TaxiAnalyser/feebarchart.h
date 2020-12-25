#ifndef FEEBARCHART_H
#define FEEBARCHART_H

#include <QWidget>
#include <QChartView>
#include <QtCharts>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

class feeBarChart : public QChartView
{
    Q_OBJECT
public:
    explicit feeBarChart(QWidget *parent = nullptr);

    QSqlDatabase db;
    int data[600];
    int count;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

    void displayFeeDistribution(QDate date, QTime start, QTime end, double feespan);

signals:

};

#endif // FEEBARCHART_H
