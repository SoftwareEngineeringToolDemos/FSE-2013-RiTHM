#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>

#include "settingswindow.h"
#include "cpuplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
       void BrowseSrcDir();
       void BrowseOutputDir();
       void BrowsePropFilepath();
       void RunTool();
       void LaunchSettingsWindow();

       void on_pushButton_clicked();
       void DynamicChanged(int index);
       void ControllerChanged(const QString &arg1);
       void FixedToggled(bool checked);
       void DynamicToggled(bool checked);

       void on_pushButtonViewLog_clicked();

private:
    enum SolveType {
        SOLVE_ILP = 0,
        SOLVE_H1,
        SOLVE_H2,
        SOLVE_SAT,
        SOLVE_GREEDY,
        SOLVE_TYPE_MAX
    };

    static const QString SolveTypeArg[SOLVE_TYPE_MAX];
    static const QString GooMFAlgType[4];
    static const QString GooMFInvokeType[2];

    Ui::MainWindow *ui;
    QAction *about;
    QAction *settings;
    QAction *run;
    MonitorPlot *plot;
};

#endif // MAINWINDOW_H
