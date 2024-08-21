#include "authorization.h"
#include "ui_Authorization.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    on_SignInButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load()
{
    hide_all();
    ui->Logo->show();
    ui->CheckProgress->show();
    ui->CheckAuthorizationLabel->show();
}

void MainWindow::authorization()
{
    hide_all();
    ui->Login->show();
    ui->LoginLabel->show();
    ui->PasswordLabel->show();
    ui->Password->show();
    ui->SignInButton->show();
    ui->SignOutButton->show();
}

void MainWindow::registration()
{
    hide_all();
    ui->RegisterButton->show();
    ui->LoginReg->show();
    ui->LoginLabelReg->show();
    ui->PasswordLabelReg->show();
    ui->PasswordReg->show();
    ui->EmailHintLabelReg->show();
    ui->EmailLabelReg->show();
    ui->EmailReg->show();
}

void MainWindow::hide_all()
{
    QList<QWidget *> widgets = findChildren<QWidget *>();
    for (QWidget *widget : widgets)
    {
        if(widget->objectName()!="centralwidget")
        widget->hide();
    }
}

bool MainWindow::correct_authorization_data()
{
    ui->CheckProgress->setValue(0);

    bool status = NetworkLib::internet_connection();
    ui->CheckProgress->setValue(33);

    status = status && correct_login();
    ui->CheckProgress->setValue(66);

    status = status && correct_password();
    ui->CheckProgress->setValue(100);

    return status;
}

bool MainWindow::correct_login()
{
    return true;
}

bool MainWindow::correct_password()
{
    return true;
}

void MainWindow::connection_status()
{
    if(NetworkLib::internet_connection())
    {
        QMessageBox::warning(0,"Connetcion error", "Check your internet connection");
    }
}

void MainWindow::on_SignInButton_clicked()
{
    load();

    if(!correct_authorization_data())
        //Go to chats menu
        ;
    else
        authorization();

}


void MainWindow::on_SignOutButton_clicked()
{
    registration();
}


void MainWindow::on_RegisterButton_clicked()
{
    on_SignInButton_clicked();
}

