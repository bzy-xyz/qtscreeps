#include "screepsmainwindow.h"
#include "ui_screepsmainwindow.h"


ScreepsMainWindow::ScreepsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreepsMainWindow)
{
    ui->setupUi(this);
    ui->memoryBar->setRange(0, SCREEPS_MEMCAP);

    connect(nm, SIGNAL(GotCPUMessage(int,int)), this, SLOT(UpdateCPU(int,int)));

    messages = new ScreepsLogWindow();
    errors = new ScreepsLogWindow();
    console = new ScreepsConsoleWindow();

    messages->setWindowTitle("Script Messages");
    errors->setWindowTitle("Errors");

    connect(nm, SIGNAL(GotConsoleLog(QList<QString>)), messages, SLOT(pushMessages(QList<QString>)));
    connect(nm, SIGNAL(GotConsoleError(QString)), errors, SLOT(pushMessage(QString)));

    connect(nm, SIGNAL(GotConsoleResults(QList<QString>)), console, SLOT(addIncomingLines(QList<QString>)));
    connect(nm, SIGNAL(GotConsoleError(QString)), console, SLOT(addIncomingLine(QString)));
    connect(console, SIGNAL(lineEntered(QString)), nm, SLOT(DoSendConsoleCommand(QString)));
    connect(console, SIGNAL(lineEntered(QString)), console, SLOT(addOutgoingLine(QString)));

    connect(ui->actionMessageLog, SIGNAL(toggled(bool)), messages, SLOT(setVisible(bool)));
    connect(ui->actionErrorLog, SIGNAL(toggled(bool)), errors, SLOT(setVisible(bool)));
    connect(ui->actionConsole, SIGNAL(toggled(bool)), console, SLOT(setVisible(bool)));

    connect(messages, SIGNAL(closed()), ui->actionMessageLog, SLOT(toggle()));
    connect(errors, SIGNAL(closed()), ui->actionErrorLog, SLOT(toggle()));
    connect(console, SIGNAL(closed()), ui->actionConsole, SLOT(toggle()));

    connect(ui->graphicsView, SIGNAL(StatusBarSignal(QString)), this, SLOT(SetStatusBar(QString)));
}

ScreepsMainWindow::~ScreepsMainWindow()
{
    delete ui;
}

void ScreepsMainWindow::closeEvent(QCloseEvent * ev)
{
    ev->accept();
    QApplication::exit();
}

void ScreepsMainWindow::SetStatusBar(QString text)
{
    ui->statusBar->showMessage(text);
}

void ScreepsMainWindow::UpdateCPU(int cpu, int memory)
{

    int cpuMax = nm->getMeParam("cpu").toInt();

    ui->cpuBar->setRange(0, cpuMax);
    ui->cpuBar->setValue(std::min(cpu, cpuMax));
    ui->cpuBar->setFormat(QString("CPU: %1 / %2").arg(cpu).arg(cpuMax));


    ui->memoryBar->setValue(std::min(memory, SCREEPS_MEMCAP));
    ui->memoryBar->setFormat(QString("Mem: %1 / %2").arg(memory).arg(SCREEPS_MEMCAP));
}
