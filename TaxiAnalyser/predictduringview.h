#ifndef PREDICTDURINGVIEW_H
#define PREDICTDURINGVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QChartView>
#include <QChart>
#include <QtCharts>
#include <QSqlDatabase>

namespace Ui {
class predictDuringView;
}

class predictDuringView : public QChartView
{
    Q_OBJECT

public:
    explicit predictDuringView(QWidget *parent = nullptr);

    QSqlDatabase db;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

    QString result;

    ~predictDuringView();

    void predictDuringTime(bool useDepatureTime, double dlat, double dlng, double elat, double elng, QTime departureTime);
private:
    Ui::predictDuringView *ui;
};

#endif // PREDICTDURINGVIEW_H
