#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dirLoaded = false;
    dataScene = new DataAnalysisScene;

    initDatabase();


    connect(this, &MainWindow::signalProgressBar, this, [=](int n){
        ui->progressBar->setValue(n);
        ui->label_1->setText(QString("%1%").arg(n));
    });

//    connect(this, &MainWindow::signalProgressBar, ui->progressBar, &QProgressBar::setValue);
//    connect(this, SIGNAL(signalProgressBar(int)), ui->label_1, SLOT(setNum(int)));

    connect(this, &MainWindow::signalProgressBar, [=](){
        qDebug() << "get signal";
    });

    connect(ui->btn_select_dir, &QPushButton::clicked, [=](){
        getFileNamesInDir();
        dirLoaded = true;
    });

    connect(ui->btn_load_data, &QPushButton::clicked, [=](){
        if(!dirLoaded){
            QMessageBox::critical(this, "Error!", "You should select directory!");
        } else if(ui->dateEdit_begin->date().day() <= ui->dateEdit_end->date().day()){
            dataScene->feeSelected = ui->cbox_fee->isChecked();
            dataScene->beginDayLoaded = ui->dateEdit_begin->date().day();
            dataScene->endDayLoaded = ui->dateEdit_end->date().day();
            dataScene->setDayRange();

            if(dirLoaded){
                auto thread = QThread::create([=](){
//                    connect(this, &MainWindow::signalProgressBar, this, &MainWindow::setProgressBar);

                    loadFilesToDB();
                    emit signalLoadFilesDone();
                });
                thread->start();
            }

        } else {
            QMessageBox::critical(this, "Error!", "The starting day should not be later than the ending day!");
        }
    });

    connect(this, &MainWindow::signalLoadFilesDone, [=](){
        QTimer::singleShot(500, this, [=](){
            this->hide();
            dataScene->show();
        });
    });


    emit signalProgressBar(0);
}

void MainWindow::setProgressBar(int n)
{
    qDebug() << n;
}


void MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
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
//    db = QSqlDatabase::addDatabase("QSQLITE", "LoadFiles");
//    db.setDatabaseName(":memory:");
//    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
//    db.open();

    QSqlQuery query(db);
    query.exec("drop table orders");

    if(dataScene->feeSelected){
        query.exec("create table orders(btime int unsigned, during int unsigned, blng float, blat float, elng float, elat float, fee float)");
    } else {
        query.exec("create table orders(btime int unsigned, during int unsigned, blng float, blat float, elng float, elat float)");
    }

    int secondsTo = 1477929600;

    int beginDay = ui->dateEdit_begin->date().day();
    int endDay = ui->dateEdit_end->date().day();

    qDebug() << beginDay << endDay;

    int i = 0, j = 0;

    for(int i = beginDay - 1; i < endDay; i++){
        for(int j = 0; j < 5; j++){

            qDebug() << i << j;

            auto path = filePathList.at(i * 5 + j);
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
                    QString command;
                    if(dataScene->feeSelected){
                        command = QString("insert into orders values(%1, %2, %3, %4, %5, %6, %7)")
                                                  .arg(split[1].toUInt()).arg(split[2].toUInt() - split[1].toUInt()).arg(split[3].toFloat())
                                                  .arg(split[4].toFloat()).arg(split[5].toFloat()).arg(split[6].toFloat()).arg(split[7].toFloat());
                    } else {
                        command = QString("insert into orders values(%1, %2, %3, %4, %5, %6)")
                                                  .arg(split[1].toUInt()).arg(split[2].toUInt() - split[1].toUInt()).arg(split[3].toFloat())
                                                  .arg(split[4].toFloat()).arg(split[5].toFloat()).arg(split[6].toFloat());
                    }

                    query.exec(command);

                    int beginGridX = (split[3].toFloat() - 103.908407474531) / 0.031363705;
                    int beginGridY = (split[4].toFloat() - 30.52408195) / 0.02697961;
                    int beginGridID = -1;
                    int endGridX = (split[5].toFloat() - 103.908407474531) / 0.031363705;
                    int endGridY = (split[6].toFloat() - 30.52408195) / 0.02697961;
                    int endGridID = -1;
                    if(beginGridX >= 0 && beginGridX < 10 && beginGridY >= 0 && beginGridY < 10){
                        beginGridID = beginGridX + beginGridY * 10;
                    }
                    if(endGridX >= 0 && endGridX < 10 && endGridY >= 0 && endGridY < 10){
                        endGridID = endGridX + endGridY * 10;
                    }

    //                if(beginGridID == -1){
    //                    qDebug() << temp_count << split[3].toFloat() << split[4].toFloat();
    //                    temp_count++;
    //                }
                    if(beginGridID != -1) {
                        dataScene->numOfBeginOrdersPerGridPerHour[beginGridID][(split[1].toInt() - secondsTo) / 1800]++;
                    }
                    if(endGridID != -1) {
                        dataScene->numOfEndOrdersPerGridPerHour[endGridID][(split[2].toInt() - secondsTo) / 1800]++;
                    }

                }
            } else {
                qDebug() << "Load file error!";
            }
            file.close();

            emit signalProgressBar( int( (i * 5.0 + j) / (endDay - beginDay + 1) / 5 * 100 ));
        }
    }

    query.exec("create index timespan on orders(btime, during)");

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
