#pragma once
#include "Page.h"
#include <string>
#include <queue>
class RequestsMenu :
    public Page
{
public:
    RequestsMenu(Application& app);
    virtual void start() override;
private:
    void help();
};

