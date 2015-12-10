#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "screepsnetworkmanager.h"

#include "screepslogwindow.h"

#define SCREEPS_MEMCAP 2048 * 1024

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void UpdateCPU(int cpu, int memory);


private:
    Ui::MainWindow *ui;

    ScreepsNetworkManager * nm = &ScreepsNetworkManager::defaultInstance();

    ScreepsLogWindow * messages;
    ScreepsLogWindow * errors;

};

#endif // MAINWINDOW_H
