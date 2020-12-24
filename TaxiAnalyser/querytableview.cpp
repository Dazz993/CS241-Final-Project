#include "querytableview.h"
#include "ui_querytableview.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QHeaderView>

queryTableView::queryTableView(QWidget *parent) :
    QTableView(parent),
    ui(new Ui::queryTableView)
{
    ui->setupUi(this);

    db = QSqlDatabase::database();

    model = new QStandardItemModel();
    labels << "departure time" << "end time" << "departure longitude" << "departure latitude" << "end longitude" << "end latitude" << "fee";

    this->setShowGrid(true);
    this->setModel(model);
}

void queryTableView::queryCommand(bool startPos, bool endPos, bool startTime, bool endTime, bool useFee, double slon, double slat, double elon, double elat, QDateTime dtime, QDateTime etime, double fee, QString similarity)
{
    model->clear();

    double lonDelta = 0.05;
    double latDelta = 0.05;
    double timeDelta = 300;
    double feeDelta = 1.5;

    if(similarity == "High"){
        lonDelta *= 0.7;
        latDelta *= 0.7;
        timeDelta *= 0.7;
        feeDelta *= 0.7;
    } else if(similarity == "Low"){
        lonDelta /= 0.8;
        latDelta /= 0.8;
        timeDelta /= 0.8;
        feeDelta /= 0.8;
    }

    QString _startPosCons, _endPosCons, _startTimeCons, _endTimeCons, _feeCons;
    bool first = true;
    if(startPos){
        first = false;
        _startPosCons = QString("blng > %1 AND blng < %2 AND blat > %3 AND blat < %4 ").arg(slon - lonDelta).arg(slon + lonDelta).arg(slat - latDelta).arg(slat + latDelta);
    }
    if(endPos){
        if(first){
            first = false;
            _endPosCons = QString("elng > %1 AND elng < %2 AND elat > %3 AND elat < %4 ").arg(elon - lonDelta).arg(elon + lonDelta).arg(elat - latDelta).arg(elat + latDelta);
        } else {
            _endPosCons = QString("AND elng > %1 AND elng < %2 AND elat > %3 AND elat < %4 ").arg(elon - lonDelta).arg(elon + lonDelta).arg(elat - latDelta).arg(elat + latDelta);
        }
    }
    if(startTime){
        if(first){
            first = false;
            _startTimeCons = QString("btime > %1 AND btime < %2 ").arg(QString::number(dtime.toSecsSinceEpoch() - timeDelta, 'f', 0)).arg(QString::number(dtime.toSecsSinceEpoch() + timeDelta, 'f', 0));
        } else {
            _startTimeCons = QString("AND btime > %1 AND btime < %2 ").arg(QString::number(dtime.toSecsSinceEpoch() - timeDelta, 'f', 0)).arg(QString::number(dtime.toSecsSinceEpoch() + timeDelta, 'f', 0));
        }
    }
    if(endTime){
        if(first){
            first = false;
            _startTimeCons = QString("btime + during > %1 AND btime + during < %2 ").arg(QString::number(etime.toSecsSinceEpoch() - timeDelta, 'f', 0)).arg(QString::number(etime.toSecsSinceEpoch() + timeDelta, 'f', 0));
        } else {
            _startTimeCons = QString("AND btime + during > %1 AND btime + during < %2 ").arg(QString::number(etime.toSecsSinceEpoch() - timeDelta, 'f', 0)).arg(QString::number(etime.toSecsSinceEpoch() + timeDelta, 'f', 0));
        }
    }
    if(useFee){
        if(first){
            first = false;
            _feeCons = QString("fee > %1 AND fee < %2 ").arg(fee - feeDelta).arg(fee + feeDelta);
        } else {
            _feeCons = QString("AND fee > %1 AND fee < %2 ").arg(fee - feeDelta).arg(fee + feeDelta);
        }
    }

    QString command = "select * from orders where " + _startPosCons + _endPosCons + _startTimeCons + _endTimeCons + _feeCons;
    qDebug() << command;

    QSqlQuery query(db);
    auto isOk = query.exec(command);

    if(!isOk){
        qDebug() << "ERROR! " << query.lastError().text();
    }

    int count = 0;
    QStandardItem* item = NULL;
    while(query.next()){
        qDebug() << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4) << query.value(5) << query.value(6);

        qDebug() << QDateTime::fromSecsSinceEpoch(query.value(0).toInt());

        item = new QStandardItem(QDateTime::fromSecsSinceEpoch(query.value(0).toInt()).toString("MM-dd  hh:mm:ss"));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 0, item);

        item = new QStandardItem(QDateTime::fromSecsSinceEpoch(query.value(0).toInt() + query.value(1).toInt()).toString("MM-dd  hh:mm:ss"));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 1, item);

        item = new QStandardItem(QString("%1°E").arg(QString::number(query.value(2).toFloat(), 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 2, item);

        item = new QStandardItem(QString("%1°N").arg(QString::number(query.value(3).toFloat(), 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 3, item);

        item = new QStandardItem(QString("%1°E").arg(QString::number(query.value(4).toFloat(), 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 4, item);

        item = new QStandardItem(QString("%1°N").arg(QString::number(query.value(5).toFloat(), 'f', 3)));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 5, item);

        item = new QStandardItem(QString::number(query.value(6).toFloat(), 'f', 2));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        model->setItem(count, 6, item);
        count++;
    }

    model->setHorizontalHeaderLabels(labels);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << count;
}

queryTableView::~queryTableView()
{
    delete ui;
}
