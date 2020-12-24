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
    ImgWidth = mapImage.width();
    ImgHeight = mapImage.height();
    _countTable = new int[ImgWidth * ImgHeight]();

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
    qDebug() << "STATUS" << status << "POSITION:" << event->x() << event->y();

    int x = event->x();
    int y = event->y();
    double lat = (y - 12.0) * (30.524 - 30.794) / (588.0 - 23.0) + 30.794;
    double lng = (x - 24) * (104.222 - 103.909) / (576 - 24) + 103.909;

    if(status == 0){
        return;
    } else if(status == 1){
        startLabel.move(event->x() - startPixmap.width() / 2, event->y() - endPixmap.height());
        emit startPosChanged(lat, lng);
    } else if(status == 2){
        endLabel.move(event->x() - endPixmap.width() / 2, event->y() - endPixmap.height());
        emit endPosChanged(lat, lng);
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

void mapView::addPoints(int x, int y, int count)
{
    Point pt{x, y, radius, count};
    _countTable[pt.posX + pt.posY * ImgWidth] = pt.count;
    _posList.push_back(pt);

    if(count > _maxCount){
        _maxCount = count;
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
        //以最大次数来计算该点的权重
        const uchar alpha=uchar(_countTable[pt.posX+pt.posY*ImgWidth]/max_count*255);
        QRadialGradient gradient(pt.posX,pt.posY,pt.radius);
        gradient.setColorAt(0,QColor(0,0,0,alpha));
        gradient.setColorAt(1,QColor(0,0,0,0));
        painter.setBrush(gradient);
        painter.drawEllipse(QPointF(pt.posX,pt.posY),pt.radius,pt.radius);
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
    _maxCount=1;
    memset(_countTable,0,ImgWidth*ImgHeight*sizeof(int));
    update();
}

void mapView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    if(heatMapActivate){
        qDebug() << "it's pressing!";
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
