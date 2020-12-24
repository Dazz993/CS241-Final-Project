#include "mapview.h"
#include "ui_mapview.h"
#include <QDebug>
#include <QPainter>

mapView::mapView(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::mapView)
{
    ui->setupUi(this);

    status = 0;

    displayMap();
    initStartEndPointPixmap();
}

void mapView::setPoints(bool start, bool end, double slon, double slat,  double elon, double elat){
    double sx = (576 - 24) / (104.222 - 103.909) * (slon - 103.909) + 24;
    double sy = (588.0 - 23.0) / (30.524 - 30.794) * (slat - 30.794) + 23 - 11;
    double ex = (576 - 24) / (104.222 - 103.909) * (elon - 103.909) + 24;
    double ey = (588.0 - 23.0) / (30.524 - 30.794) * (elat - 30.794) + 23 - 11;

    if(start){
        startLabel.move(sx - startPixmap.width() / 2, sy - startPixmap.height() / 2);
        startLabel.setVisible(true);
    } else {
        startLabel.setVisible(false);
    }

    if(end){
        endLabel.move(ex - endPixmap.width() / 2, ey - endPixmap.height() / 2);
        endLabel.setVisible(true);
    } else {
        endLabel.setVisible(false);
    }
}

void mapView::displayMap()
{
    // Load image
    auto mapImage = QPixmap::fromImage(QImage(":/figs/map_chengdu.png"));

    qDebug() << "size:" << mapImage.width() << mapImage.height();

    // Construct item
    item = new QGraphicsPixmapItem(mapImage);
    item->setScale(0.46);

    // Construct scene
    mapScene = new QGraphicsScene(this);
    mapScene->addItem(item);

    // Set scene and show
    this->setScene(mapScene);
    this->show();
    this->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
}

void mapView::initStartEndPointPixmap()
{
    startPixmap = QPixmap::fromImage(QImage(":/figs/start_point.png"));
    startLabel.setParent(this);
    startLabel.setPixmap(startPixmap);
    startLabel.setVisible(false);
    startLabel.resize(startPixmap.width(), startPixmap.height());

    endPixmap = QPixmap::fromImage(QImage(":/figs/end_point.png"));
    endLabel.setParent(this);
    endLabel.setPixmap(endPixmap);
    endLabel.setVisible(false);
    endLabel.resize(startPixmap.width(), startPixmap.height());
}

void mapView::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "POSITION:" << event->x() << event->y();
//    if(status == 0){
//        return;
//    } else if(status == 1){
//        startLabel.move(event->x() - startPixmap.width() / 2, event->y() - startPixmap.height() / 2);
//    } else if(status == 2){
//        endLabel.move(event->x() - endPixmap.width() / 2, event->y() - endPixmap.height() / 2);
//    }
}

mapView::~mapView()
{
    delete ui;
    delete item;
    delete mapScene;
}
