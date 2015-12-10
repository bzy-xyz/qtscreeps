#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->memoryBar->setRange(0, SCREEPS_MEMCAP);

    connect(nm, SIGNAL(GotCPUMessage(int,int)), this, SLOT(UpdateCPU(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateCPU(int cpu, int memory)
{

    int cpuMax = nm->getMeParam("cpu").toInt();

    ui->cpuBar->setRange(0, cpuMax);
    ui->cpuBar->setValue(std::min(cpu, cpuMax));
    ui->cpuBar->setFormat(QString("CPU: %1 / %2").arg(cpu).arg(cpuMax));


    ui->memoryBar->setValue(std::min(memory, SCREEPS_MEMCAP));
    ui->memoryBar->setFormat(QString("Mem: %1 / %2").arg(memory).arg(SCREEPS_MEMCAP));
}
