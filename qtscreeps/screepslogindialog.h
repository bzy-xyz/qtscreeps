#ifndef SCREEPSLOGINDIALOG_H
#define SCREEPSLOGINDIALOG_H

#include <QtCore>
#include <QtWidgets>

namespace Ui {
class ScreepsLoginDialog;
}

class ScreepsLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreepsLoginDialog(QWidget *parent = 0);
    ~ScreepsLoginDialog();

signals:
    void DoLogin(QString email, QString password);

public slots:
    void accept();
    //void reject();

    void LoginFailed();
    void LoginSuccess();

private:
    Ui::ScreepsLoginDialog *ui;
};

#endif // SCREEPSLOGINDIALOG_H
