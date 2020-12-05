#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dirLoaded = false;
    dataScene = new DataAnalysisScene;

    initDatabase();


    connect(ui->btn_select_dir, &QPushButton::clicked, [=](){
        getFileNamesInDir();
        dirLoaded = true;
        selectFee = true;
    });

    connect(ui->btn_load_data, &QPushButton::clicked, [=](){
        selectFee = ui->cbox_fee->isChecked();

        if(dirLoaded){
            loadFilesToDB();
        }

        QTimer::singleShot(500, this, [=](){
            this->hide();
            dataScene->show();
        });
    });
}

void MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if(!db.open()){
        QMessageBox::critical(this, "Can't open database", "unable to establish a database connection", QMessageBox::Cancel);
    }
}

void MainWindow::getFileNamesInDir()
{
//     auto dirString = QFileDialog::getExistingDirectory(this, "Get directories", "/Users/zhuzhanda/Desktop");
     QString dirString = "/Users/zhuzhanda/Downloads/Dataset-CS241-2020";

     ui->le_dir->setText(dirString);

     QDir dir;
     dir.setPath(dirString);
     dir.setNameFilters(QStringList() << "order*");

     auto relativeFilePathList = dir.entryList();

     for(auto path : relativeFilePathList){
         filePathList << (dirString + '/' + path);
     }
}

void MainWindow::loadFilesToDB()
{
    QSqlQuery query;
    query.exec("drop table orders");
    query.exec("create table orders(btime int unsigned, etime int unsigned, blng float, blat float, elng float, elat float, fee float)");

    int secondsTo = 1477929600;

    auto path = filePathList.at(0);
//    for(auto path : filePathList){

        QFile file(path);
        QStringList lines;

//        int temp_count = 0;

        qDebug() << path;
        if(file.open(QIODevice::ReadOnly))
        {
            // QTextStream is used to get rid of '\n'
            QTextStream stream_text(&file);
            while(!stream_text.atEnd())
            {
                lines.push_back(stream_text.readLine());
            }
            for(int i = 1; i < lines.size(); i++)
            {
                QString line = lines.at(i);
                QStringList split = line.split(",");
                QString command = QString("insert into orders values(%1, %2, %3, %4, %5, %6, %7)")
                                          .arg(split[1].toUInt()).arg(split[2].toUInt()).arg(split[3].toFloat())
                                          .arg(split[4].toFloat()).arg(split[5].toFloat()).arg(split[6].toFloat()).arg(split[7].toFloat());
                query.exec(command);

                int gridX = (split[3].toFloat() - 103.908407474531) / 0.031363705;
                int gridY = (split[4].toFloat() - 30.52408195) / 0.02697961;
                int gridID = -1;
                if(gridX >= 0 && gridX < 10 && gridY >= 0 && gridY < 10){
                    gridID = gridX + gridY * 10;
                }

//                if(gridID == -1){
//                    qDebug() << temp_count << split[3].toFloat() << split[4].toFloat();
//                    temp_count++;
//                }
                dataScene->numOfOrders[(split[1].toInt() - secondsTo) / 3600]++;
                if(gridID != -1) {
                    dataScene->numOfOrdersPerGridPerHour[gridID][(split[1].toInt() - secondsTo) / 3600]++;
                }
            }
        } else {
            qDebug() << "Load file error!";
        }
        file.close();
//    }

    query.exec("select count(*) from orders");
    if(query.next()){
//        qDebug() << query.value(0) << query.value(1) << query.value(2) << query.value(3) << query.value(4) << query.value(5);
        qDebug() << query.value(0).toInt();
    } else {
        qDebug() << "Error!";
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
