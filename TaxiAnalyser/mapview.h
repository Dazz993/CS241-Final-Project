#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QLabel>

namespace Ui {
class mapView;
}

class mapView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit mapView(QWidget *parent = nullptr);
    ~mapView();

    int status;                 // status, 0: not use mouse to localize, 1: localize the origin, 2: localizate the destination

    QGraphicsPixmapItem * item;
    QGraphicsScene * mapScene;

    QPixmap startPixmap;
    QPixmap endPixmap;

    QLabel startLabel;
    QLabel endLabel;

    void setPoints(bool start, bool end, double slon = 0, double slat = 0,  double elon = 0, double elat = 0);
    void displayMap();
    void initStartEndPointPixmap();

    void mousePressEvent(QMouseEvent *event);
private:
    Ui::mapView *ui;
};

#endif // MAPVIEW_H
