#ifndef GRIDSELECTIONVIEW_H
#define GRIDSELECTIONVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>

namespace Ui {
class gridSelectionView;
}

class gridSelectionView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit gridSelectionView(QWidget *parent = nullptr);
    ~gridSelectionView();

    QPixmap pixmap_choosen;
    QPixmap pixmap_not_choosen;

    QGraphicsPixmapItem * item;
    QGraphicsScene * mapScene;

    QLabel * iconLabel[100];

    void displayMap();
    void displayGridAndInitLabel();
    void paintEvent(QPaintEvent *event);
    void toggleLabelVisible(int id);
    void setAllLabelsChecked(bool checked);
    int getGridStatus(int id);
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::gridSelectionView *ui;
};

#endif // GRIDSELECTIONVIEW_H
