#pragma once

#include "TTuiDevice.h"


class SKApp;

class SKCallback : public TTuiLite::TTuiCallback {
public:
    explicit SKCallback(SKApp &app);
    void init();
    void onButton(unsigned id, unsigned value) override;
    void onEncoder(unsigned id, int value) override;

private:
    SKApp &app_;
};

