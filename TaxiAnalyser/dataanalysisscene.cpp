#include "dataanalysisscene.h"
#include "ui_dataanalysisscene.h"
#include <QSizePolicy>
#include <QSqlError>

DataAnalysisScene::DataAnalysisScene(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataAnalysisScene)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    initValues();
    initMapLabels();

    // Scene 1 - tag 1
    // display chart
    displayInSplineChart();

    connect(ui->btn_analyse, &QPushButton::clicked, [=](){
        if(ui->dateEdit_tag1_begin->date().day() <= ui->dateEdit_tag1_end->date().day()){
            displayInSplineChart();
        } else {
            QMessageBox::critical(this, "Error!", "The starting day should not be later than the ending day!");
        }
    });

    // Scene 1 - tag 2
    displayInBarChart();

    connect(ui->btn_tag2_analyse, &QPushButton::clicked, [=](){
        if(ui->timeEdit_tag2_begin_time->time() <= ui->timeEdit_tag2_end_time->time()){
            displayInBarChart();
        } else {
            QMessageBox::critical(this, "Error!", "The starting time should not be later than the ending time!");
        }

    });


    connect(ui->pushButton_check_all, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_not_choosen){
                iconLabel[i]->setPixmap(pixmap_choosen);
            }
        }
    });

    connect(ui->pushButton_tag2_check_all, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_not_choosen){
                iconLabel[i]->setPixmap(pixmap_choosen);
            }
        }
    });

    connect(ui->pushButton_check_none, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_choosen){
                iconLabel[i]->setPixmap(pixmap_not_choosen);
            }
        }
    });

    connect(ui->pushButton_tag2_check_none, &QPushButton::clicked, [=](){
        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_choosen){
                iconLabel[i]->setPixmap(pixmap_not_choosen);
            }
        }
    });


    // Scene 2 - tab 1
    connect(ui->btn_scene2_analyse, &QPushButton::clicked, [=](){
        analyseTravelTime();
    });

    // Scene 2 - tab 2
    connect(ui->btn_scene2_analyse_2, &QPushButton::clicked, [=](){
        analyseFee();
        analyseRevenue();
    });


    // Switch button

    connect(ui->toolButton_0, &QToolButton::clicked, [=](){
        ui->stackedWidget->setCurrentWidget(ui->page_1);
        enableMapLabels();
        qDebug() << ui->stackedWidget->currentIndex();


        for(int i = 0; i < 100; i++){
            auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
            if(pixmapReturn == pixmap_choosen){
                qDebug() << "right";
            } else {
                qDebug() << "error";
            }
        }
    });

    connect(ui->toolButton_1, &QToolButton::clicked, [=](){
        disableMapLabels();
        ui->stackedWidget->setCurrentWidget(ui->page_2);
        qDebug() << ui->stackedWidget->currentIndex();

        db = QSqlDatabase::database();
        qDebug() << db;

    });
}

void DataAnalysisScene::initValues()
{
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 24 * 15; j++){
            numOfBeginOrdersPerGridPerHour[i][j] = 0;
            numOfEndOrdersPerGridPerHour[i][j] = 0;
        }
    }

    ui->timeEdit_tag2_begin_time->setTime(QTime(0, 0));
    ui->timeEdit_tag2_end_time->setTime(QTime(23, 59));
}

void DataAnalysisScene::initMapLabels()
{
    ui->label_tag1_map->setPixmap(QPixmap(":/figs/final_map2.png").scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label_tag2_map->setPixmap(QPixmap(":/figs/final_map2.png").scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    pixmap_choosen = QPixmap(":/figs/tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap_not_choosen = QPixmap(":/figs/not_tick.png").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    for(int i = 0; i < 100; i++){
        int gridX = i % 10;
        int gridY = i / 10;
        int x = gridX * 54 + 231 - 40;
        int y = 712 - gridY * 54 - 54 - 5;

        if(gridX >= 6){
            x += gridX / 2 - 3;
        }
        if(gridY <= 2){
            y += 2 - gridY / 2;
        }

        iconLabel[i] = new QLabel(this);
        iconLabel[i]->move(x, y);
        iconLabel[i]->setAlignment(Qt::AlignCenter);
        iconLabel[i]->setPixmap(pixmap_choosen);
        iconLabel[i]->setVisible(true);
    }
}

void DataAnalysisScene::enableMapLabels(){
    for(int i = 0; i < 100; i++){
//        delete iconLabel[i];
        iconLabel[i]->setVisible(true);
    }
}

void DataAnalysisScene::disableMapLabels(){
    for(int i = 0; i < 100; i++){
//        delete iconLabel[i];
        iconLabel[i]->setVisible(false);
    }
}

void DataAnalysisScene::analyseTravelTime(){
    QSqlQuery query(db);

    // travelTime BarChart
    if(!travelTimeChart){
        travelTimeChart = new QChart;
        travelTimeSeries = new QBarSeries;
        travelTimeChart->addSeries(travelTimeSeries);
    } else {
        delete travelTimeAxisX;
        delete travelTimeAxisY;
        travelTimeSeries->clear();
    }

    auto barSet = new QBarSet("The number of order in a period of time");
    barSet->setColor(QColor(99, 192, 135));
//    barSet->setColor(QColor(61, 138, 254));
    travelTimeSeries->append(barSet);

    QStringList cat;

    int date = ui->dateEdit_scene2_date->date().day();
    int beginUTime = timeToUnixTime(date, ui->timeEdit_scene2_begin_time->time().hour(), ui->timeEdit_scene2_begin_time->time().minute());
    int endUTime = timeToUnixTime(date, ui->timeEdit_scene2_end_time->time().hour(), ui->timeEdit_scene2_end_time->time().minute());

    if(beginUTime >= endUTime) return;

    int timeStep = ui->spinBox_scene2_timestep->value();

    int maxValue = 0;
    int count = 0;

    const int NUM_DATA = 600;
    int data[NUM_DATA];
    for(int i = 0; i < NUM_DATA; i++){
        data[i] = 0;
    }

    qDebug() << beginUTime << endUTime << timeStep;

    auto command = QString("SELECT during / 60 / %3, count(during / 60 / %3) \n"
                        "FROM orders \n"
                        "WHERE btime > %1 AND btime < %2 \n"
                        "GROUP BY during / 60 / %3 \n"
                        "ORDER BY during / 60 / %3 \n").arg(beginUTime).arg(endUTime).arg(timeStep);

    auto isOK = query.exec(command);

    qDebug() << "isOK?" << isOK << query.lastError().text();

    while(query.next()){
        qDebug() << query.value(0).toInt() << query.value(1).toInt();
        data[query.value(0).toInt()] = query.value(1).toInt();
        count += query.value(1).toInt();
    }

    std::vector<int> t(data, data + NUM_DATA);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    for(int i = 0; i < 60 / timeStep; i++){
        cat << QString("%1~%2 ").arg(i * timeStep).arg((i+1) * timeStep);
        *barSet << data[i] * 100.0 / count;
        qDebug() << data[i] * 100.0 / count;
    }

    maxValue = t[0] * 100 / count;

    travelTimeAxisX = new QBarCategoryAxis;
    travelTimeAxisX->append(cat);
    travelTimeAxisX->setLabelsFont(QFont("Helvetica", 12));
    travelTimeAxisX->setLabelsAngle(90);
    travelTimeAxisX->setGridLineVisible(false);
    travelTimeAxisX->setTitleText("Travel time period / minutes");
    travelTimeAxisX->setTitleFont(QFont("Helvetica", 12));
    travelTimeChart->addAxis(travelTimeAxisX, Qt::AlignBottom);
    travelTimeSeries->attachAxis(travelTimeAxisX);

    travelTimeAxisY = new QValueAxis;
    travelTimeAxisY->setLabelsFont(QFont("Helvetica", 13));
    travelTimeAxisY->setLabelFormat("%d");
    travelTimeAxisY->setTitleText("Percentage of time period / \%");
    travelTimeAxisY->setTitleFont(QFont("Helvetica", 12));
    int rangeMax = 0;
    if(maxValue < 20){
        rangeMax = 20;
    } else if(maxValue < 40){
        rangeMax = 40;
    } else if(maxValue < 60){
        rangeMax = 60;
    }
    travelTimeAxisY->setRange(0, rangeMax);
    travelTimeAxisY->setTickCount(6);
    travelTimeChart->addAxis(travelTimeAxisY, Qt::AlignLeft);
    travelTimeSeries->attachAxis(travelTimeAxisY);

//    travelTimeChart->legend()->setVisible(true);
//    travelTimeChart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView_3->setChart(travelTimeChart);


    // travelTime PieChart

    QPieSeries *series = new QPieSeries();

    int first_n_count = 0;
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < NUM_DATA; j++) {
            if(t[i] == data[j]){
                series->append(QString("%1~%2 min: %3%").arg(j * timeStep).arg((j+1) * timeStep).arg(QString::number(t[i] * 100.0 / count, 'f', 2)), t[i] * 100.0 / count);
                first_n_count += t[i];
                break;
            }
        }
//        QPieSlice *slice = series->slices().at(i);
//        slice->setExploded();
//        slice->setLabelVisible();
    }
    series->append("Other", (count - first_n_count) * 100.0 / count);

    QPieSlice *slice = series->slices().at(0);
    slice->setExploded();
    slice->setLabelVisible();

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Pie Chart");
    chart->setTitleFont(QFont("Tahoma", 14));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setContentsMargins(0,0,0,0);
    chart->legend()->setWindowFrameMargins(0,0,0,0);
//    chart->legend()->hide();

    ui->graphicsView_4->setChart(chart);
    ui->graphicsView_4->setRenderHint(QPainter::Antialiasing);
}

void DataAnalysisScene::analyseFee()
{
    auto chart = new QChart;
    auto barSeries = new QBarSeries;
    chart->addSeries(barSeries);

    auto barSet = new QBarSet("The number of order in a period of time");
    barSet->setColor(QColor(99, 192, 135));
//    barSet->setColor(QColor(61, 138, 254));
    barSeries->append(barSet);

    QStringList cat;

    int date = ui->dateEdit_scene2_date_2->date().day();
    int beginUTime = timeToUnixTime(date, ui->timeEdit_scene2_begin_time_2->time().hour(), ui->timeEdit_scene2_begin_time_2->time().minute());
    int endUTime = timeToUnixTime(date, ui->timeEdit_scene2_end_time_2->time().hour(), ui->timeEdit_scene2_end_time_2->time().minute());

    if(beginUTime >= endUTime) return;

    int nParts = 25;

    int maxValue = 0;
    int count = 0;

    const int NUM_DATA = 100;
    int data[NUM_DATA];
    for(int i = 0; i < NUM_DATA; i++){
        data[i] = 0;
    }

    qDebug() << beginUTime << endUTime << nParts;

    QSqlQuery query(db);

    auto command = QString("SELECT round(fee), count(round(fee)) \n"
                        "FROM orders \n"
                        "WHERE btime > %1 AND btime < %2 \n"
                        "GROUP BY round(fee) \n"
                        "ORDER BY round(fee) \n").arg(beginUTime).arg(endUTime);

    auto isOK = query.exec(command);

    qDebug() << "isOK?" << isOK << query.lastError().text();

    while(query.next()){
        qDebug() << query.value(0).toInt() << query.value(1).toInt();
        data[query.value(0).toInt()] = query.value(1).toInt();
        count += query.value(1).toInt();
    }

    std::vector<int> t(data, data + NUM_DATA);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    for(int i = 0; i < nParts; i++){
        if(i == 0){
            cat << QString("0~0.5");
        } else if(i == nParts - 1){
            cat << QString(">=%1").arg(i+0.5);
        } else {
            cat << QString("%1~%2").arg(i-0.5).arg(i+0.5);
        }
        *barSet << data[i] * 100.0 / count;
        qDebug() << data[i] * 100.0 / count;
    }

    maxValue = t[0] * 100 / count;

    auto AxisX = new QBarCategoryAxis;
    AxisX->append(cat);
    AxisX->setLabelsFont(QFont("Helvetica", 12));
    AxisX->setLabelsAngle(90);
    AxisX->setGridLineVisible(false);
    AxisX->setTitleText("Travel time period / minutes");
    AxisX->setTitleFont(QFont("Helvetica", 12));
    chart->addAxis(AxisX, Qt::AlignBottom);
    barSeries->attachAxis(AxisX);

    auto AxisY = new QValueAxis;
    AxisY->setLabelsFont(QFont("Helvetica", 13));
    AxisY->setLabelFormat("%d");
    AxisY->setTitleText("Percentage of time period / \%");
    AxisY->setTitleFont(QFont("Helvetica", 12));
    int rangeMax = 0;
    if(maxValue < 20){
        rangeMax = 20;
    } else if(maxValue < 40){
        rangeMax = 40;
    } else if(maxValue < 60){
        rangeMax = 60;
    }
    AxisY->setRange(0, rangeMax);
    AxisY->setTickCount(6);
    chart->addAxis(AxisY, Qt::AlignLeft);
    barSeries->attachAxis(AxisY);

//    travelTimeChart->legend()->setVisible(true);
//    travelTimeChart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView_6->setChart(chart);
}

void DataAnalysisScene::analyseRevenue()
{
    auto chart = new QChart;
    auto barSeries = new QBarSeries;
    chart->addSeries(barSeries);

    auto barSet = new QBarSet("The number of order in a period of time");
//    barSet->setColor(QColor(99, 192, 135));
    barSet->setColor(QColor(61, 138, 254));
    barSeries->append(barSet);

    QStringList cat;

    int date = ui->dateEdit_scene2_date_2->date().day();
    int beginUTime = timeToUnixTime(date, ui->timeEdit_scene2_begin_time_2->time().hour(), ui->timeEdit_scene2_begin_time_2->time().minute());
    int endUTime = timeToUnixTime(date, ui->timeEdit_scene2_end_time_2->time().hour(), ui->timeEdit_scene2_end_time_2->time().minute());

    if(beginUTime >= endUTime) return;

    int stepTime = ui->spinBox_scene2_n_parts->value() * 60;

    int maxValue = 0;
    int count = 0;

    const int NUM_DATA = 100;
    int data[NUM_DATA];
    for(int i = 0; i < NUM_DATA; i++){
        data[i] = 0;
    }

    qDebug() << beginUTime << endUTime << stepTime;

    QSqlQuery query(db);

    QString subItem = QString("(btime - %1) / %2").arg(beginUTime).arg(stepTime);

    auto command = QString("SELECT %1, count(fee) \n"
                            "FROM orders \n"
                            "WHERE btime > %2 AND btime < %3 \n"
                            "GROUP BY %1 \n"
                            "ORDER BY %1 \n").arg(subItem).arg(beginUTime).arg(endUTime);

    auto isOK = query.exec(command);

    qDebug() << query.lastQuery();

    qDebug() << "isOK?" << isOK << query.lastError().text();

    while(query.next()){
        qDebug() << "OUTPUT" << query.value(0).toFloat() << query.value(1).toFloat();
        data[query.value(0).toInt()] = query.value(1).toInt();
        count += query.value(1).toInt();
    }

    std::vector<int> t(data, data + NUM_DATA);
    std::sort(t.begin(), t.end(), [](int a, int b){
        return b < a;
    });

    for(int i = 0; i < (endUTime - beginUTime) / stepTime + 1; i++){
        QTime beginTime = unixTimeToTime(beginUTime + i * stepTime).time();
        QTime endTime = unixTimeToTime(std::min(endUTime, beginUTime + (i+1) * stepTime)).time();

        if(beginTime == endTime) break;

        cat << QString("%1:%2~%3:%4").arg(beginTime.hour(), 2, 10, QLatin1Char('0')).arg(beginTime.minute(), 2, 10, QLatin1Char('0')).arg(endTime.hour(), 2, 10, QLatin1Char('0')).arg(endTime.minute(), 2, 10, QLatin1Char('0'));
        *barSet << data[i];
        qDebug() << data[i];
    }

    maxValue = t[0];

    auto AxisX = new QBarCategoryAxis;
    AxisX->append(cat);
    AxisX->setLabelsFont(QFont("Helvetica", 12));
    AxisX->setLabelsAngle(90);
    AxisX->setGridLineVisible(false);
    AxisX->setTitleText("Travel time period / minutes");
    AxisX->setTitleFont(QFont("Helvetica", 12));
    chart->addAxis(AxisX, Qt::AlignBottom);
    barSeries->attachAxis(AxisX);

    auto AxisY = new QValueAxis;
    AxisY->setLabelsFont(QFont("Helvetica", 13));
    AxisY->setLabelFormat("%d");
    AxisY->setTitleText("Percentage of time period / \%");
    AxisY->setTitleFont(QFont("Helvetica", 12));
    int rangeMax = 0;
    if(maxValue < 5000){
        rangeMax = 5000;
    } else if(maxValue < 10000){
        rangeMax = 10000;
    } else if(maxValue < 20000){
        rangeMax = 20000;
    }
    AxisY->setRange(0, rangeMax);
    AxisY->setTickCount(6);
    chart->addAxis(AxisY, Qt::AlignLeft);
    barSeries->attachAxis(AxisY);

//    travelTimeChart->legend()->setVisible(true);
//    travelTimeChart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView_5->setChart(chart);
}


bool DataAnalysisScene::event(QEvent *e)
{
    //如果是鼠标按下 ，在event事件分发中做拦截操作
    if(e->type() == QEvent::MouseButtonPress && ui->stackedWidget->currentIndex() == 0)
    {
        QMouseEvent * ev  = static_cast<QMouseEvent *>(e);

        if(ev->x() >= 232 && ev->x() <= 770 && ev->y() <= 712 && ev->y() >= 173){
            int gridX = (ev->x() - 231) / 54;
            int gridY = (712 - ev->y()) / 54;
            int gridID = gridX+ gridY * 10;

            QString str = QString( "Event函数中：：鼠标按下了 x = %1   y = %2  globalX = %3 globalY = %4 " ).arg(ev->x()).arg(ev->y()).arg(ev->globalX()).arg(ev->globalY());
            qDebug() << str << "gridID:" << gridID;

            auto returnPixmap = iconLabel[gridID]->pixmap(Qt::ReturnByValue);
            if(returnPixmap == pixmap_choosen){
                iconLabel[gridID]->setPixmap(pixmap_not_choosen);
            } else {
                iconLabel[gridID]->setPixmap(pixmap_choosen);
            }


            return true; //true代表用户自己处理这个事件，不向下分发
        }
    }

    //其他事件 交给父类处理  默认处理
    return QMainWindow::event(e);
}

void DataAnalysisScene::displayInSplineChart()
{
    // New and delete object for new display
    if(!splineChart){
        splineChart = new QChart;
    } else {
        delete splineAxisX;
        delete splineAxisY;
        splineChart->removeAllSeries();
    }

    auto series = new QSplineSeries;

    // Data processing and insert them into series
    // Grid filter
    int choosen[100] = {-1};
    for(int i = 0; i < 100; i++){
        auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
        if(pixmapReturn == pixmap_choosen){
            choosen[i] = 1;
        } else {
            choosen[i] = 0;
        }
    }

    int beginDay = ui->dateEdit_tag1_begin->date().day();
    int endDay = ui->dateEdit_tag1_end->date().day() + 1;
    int timeStep = ui->spinBox_tag1_timestep->value() * 3600;

    int beginUTime = timeToUnixTime(beginDay);
    int endUTime = timeToUnixTime(endDay);

    int step_begin = beginUTime;
    int step_end = (beginUTime + timeStep) < endUTime ? (beginUTime + timeStep) : endUTime;
    int step_middle = step_begin + (step_end - step_begin) / 2;

    int maxValue = 0;

    while(step_begin < endUTime){
        int sumInPeriod = 0;
        int startHalfH = (step_begin - BASETIME) / 1800;
        int endHalfH = (step_end - BASETIME) / 1800;
        step_middle = step_begin + (step_end - step_begin) / 2;


//        qDebug() << ui->comboBox_tag1_type->currentText();

        for(int i = 0; i < 100; i++){
            if(choosen[i]){
                for(int j = startHalfH; j < endHalfH; j++){
                    auto type = ui->comboBox_tag1_type->currentText();
                    if(type == "Total orders in the grids"){
                        sumInPeriod += numOfBeginOrdersPerGridPerHour[i][j] + numOfEndOrdersPerGridPerHour[i][j];
                    } else if(type == "Orders of departure in the grids"){
                        sumInPeriod += numOfEndOrdersPerGridPerHour[i][j];
                    } else if(type == "Orders of end in the grids"){
                        sumInPeriod += numOfBeginOrdersPerGridPerHour[i][j];
                    }
                }
            }
        }

        if(sumInPeriod > maxValue){
            maxValue = sumInPeriod;
        }

        series->append(unixTimeToTime(step_middle).toMSecsSinceEpoch(), sumInPeriod);

//        total += sumInPeriod;
//        qDebug() << startH << endH << step_begin << step_middle << step_end << unixTimeToTime(step_middle) << sumInPeriod;

        step_begin = step_end;
        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
    }

//    qDebug() << total;

//    while(true){
//        QString command = QString("select count(*) from orders where btime >= %1 and etime < %2").arg(step_begin).arg(step_end);
//        query.exec(command);
//        if(query.next()){
//            auto _begin_datetime = unixTimeToTime(step_begin);
//            auto _end_datetime = unixTimeToTime(step_end);

//            qDebug() << _begin_datetime << unixTimeToTime(step_end) << query.value(0).toInt();

//            series->append(_begin_datetime.toMSecsSinceEpoch(), query.value(0).toInt());
//        } else {
//            qDebug() << "Error!";
//        }

//        if(step_end == endUTime) break;

//        step_begin = step_end;
//        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
//    }

    // Modify settings for series
    series->setPen(QPen(QColor(99, 192, 135), 4 ,Qt::SolidLine));
    series->setPointLabelsFormat("@yPoint");
    series->setPointsVisible();
    series->setPointLabelsVisible();
    series->setPointLabelsClipping(false);

    // Modify settings for chart
    splineChart->setTitle("The number of orders in fine-grid over time");
//    chart->setAnimationOptions(QChart::SeriesAnimations);
    splineChart->legend()->hide();
    splineChart->addSeries(series);
//    splineChart->setAxisX(splineAxisX, series);
//    splineChart->setAxisY(splineAxisY, series);

    // Create axisX
    splineAxisX = new QDateTimeAxis;
    splineAxisX->setRange(QDateTime(QDate(2016, 11, beginDay), QTime(0, 0)), QDateTime(QDate(2016, 11, endDay), QTime(0, 0)));
    splineAxisX->setFormat("MM-dd");
    splineAxisX->setGridLineVisible(true);
    splineAxisX->setTickCount(endDay - beginDay + 1);
    splineAxisX->setTitleText("Time");
    splineChart->addAxis(splineAxisX, Qt::AlignBottom);
    series->attachAxis(splineAxisX);

    // Create axisY
    splineAxisY = new QValueAxis;
    int maxAxisYValue;
    if(maxValue < 50000){
        maxAxisYValue = 50000;
    } else if (maxValue < 100000){
        maxAxisYValue = 100000;
    } else if(maxValue < 200000){
        maxAxisYValue = 200000;
    } else if (maxValue < 300000){
        maxAxisYValue = 300000;
    } else {
        maxAxisYValue = 500000;
    }
    splineAxisY->setRange(0, maxAxisYValue);
    splineAxisY->setLabelFormat("%d");
//    splineAxisY->setGridLineVisible(true);
    splineAxisY->setTickCount(6);
//    splineAxisY->setMinorTickCount(1);
    splineAxisY->setTitleText("The number of orders");
    splineChart->addAxis(splineAxisY,Qt::AlignLeft);
    series->attachAxis(splineAxisY);

    // Put splineChart into the chartView of ui
    ui->graphicsView_2->setRenderHints(QPainter::Antialiasing);
    ui->graphicsView_2->setChart(splineChart);
}

void DataAnalysisScene::displayInBarChart()
{
    if(!barChart){
        barChart = new QChart;
        barSeries = new QBarSeries;
        barChart->addSeries(barSeries);
    } else {
        delete barAxisX;
        delete barAxisY;
        barSeries->clear();
    }

    auto barSet = new QBarSet("The number of order in a period of time");
    barSet->setColor(QColor(99, 192, 135));
//    barSet->setColor(QColor(61, 138, 254));
    barSeries->append(barSet);

    QStringList cat;

    // Data processing and insert them into series
    // Grid filter
    int choosen[100] = {-1};
    for(int i = 0; i < 100; i++){
        auto pixmapReturn = iconLabel[i]->pixmap(Qt::ReturnByValue);
        if(pixmapReturn == pixmap_choosen){
            choosen[i] = 1;
        } else {
            choosen[i] = 0;
        }
    }

    int date = ui->dateEdit_tag2_date->date().day();

//    int beginDay = ui->timeEdit_tag2_begin_time->date().day();
//    int endDay = ui->timeEdit_tag2_end_time->date().day() + 1;

    int beginUTime = timeToUnixTime(date, ui->timeEdit_tag2_begin_time->time().hour(), ui->timeEdit_tag2_begin_time->time().minute());
    int endUTime = timeToUnixTime(date, ui->timeEdit_tag2_end_time->time().hour(), ui->timeEdit_tag2_end_time->time().minute());
    int timeStep = ui->spinBox_tag2_timestep->value() * 60;

    int step_begin = beginUTime;
    int step_end = (beginUTime + timeStep) < endUTime ? (beginUTime + timeStep) : endUTime;
    int step_middle = step_begin + (step_end - step_begin) / 2;

    int maxValue = 0;
    int count = 0;

    while(step_begin < endUTime){
        int sumInPeriod = 0;
        int startHalfH = (step_begin - BASETIME) / 1800;
        int endHalfH = (step_end - BASETIME) / 1800;
        step_middle = step_begin + (step_end - step_begin) / 2;


//        qDebug() << ui->comboBox_tag1_type->currentText();

        for(int i = 0; i < 100; i++){
            if(choosen[i]){
                for(int j = startHalfH; j < endHalfH; j++){
                    auto type = ui->comboBox_tag2_type->currentText();
                    if(type == "Total orders in the grids"){
                        sumInPeriod += numOfBeginOrdersPerGridPerHour[i][j] + numOfEndOrdersPerGridPerHour[i][j];
                    } else if(type == "Orders of departure in the grids"){
                        sumInPeriod += numOfEndOrdersPerGridPerHour[i][j];
                    } else if(type == "Orders of end in the grids"){
                        sumInPeriod += numOfBeginOrdersPerGridPerHour[i][j];
                    }
                }
            }
        }

        if(sumInPeriod > maxValue){
            maxValue = sumInPeriod;
        }

//        series->append(unixTimeToTime(step_middle).toMSecsSinceEpoch(), sumInPeriod);
        count++;
//        qDebug() << count << sumInPeriod;
        *barSet << sumInPeriod;
//        cat << (QString::number(unixTimeToTime(step_middle).time().hour()) + QString::number(unixTimeToTime(step_middle).time().minute()));
        cat << unixTimeToTime(step_middle).time().toString("hh:mm");

//        total += sumInPeriod;
//        qDebug() << startH << endH << step_begin << step_middle << step_end << unixTimeToTime(step_middle) << sumInPeriod;

        step_begin = step_end;
        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
    }

//    chart->setAnimationOptions(QChart::SeriesAnimations);
    qDebug() << count;

    barAxisX = new QBarCategoryAxis;
    barAxisX->append(cat);
    if(count >= 42){
        barAxisX->setLabelsFont(QFont("Tahoma", 4));
    } else if(count >= 36){
        barAxisX->setLabelsFont(QFont("Tahoma", 6));
    } else if(count >= 30){
        barAxisX->setLabelsFont(QFont("Tahoma", 10));
    } else if(count >= 24){
        barAxisX->setLabelsFont(QFont("Tahoma", 12));
    } else{
        barAxisX->setLabelsFont(QFont("Tahoma", 14));
    }
    barAxisX->setLabelsAngle(90);
    barAxisX->setGridLineVisible(false);
    barChart->addAxis(barAxisX, Qt::AlignBottom);
    barSeries->attachAxis(barAxisX);

    barAxisY = new QValueAxis;
    barAxisY->setLabelsFont(QFont("Tahoma", 13));
    barAxisY->setLabelFormat("%d");
    int maxAxisYValue = 0;
    if(maxValue < 500){
        maxAxisYValue = 500;
    } else if (maxValue < 1000){
        maxAxisYValue = 1000;
    } else if(maxValue < 3000){
        maxAxisYValue = 3000;
    } else if(maxValue < 5000){
        maxAxisYValue = 5000;
    } else if (maxValue < 10000){
        maxAxisYValue = 10000;
    } else if (maxValue < 30000){
        maxAxisYValue = 30000;
    } else if (maxValue < 50000){
        maxAxisYValue = 50000;
    } else if (maxValue < 100000){
        maxAxisYValue = 100000;
    } else {
        maxValue = 200000;
    }
    barAxisY->setRange(0, maxAxisYValue);
    barAxisY->setTickCount(6);
    barChart->addAxis(barAxisY, Qt::AlignLeft);
    barSeries->attachAxis(barAxisY);

//    barChart->legend()->setVisible(true);
//    barChart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setChart(barChart);
}

//void DataAnalysisScene::preLoadNumOfOrders(){
//    int beginDay = 1;
//    int endDay = 16;
//    int timeStep = 3600;

//    int beginUTime = timeToUnixTime(beginDay);
//    int endUTime = timeToUnixTime(endDay);

//    QSqlQuery query;
//    int step_begin = beginUTime;
//    int step_end = beginUTime + timeStep;

//    int hourIdx = 0;

//    while(true){
//        QString command = QString("select count(*) from orders where btime >= %1 and etime < %2").arg(step_begin).arg(step_end);
//        query.exec(command);
//        if(query.next()){
//            auto _begin_datetime = unixTimeToTime(step_begin);
//            auto _end_datetime = unixTimeToTime(step_end);

//            qDebug() << _begin_datetime << unixTimeToTime(step_end) << query.value(0).toInt();

//            numOfOrders[hourIdx++] = query.value(0).toInt();
//        } else {
//            qDebug() << "Error!";
//        }

//        if(step_end == endUTime) break;

//        step_begin = step_end;
//        step_end = (step_end + timeStep > endUTime) ? endUTime : step_end + timeStep;
//    }
//}

void DataAnalysisScene::setDayRange(){
    ui->dateEdit_tag2_date->setMinimumDate(QDate(2016, 11, beginDayLoaded));
    ui->dateEdit_tag2_date->setMaximumDate(QDate(2016, 11, endDayLoaded));
    ui->dateEdit_tag1_begin->setMinimumDate(QDate(2016, 11, beginDayLoaded));
    ui->dateEdit_tag1_begin->setMaximumDate(QDate(2016, 11, endDayLoaded));
    ui->dateEdit_tag1_end->setMinimumDate(QDate(2016, 11, beginDayLoaded));
    ui->dateEdit_tag1_end->setMaximumDate(QDate(2016, 11, endDayLoaded));
}

QDateTime DataAnalysisScene::unixTimeToTime(int uTime)
{
    //    QDate unixStart(1970, 1, 1);
    //    QDate datasetStart(2016, 11, 1);
    //    int daysTo = unixStart.daysTo(datasetStart);
    //    int secondsTo = daysTo * 24 * 3600;
    int secondsTo = 1477958400;

    int dTime = uTime + 8 * 3600;

    int day = (dTime - secondsTo) / 3600 / 24 + 1;
    int hour = (dTime - secondsTo) / 3600 % 24;
    int min = ((dTime - secondsTo) - (day - 1) * 3600 * 24 - hour * 3600) / 60;
    int second = ((dTime - secondsTo) - (day - 1) * 3600 * 24 - hour * 3600) % 60;

    return QDateTime(QDate(2016, 11, day), QTime(hour, min, second));
}

int DataAnalysisScene::timeToUnixTime(int day, int hour, int min, int second)
{
    //    QDate unixStart(1970, 1, 1);
    //    QDate datasetStart(2016, 11, 1);
    //    int daysTo = unixStart.daysTo(datasetStart);
    //    int secondsTo = daysTo * 24 * 3600;
    int secondsTo = 1477958400;

    int uTime = secondsTo + (day - 1) * 3600 * 24 + hour * 3600 + min * 60 + second - 8 * 3600;
    return uTime;
}


DataAnalysisScene::~DataAnalysisScene()
{
    delete ui;
}
