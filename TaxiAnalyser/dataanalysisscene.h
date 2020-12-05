#ifndef DATAANALYSISSCENE_H
#define DATAANALYSISSCENE_H

#include <QMainWindow>
#include <QtCharts>
#include <QChartView>
#include <QSplineSeries>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QIcon>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>

namespace Ui {
class DataAnalysisScene;
}

class DataAnalysisScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataAnalysisScene(QWidget *parent = nullptr);
    ~DataAnalysisScene();

    QChart * chart = NULL;
    QChartView * chartView = NULL;
    QSplineSeries * series = NULL;
    QDateTimeAxis * axisX = NULL;
    QValueAxis * axisY = NULL;

    QChart * barChart = NULL;
    QChartView * barChartView = NULL;
    QBarSeries * barSeries = NULL;
    QBarSet * barSet = NULL;
    QBarCategoryAxis *barCatAxis = NULL;


    QLabel * mapLabel = NULL;

    QLabel * iconLabel[100];

    void initAnalyseDemmandPattern();
    void displayInBarSet();
    void preLoadNumOfOrders();

    QPixmap pixmap_choosen;
    QPixmap pixmap_not_choosen;

    static QDateTime unixTimeToTime(int uTime);
    static int timeToUnixTime(int day, int hour = 0, int min = 0, int second = 0);

    bool event(QEvent *e);

    bool flag_numOfOrders;
    int numOfOrders[24 * 15];
    int numOfOrdersPerGridPerHour[100][24 * 15];

private:
    Ui::DataAnalysisScene *ui;
};

#endif // DATAANALYSISSCENE_H
