#include "SKHardwareCallback.h"
#include "SKApp.h"

SKHardwareCallback::SKHardwareCallback(SKApp &app) : app_(app) {

}

void SKHardwareCallback::init() {
}

void SKHardwareCallback::onButton(unsigned id, unsigned value) {
    app_.onButton(id, value);
}

void SKHardwareCallback::onPot(unsigned id, int value) {
    app_.onPot(id, value);
}
