#ifndef DATAANALYSISSCENE_H
#define DATAANALYSISSCENE_H

#include <QMainWindow>

namespace Ui {
class DataAnalysisScene;
}

class DataAnalysisScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataAnalysisScene(QWidget *parent = nullptr);
    ~DataAnalysisScene();

private:
    Ui::DataAnalysisScene *ui;
};

#endif // DATAANALYSISSCENE_H
