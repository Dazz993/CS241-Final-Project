#include "gridselectionview.h"
#include "ui_gridselectionview.h"

#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPainter>
#include <QPen>

gridSelectionView::gridSelectionView(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::gridSelectionView)
{
    ui->setupUi(this);
    displayMap();
    displayGridAndInitLabel();
}

void gridSelectionView::displayMap()
{
    // Load image
    auto mapImage = QPixmap::fromImage(QImage(":/figs/map_chengdu.png"));

    // Construct item
    item = new QGraphicsPixmapItem(mapImage);
    item->setScale(0.47);

    // Construct scene
    mapScene = new QGraphicsScene(this);
    mapScene->addItem(item);

    // Set scene and show
    this->setScene(mapScene);
    this->show();
    this->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
}

void gridSelectionView::displayGridAndInitLabel()
{
    pixmap_choosen = QPixmap(":/figs/tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap_not_choosen = QPixmap(":/figs/not_tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    for(int i = 0; i < 100; i++){
        int gridX = i % 10;
        int gridY = i / 10;
        int x = gridX * 56.7 + 23;
        int y = gridY * 57.5 + 24;

        iconLabel[i] = new QLabel(this);
        iconLabel[i]->move(x, y);
        iconLabel[i]->setAlignment(Qt::AlignCenter);
        iconLabel[i]->setPixmap(pixmap_choosen);
        iconLabel[i]->setVisible(true);
    }

    this->viewport()->update();
}

void gridSelectionView::toggleLabelVisible(int id)
{
    auto returnPixmap = iconLabel[id]->pixmap(Qt::ReturnByValue);
    if(returnPixmap == pixmap_choosen){
        iconLabel[id]->setPixmap(pixmap_not_choosen);
    } else {
        iconLabel[id]->setPixmap(pixmap_choosen);
    }
}

void gridSelectionView::setAllLabelsChecked(bool checked)
{
    if(checked){
        for(int i = 0; i < 100; i++){
            iconLabel[i]->setPixmap(pixmap_choosen);
        }
    } else {
        for(int i = 0; i < 100; i++){
            iconLabel[i]->setPixmap(pixmap_not_choosen);
        }
    }
}

void gridSelectionView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    QPainter painter(this->viewport());
    painter.setPen(QPen(QColor(0, 103, 124), 2));
    for(int i = 0; i < 11; i++){
        painter.drawLine(18 + i * 56.7, 19, 18 + i * 56.7, 594);
        painter.drawLine(18, 19 + i * 57.5, 585, 19 + i * 57.5);
    }
}

void gridSelectionView::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    int gridX = (x - 18) / 56.7;
    int gridY = (y - 19) / 57.5;
//    qDebug() << x << y;

    if(x >= 18 && x <= 585 && y >= 19 && y <= 594){
        toggleLabelVisible(gridX + 10 * gridY);
    }
}

int gridSelectionView::getGridStatus(int id)
{
    auto returnPixmap = iconLabel[id]->pixmap(Qt::ReturnByValue);
    if(returnPixmap == pixmap_choosen){
        return 1;
    } else {
        return 0;
    }
}

gridSelectionView::~gridSelectionView()
{
    delete ui;
}
