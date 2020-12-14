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

#define BASETIME 1477929600

namespace Ui {
class DataAnalysisScene;
}

class DataAnalysisScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataAnalysisScene(QWidget *parent = nullptr);
    ~DataAnalysisScene();

    // Parameters definition
    bool feeSelected;
    int beginDayLoaded;
    int endDayLoaded;

    QChart * splineChart = NULL;
    QDateTimeAxis * splineAxisX = NULL;
    QValueAxis * splineAxisY = NULL;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

    QLabel * mapLabel = NULL;

    QLabel * iconLabel[100];

    void initValues();
    void initMapLabels();

    void displayInSplineChart();
    void displayInBarChart();
    void preLoadNumOfOrders();

    void setDayRange();

    QPixmap pixmap_choosen;
    QPixmap pixmap_not_choosen;

    static QDateTime unixTimeToTime(int uTime);
    static int timeToUnixTime(int day, int hour = 0, int min = 0, int second = 0);

    bool event(QEvent *e);

    int numOfBeginOrdersPerGridPerHour[100][2 * 24 * 15];
    int numOfEndOrdersPerGridPerHour[100][2 * 24 * 15];


private:
    Ui::DataAnalysisScene *ui;
};

#endif // DATAANALYSISSCENE_H
