#include "dataanalysisscene.h"
#include "ui_dataanalysisscene.h"

DataAnalysisScene::DataAnalysisScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataAnalysisScene)
{
    ui->setupUi(this);
}

DataAnalysisScene::~DataAnalysisScene()
{
    delete ui;
}
