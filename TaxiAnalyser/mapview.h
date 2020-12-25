#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class mapView;
}

class mapView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit mapView(QWidget *parent = nullptr);
    ~mapView();

    struct Point
    {
        int posX;
        int posY;
        int radius;
        int count;
    };

    void displayMap();
    void initStartEndPointPixmap();

    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void setPoints(bool start, bool end, double slon = 0, double slat = 0,  double elon = 0, double elat = 0);
    void setStatus(int status);
    void setHeatMapFlag(bool activate);


    void displayHeatMap();
    void addPoints(int x, int y, int count);
    //最大权重改变之后重新绘制dataimg
    void drawDataImg();
    //根据dataimg绘制热力图
    void drawHeatImg();

    //清空
    void clear();

    void initHeatMap();

    int status;                 // status, 0: not use mouse to localize, 1: localize the origin, 2: localizate the destination
    bool heatMapActivate;
    QGraphicsPixmapItem * item;
    QGraphicsScene * mapScene;

    QSqlDatabase db;
    QPixmap startPixmap;
    QPixmap endPixmap;

    QLabel startLabel;
    QLabel endLabel;

//     Varibals for heat map
    const int radius = 25;
    int HeatAlpha = 200;
    int ImgWidth;
    int ImgHeight;
    int _maxCount=1;
    int _totalCount = 0;
    int _maxPosX;
    int _maxPosY;
    int *_countTable;
    bool *_flagTable;

    QList<Point> _posList;
    QList<Point> _resList;
    QImage _dataImg;
    QImage _heatImg;
    QRgb _colorList[256];

    void addData(double lat, double lng, bool use_depature_time, QTime depature_time);
signals:
    void startPosChanged(double lat, double lng);
    void endPosChanged(double lab, double lng);

private:
    Ui::mapView *ui;


};

#endif // MAPVIEW_H
