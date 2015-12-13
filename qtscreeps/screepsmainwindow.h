#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "screepsnetworkmanager.h"

#include "screepslogwindow.h"
#include "screepsconsolewindow.h"

#define SCREEPS_MEMCAP 2048 * 1024

namespace Ui {
class ScreepsMainWindow;
}

class ScreepsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreepsMainWindow(QWidget *parent = 0);
    ~ScreepsMainWindow();

public slots:
    void UpdateCPU(int cpu, int memory);
    void SetStatusBar(QString text);


protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::ScreepsMainWindow *ui;

    ScreepsNetworkManager * nm = &ScreepsNetworkManager::defaultInstance();

    ScreepsLogWindow * messages;
    ScreepsLogWindow * errors;
    ScreepsConsoleWindow * console;

};

#endif // MAINWINDOW_H
