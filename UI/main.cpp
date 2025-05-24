#include "UI.h"
#include "Application.h"
int main()
{
	Application* app = new Application();
	app->regUser("test2", "milana");
	return 0;
	UI* ui = new UI();
	ui->start();
	delete ui;
	ui = nullptr;
}