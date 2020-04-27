#include "TTuiDevice.h"

#include <iostream>

namespace TTuiLite {


TTuiDevice::TTuiDevice() {
    throw std::runtime_error("Unsupported Platform");
}

TTuiDevice::~TTuiDevice() {
}

void TTuiDevice::start() {
}

void TTuiDevice::stop() {
}

unsigned TTuiDevice::process(bool) {
    return 0;
}

void TTuiDevice::addCallback(std::shared_ptr<TTuiCallback> ) {
}

bool TTuiDevice::buttonState(unsigned ) {
    return false;
}

unsigned TTuiDevice::numPots() {
    return 0;
}

unsigned TTuiDevice::numButtons() {
    return 0;
}

void TTuiDevice::displayPaint() {
}

void TTuiDevice::displayClear(unsigned ) {
}

void TTuiDevice::displayText(unsigned , unsigned , unsigned , unsigned , const std::string &) {
}


void TTuiDevice::clearText(unsigned , unsigned , unsigned ) {
}

void TTuiDevice::drawBitmap(unsigned , unsigned , unsigned , const char *) {
}

void TTuiDevice::clearRect(unsigned , unsigned , unsigned , unsigned , unsigned w, unsigned ) {
}

void TTuiDevice::drawText(unsigned , unsigned , unsigned , unsigned , const std::string &) {
}


}// namespace

