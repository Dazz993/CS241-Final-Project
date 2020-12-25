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
#include <QSqlDatabase>
#include <QPieSeries>
#include <QPieSlice>

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

//     Parameters definition
//    bool feeSelected;
//    int beginDayLoaded;
//    int endDayLoaded;

    QChart * splineChart = NULL;
    QDateTimeAxis * splineAxisX = NULL;
    QValueAxis * splineAxisY = NULL;

    QChart * barChart = NULL;
    QBarSeries * barSeries = NULL;
    QBarCategoryAxis * barAxisX = NULL;
    QValueAxis * barAxisY = NULL;

//    QChart * travelTimeChart = NULL;
//    QBarSeries * travelTimeSeries = NULL;
//    QBarCategoryAxis * travelTimeAxisX = NULL;
//    QValueAxis * travelTimeAxisY = NULL;

//    QLabel * mapLabel = NULL;

//    QLabel * iconLabel[100];

//    QSqlDatabase db;

//    void initValues();
//    void initMapLabels();
//    void enableMapLabels();
//    void disableMapLabels();


    void displayInBarChart();
    void displayInSplineChart();
//    void preLoadNumOfOrders();

//    void analyseTravelTime();
//    void analyseFee();
//    void analyseRevenue();

//    void querySimilarOrders();

//    void setDayRange();

//    QPixmap pixmap_choosen;
//    QPixmap pixmap_not_choosen;
//    QPixmap map;

    QPixmap startPixmap;
    QPixmap endPixmap;

//    static QDateTime unixTimeToTime(int uTime);
//    static int timeToUnixTime(int day, int hour = 0, int min = 0, int second = 0);

////    bool event(QEvent *e);

//    void initWidgets();



    // Flags or parameters
    bool feeSelected;
    QDate beginDayLoaded;
    QDate endDayLoaded;

    // Data
    int numOfBeginOrdersPerGridPerHalfHour[100][2 * 24 * 15];
    int numOfEndOrdersPerGridPerHalfHour[100][2 * 24 * 15];


    // Front end initialization
    void initFrontEndAndConnection();
    void initAnalyseOrderDemandPatternInADay();
    void initAnalyseOrderDemandPatternInSeveralDays();
    void initAnalyseDistributionOfTravelTime();
    void initAnalyseDistributionOfFee();
    void initAnalyseDistributionOfRevenue();
    void initPredictTravelTime();
    void initPredictDestination();
    void initQuerySimilarOrders();
private:
    Ui::DataAnalysisScene *ui;
};

#endif // DATAANALYSISSCENE_H
