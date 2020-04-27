#pragma once

#include "TTuiDevice.h"


class SKApp;

class SKHardwareCallback : public TTuiLite::TTuiCallback {
public:
    explicit SKHardwareCallback(SKApp &app);
    void init();
    void onButton(unsigned id, unsigned value) override;
    void onPot(unsigned id, unsigned value) override;
    void onTrig(unsigned id, unsigned value) override;
    
private:
    SKApp &app_;
};

