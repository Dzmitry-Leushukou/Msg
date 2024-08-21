#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QMainWindow>
#include "D:\Programming\Projects\Msg\Source\NetworkLib\NetworkLib.h"
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_SignInButton_clicked();

    void on_SignOutButton_clicked();

    void on_RegisterButton_clicked();

private:

    //Menu types
    void load();
    void authorization();
    void registration();
    void hide_all();
    //MsgBoxes
    void connection_status();
    //Checks
    bool correct_authorization_data();
    bool correct_login();
    bool correct_password();
    //Fields
    Ui::MainWindow *ui;



};
#endif // AUTHORIZATION_H
