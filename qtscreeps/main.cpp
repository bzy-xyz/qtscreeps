#include <QtCore>
#include <QtWidgets>

#include "mainwindow.h"
#include "screepslogindialog.h"
#include "screepsnetworkmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    ScreepsNetworkManager & nm = ScreepsNetworkManager::defaultInstance();
    ScreepsLoginDialog ld;

    QObject::connect(&ld, SIGNAL(DoLogin(QString,QString)), &nm, SLOT(DoLogin(QString,QString)));
    QObject::connect(&nm, SIGNAL(DoLoginSuccess()), &ld, SLOT(LoginSuccess()));
    QObject::connect(&nm, SIGNAL(DoLoginFailed()), &ld, SLOT(LoginFailed()));

    ld.show();
    return a.exec();
}
