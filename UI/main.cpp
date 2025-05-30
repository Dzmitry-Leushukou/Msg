#include "UI.h"
#include "Application.h"
#include "Text.h"
int main()
{
	UI* ui = new UI();
	ui->start();
	delete ui;
	ui = nullptr;
}