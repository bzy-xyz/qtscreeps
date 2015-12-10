#include "screepslogindialog.h"
#include "ui_screepslogindialog.h"

#include "mainwindow.h"

#include "screepslocalconfigstore.h"

ScreepsLoginDialog::ScreepsLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreepsLoginDialog)
{
    ui->setupUi(this);
    ui->errorLabel->setText("");

    ScreepsLocalConfigStore & configStore = ScreepsLocalConfigStore::defaultInstance();
    int rememberMe = configStore.get("login_rememberMe").toInt();
    if(rememberMe)
    {
        ui->rememberMeCheckbox->setChecked(true);
        ui->emailEdt->setText(configStore.get("login_savedEmail"));
        ui->passwordEdit->setText(configStore.get("login_savedPassword"));
    }
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

    ScreepsLocalConfigStore & configStore = ScreepsLocalConfigStore::defaultInstance();
    // Save credentials if requested
    if(ui->rememberMeCheckbox->isChecked())
    {
        configStore.set("login_rememberMe", "1");
        configStore.set("login_savedEmail", ui->emailEdt->text());
        configStore.set("login_savedPassword", ui->passwordEdit->text());
    }
    else
    {
        configStore.set("login_rememberMe", "0");
        configStore.set("login_savedEmail", "");
        configStore.set("login_savedPassword", "");
    }

    // Deliberately creating a window without knowing its lifetime;
    // QApplication tracks top level widgets automatically
    // so this should be OK

    auto mw = new MainWindow();
    mw->show();

    this->close();
}
