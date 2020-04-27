#pragma once

#include <string>
#include <memory>

namespace TTuiLite {


//Note: only display functions work at this time!

class TTuiCallback {
public:
    TTuiCallback() { ; }

    virtual ~TTuiCallback() = default;
    virtual void onButton(unsigned id, unsigned value) = 0;
    virtual void onPot(unsigned id, int value) = 0;
};


class TTuiDeviceImpl_;

class TTuiDevice {
public:
    TTuiDevice();
    ~TTuiDevice();

    void start();
    void stop();
    unsigned process(bool paint = true);
    void addCallback(std::shared_ptr<TTuiCallback>);
    bool buttonState(unsigned but);
    unsigned numPots();
    unsigned numButtons();

    void displayClear(unsigned d);
    void displayPaint();
    // draw funcs
    void clearRect(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h);
    void drawText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str);
    void drawBitmap(unsigned d, unsigned x, unsigned y, const char *filename);

    // simple text displays
    void displayText(unsigned d,unsigned clr, unsigned line, unsigned col, const std::string &str);
    void clearText(unsigned d, unsigned clr, unsigned line);
private:
    TTuiDeviceImpl_ *impl_;
};


}
