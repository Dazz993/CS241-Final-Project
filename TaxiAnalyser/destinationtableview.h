#ifndef DESTINATIONTABLEVIEW_H
#define DESTINATIONTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>
#include "mapview.h"

class destinationTableView : public QTableView
{
    Q_OBJECT

public:
    explicit destinationTableView(QWidget *parent = nullptr);

    QStandardItemModel *model;
    QStringList labels;

    void showMaxPosPlaces(QList<mapView::Point> _resList, int count);
signals:

};

#endif // DESTINATIONTABLEVIEW_H
