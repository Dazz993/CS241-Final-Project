#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <dataanalysisscene.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // flags
    bool dirLoaded;

    QSqlDatabase db;                                    // Database
    QStringList filePathList;                           // List of the files

    DataAnalysisScene *dataScene = NULL;

    void initDatabase();
    void getFileNamesInDir();
    void loadFilesToDB();

    void setProgressBar(int);

signals:
    void signalLoadFilesDone();
    void signalProgressBar(int);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
