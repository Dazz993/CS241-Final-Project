#ifndef QUERYTABLEVIEW_H
#define QUERYTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>

namespace Ui {
class queryTableView;
}

class queryTableView : public QTableView
{
    Q_OBJECT

public:
    explicit queryTableView(QWidget *parent = nullptr);

    QSqlDatabase db;
    bool feeSelected;
    QStandardItemModel *model;
    QStringList labels;

    void queryCommand(bool startPos, bool endPos, bool startTime, bool endTime, bool useFee, double slon, double slat, double elon, double elat, QDateTime dtime, QDateTime etime, double fee, QString similarity);

    ~queryTableView();

private:
    Ui::queryTableView *ui;
};

#endif // QUERYTABLEVIEW_H
