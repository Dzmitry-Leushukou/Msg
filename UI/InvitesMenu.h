#pragma once
#include "Page.h"
#include <string>
class InvitesMenu :
    public Page
{
public:
    InvitesMenu(Application& app);
    virtual void start() override;
private:
    void help();
};

