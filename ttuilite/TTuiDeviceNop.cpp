#include "TTuiDevice.h"

#include <iostream>

namespace TTuiLite {


TTuiDevice::TTuiDevice(TTDevType t) {
    throw std::runtime_error("Unsupported Platform");
}

TTuiDevice::~TTuiDevice() {}
void TTuiDevice::start() {}
void TTuiDevice::stop() {}
unsigned TTuiDevice::process(bool) {return 0;}
void TTuiDevice::addCallback(std::shared_ptr<TTuiCallback> ) {}
bool TTuiDevice::buttonState(unsigned ) { return false; }

unsigned TTuiDevice::numPots() { return 0;}
unsigned TTuiDevice::numButtons() { return 0; }
unsigned TTuiDevice::numTrigs() { return 0;}
unsigned TTuiDevice::numGateOut() { return 0;}

void TTuiDevice::gpioSmoothing(unsigned) {}
void TTuiDevice::gpioDeadband(unsigned) {}
void TTuiDevice::gpioPollTime(unsigned){}

void TTuiDevice::gateOut(unsigned,bool){}
void TTuiDevice::ledOut(bool) {}

void TTuiDevice::displayClear(unsigned d){}
void TTuiDevice::displayPaint(){}
// draw funcs
void TTuiDevice::gClear(unsigned d, unsigned clr) {}
void TTuiDevice::gSetPixel(unsigned d, unsigned clr, unsigned x, unsigned y) {}
void TTuiDevice::gFillArea(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {}
void TTuiDevice::gCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {}
void TTuiDevice::gFilledCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {}
void TTuiDevice::gLine(unsigned d, unsigned clr, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {}
void TTuiDevice::gRectangle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {}
void TTuiDevice::gInvert(unsigned d) {}
void TTuiDevice::gText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str) {}
void TTuiDevice::gBitmap(unsigned d, unsigned x, unsigned y, const char *filename) {}
// void gWaveform(unsigned d, unsigned clr, const std::vector<unsigned> &wave); // 128 values, of 0..64
// void gInvertArea(unsigned d, unsigned x, unsigned y, unsigned w, unsigned h);
// void gPng(unsigned d, unsigned x, unsigned y, const char *filename);

// simple text displays
void TTuiDevice::textLine(unsigned d,unsigned clr, unsigned line, unsigned col, const std::string &str) {}
void TTuiDevice::clearLine(unsigned d, unsigned clr, unsigned line){}
// void invertLine(unsigned line);



}// namespace

