#include "screepslogindialog.h"
#include "ui_screepslogindialog.h"

ScreepsLoginDialog::ScreepsLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreepsLoginDialog)
{
    ui->setupUi(this);
}

ScreepsLoginDialog::~ScreepsLoginDialog()
{
    delete ui;
}

void ScreepsLoginDialog::accept()
{
    emit DoLogin(ui->emailEdt->text(), ui->passwordEdit->text());
}

void ScreepsLoginDialog::LoginFailed()
{
    ui->errorLabel->setText("Login failed :(");
}

void ScreepsLoginDialog::LoginSuccess()
{
    // TODO this is testing only
    ui->errorLabel->setText("Successfully logged in");
}
