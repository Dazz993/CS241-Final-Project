#include "mapview.h"
#include "ui_mapview.h"
#include <QDebug>
#include <QPainter>
#include <QSqlError>

mapView::mapView(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::mapView)
{
    ui->setupUi(this);

    status = 0;
    heatMapActivate = false;

    displayMap();
    initStartEndPointPixmap();
    initHeatMap();
}

void mapView::setPoints(bool start, bool end, double slon, double slat,  double elon, double elat){
    double sx = (576 - 24) / (104.222 - 103.909) * (slon - 103.909) + 24;
    double sy = (588.0 - 23.0) / (30.524 - 30.794) * (slat - 30.794) + 23 - 11;
    double ex = (576 - 24) / (104.222 - 103.909) * (elon - 103.909) + 24;
    double ey = (588.0 - 23.0) / (30.524 - 30.794) * (elat - 30.794) + 23 - 11;

    if(start){
        startLabel.move(sx - startPixmap.width() / 2, sy - startPixmap.height());
        startLabel.setVisible(true);
    } else {
        startLabel.setVisible(false);
    }

    if(end){
        endLabel.move(ex - endPixmap.width() / 2, ey - endPixmap.height());
        endLabel.setVisible(true);
    } else {
        endLabel.setVisible(false);
    }
}

void mapView::displayMap()
{
    // Load image
    auto mapImage = QPixmap::fromImage(QImage(":/figs/map_chengdu.png"));
    ImgWidth = mapImage.width();
    ImgHeight = mapImage.height();
    _countTable = new int[ImgWidth * ImgHeight]();
    _flagTable = new bool[ImgWidth * ImgHeight]();

//    qDebug() << "size:" << mapImage.width() << mapImage.height();

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

void mapView::initHeatMap()
{
    _dataImg=QImage(ImgWidth,ImgHeight,QImage::Format_Alpha8);
    _dataImg.fill(Qt::transparent);

    _heatImg=QImage(ImgWidth,ImgHeight,QImage::Format_ARGB32);
    _heatImg.fill(Qt::transparent);

    QLinearGradient linear=QLinearGradient(QPoint(0,0),QPoint(255,0));
    linear.setColorAt(0, Qt::blue);
    linear.setColorAt(0.4, Qt::blue);
    linear.setColorAt(0.5, Qt::cyan);
    linear.setColorAt(0.6, Qt::green);
    linear.setColorAt(0.8, Qt::yellow);
    linear.setColorAt(0.95, Qt::red);

    QImage img(256,1,QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.fillRect(img.rect(),linear);

    quint32 alpha=0;
    for(quint32 i=0;i<256;i++){
        alpha=HeatAlpha/255.0*i;
        _colorList[i] = (img.pixel(i,0) & 0x00FFFFFF) | (alpha<<24);
    }
}

void mapView::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "STATUS" << status << "POSITION:" << event->x() << event->y();

    int x = event->x();
    int y = event->y();
    double lat = (y - 12.0) * (30.524 - 30.794) / (588.0 - 23.0) + 30.794;
    double lng = (x - 24) * (104.222 - 103.909) / (576 - 24) + 103.909;

    if(status == 0){
        return;
    } else if(status == 1){
        emit startPosChanged(lat, lng);
        startLabel.move(event->x() - startPixmap.width() / 2, event->y() - startPixmap.height());
    } else if(status == 2){
        emit endPosChanged(lat, lng);
        endLabel.move(event->x() - endPixmap.width() / 2, event->y() - startPixmap.height());
    }
}

void mapView::setStatus(int _status)
{
    status = _status;
}

void mapView::setHeatMapFlag(bool activate)
{
    heatMapActivate = activate;
}

void mapView::displayHeatMap()
{
    drawDataImg();
    drawHeatImg();
    this->viewport()->update();
}

void mapView::addData(double lat, double lng, bool use_depature_time,  QTime depature_time)
{
    clear();

    double lonDelta = 0.02;
    double latDelta = 0.04;
    double timeDelta = 500;

    QString _startPosCons, _startTimeCons;

    _startPosCons = QString("blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 ").arg(lng - lonDelta).arg(lng + lonDelta).arg(lat - latDelta).arg(lat + latDelta);

    const int BASETIME = QDateTime(QDate(2016, 11, 1), QTime(0, 0)).toSecsSinceEpoch();
    const int BASE_BEGIN = QDateTime(QDate(2016, 11, 1), depature_time).toSecsSinceEpoch() - BASETIME;

    QString subitem1 = QString("((btime - %1) % 86400 > %2)").arg(BASETIME).arg(BASE_BEGIN - timeDelta);
    QString subitem2 = QString("((btime - %1) % 86400 < %2)").arg(BASETIME).arg(BASE_BEGIN + timeDelta);

    if(use_depature_time){
        _startTimeCons = QString("AND %1 AND %2 ").arg(subitem1).arg(subitem2);
    }

    QString command = "select elng, elat from orders where " + _startPosCons + _startTimeCons;
//    qDebug() << command;

    QSqlQuery query(db);
    auto isOk = query.exec(command);

    if(!isOk){
        qDebug() << "ERROR! " << query.lastError().text();
    }

    while(query.next()){
//        qDebug() << "POINT" << query.value(0).toFloat() << query.value(1).toFloat();
        double slon = query.value(0).toFloat(), slat = query.value(1).toFloat();
        double sx = (576 - 24) / (104.222 - 103.909) * (slon - 103.909);
        double sy = (588.0 - 23.0) / (30.524 - 30.794) * (slat - 30.794);
        if(sx >= 0 && sx < ImgWidth && sy >= 0 && sy < ImgHeight){
            addPoints(sx, sy, 1);
        }
    }
}


void mapView::addPoints(int x, int y, int count)
{
    Point pt{x, y, radius, count};
    _countTable[pt.posX + pt.posY * ImgWidth] += pt.count;
    _posList.push_back(pt);

    if(_countTable[pt.posX + pt.posY * ImgWidth] > _maxCount){
        _maxCount = _countTable[pt.posX + pt.posY * ImgWidth];
        _maxPosX = pt.posX;
        _maxPosY = pt.posY;
    }
}

void mapView::drawDataImg()
{
    //重新绘制先清空
    _dataImg.fill(Qt::transparent);
    QPainter painter(&_dataImg);
    painter.setPen(Qt::transparent);
    //绘制点的部分可以调用drawpoint
    const double max_count=_maxCount;
    for(int i=0;i<_posList.count();i++)
    {
        const Point &pt=_posList.at(i);
        const int id = pt.posX + pt.posY * ImgWidth;
        //以最大次数来计算该点的权重
        if(!_flagTable[id]){
            const uchar alpha=uchar(_countTable[id]/max_count*255);
            QRadialGradient gradient(pt.posX,pt.posY,pt.radius);
            gradient.setColorAt(0,QColor(0,0,0,alpha));
            gradient.setColorAt(1,QColor(0,0,0,0));
            painter.setBrush(gradient);
            painter.drawEllipse(QPointF(pt.posX,pt.posY),pt.radius,pt.radius);
            _flagTable[id] = true;

//            qDebug() << "Calculating:" << _countTable[id];
            Point tempPoint{pt.posX, pt.posY, pt.radius, _countTable[id]};
            _resList.push_back(tempPoint);
            _totalCount += _countTable[id];
        }
    }
}

void mapView::drawHeatImg()
{
    for(int row=0;row<_dataImg.height();row++)
    {
        //dataimg QImage::Format_Alpha8，一个点1个字节
        const uchar *line_data=_dataImg.scanLine(row);
        //heatimg QImage::Format_ARGB32，一个点4个字节
        QRgb *line_heat=reinterpret_cast<QRgb*>(_heatImg.scanLine(row));
        for(int col=0;col<_dataImg.width();col++)
        {
            //根据alpha透明度从颜色表取颜色
            line_heat[col]=_colorList[line_data[col]];
        }
    }
}

void mapView::clear()
{
    _dataImg.fill(Qt::transparent);
    _heatImg.fill(Qt::transparent);
    _posList.clear();
    _resList.clear();
    _maxCount=1;
    _maxPosX = 0;
    _maxPosY = 0;
    _totalCount = 0;
    memset(_countTable, 0, ImgWidth * ImgHeight*sizeof(int));
    memset(_flagTable, 0, ImgWidth * ImgHeight*sizeof(bool));

    update();
}

void mapView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    if(heatMapActivate){
//        qDebug() << "it's pressing!";
        QPainter p(this->viewport());
        p.drawImage(24, 24, _heatImg);
    }
}

mapView::~mapView()
{
    delete ui;
    delete item;
    delete mapScene;
}
