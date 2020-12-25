#include "destinationtableview.h"
#include <QHeaderView>
#include <QDebug>
destinationTableView::destinationTableView(QWidget *parent) : QTableView(parent)
{
    model = new QStandardItemModel();
    labels << "Range of latitude" << "Range of longitude" << "probability priority(the smaller, the better)";

    this->setShowGrid(true);
    this->setModel(model);
}

void destinationTableView::showMaxPosPlaces(QList<mapView::Point> _resList, int count)
{
    model->clear();

    std::sort(_resList.begin(), _resList.end(), [](mapView::Point a, mapView::Point b){
        return b.count < a.count;
    });

    QStandardItem* item = NULL;

    for(int i = 0; i < 20; i++){
        const mapView::Point &p = _resList.at(i);
        const double range = 0.01;
        double lat = (p.posY - 12.0) * (30.524 - 30.794) / (588.0 - 23.0) + 30.794;
        double lng = (p.posX - 24) * (104.222 - 103.909) / (576 - 24) + 103.909;

//        qDebug() << p.count << count << p.count / count;

        item = new QStandardItem(QString("%1~%2°N").arg(QString::number(lat - range, 'f', 3)).arg(QString::number(lat + range, 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(i, 0, item);

        item = new QStandardItem(QString("%1~%2°E").arg(QString::number(lng - range, 'f', 3)).arg(QString::number(lng + range, 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(i, 1, item);

        item = new QStandardItem(QString::number(i + 1));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(i, 2, item);
    }

    model->setHorizontalHeaderLabels(labels);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
