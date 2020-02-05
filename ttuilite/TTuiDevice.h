#pragma once

#include <string>
#include <memory>

namespace TTuiLite {


class TTuiCallback {
public:
    TTuiCallback() { ; }

    virtual ~TTuiCallback() = default;
    virtual void onButton(unsigned id, unsigned value) = 0;
    virtual void onEncoder(unsigned id, int value) = 0;
};


class TTuiDeviceImpl_;

class TTuiDevice {
public:
    TTuiDevice(const char *resourcePath = nullptr);
    ~TTuiDevice();

    void start();
    void stop();
    unsigned process(bool paint = true);
    void addCallback(std::shared_ptr<TTuiCallback>);

    bool buttonState(unsigned but);
    unsigned numEncoders();

    void displayClear();
    void displayPaint();
    // draw funcs
    void clearRect(unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h);
    void drawText(unsigned clr, unsigned x, unsigned y, const std::string &str);
    void drawPNG(unsigned x, unsigned y, const char *filename);

    // simple text displays
    void displayText(unsigned clr, unsigned line, unsigned col, const std::string &str);
    void clearText(unsigned clr, unsigned line);
    void invertText(unsigned line);
private:
    TTuiDeviceImpl_ *impl_;
};


}
