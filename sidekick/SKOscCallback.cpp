#include "SKOscCallback.h"
#include "SKApp.h"

#include <iostream>

SKOscCallback::SKOscCallback(SKApp &app) : app_(app) {

}

unsigned getUnsignedArg(osc::ReceivedMessage::const_iterator &arg) {
    if (arg->IsInt32()) {
        return (unsigned) (arg++)->AsInt32();
    } else if (arg->IsFloat()) {
        return (unsigned) (arg++)->AsFloat();
    }
    return 0;
}

void SKOscCallback::ProcessMessage(const osc::ReceivedMessage &m,
                                   const IpEndpointName &remoteEndpoint) {
    (void) remoteEndpoint; // suppress unused parameter warning

    try {
        osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
        std::string addr = m.AddressPattern();
        if (addr == "/sk/activate") {
            app_.stopPatch();
        } else if (addr == "/sk/deviceInfo") {
            app_.sendDeviceInfo();
        } else if (addr == "/ttui/displayClear") {
            // app_.device().displayClear();
        } else if (addr == "/ttui/displayPaint") {
            app_.device().displayPaint();
        } else if (addr == "/ttui/gClear") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            // app_.device().gClear(clr);
        } else if (addr == "/ttui/gSetPixel") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            // app_.device().gSetPixel(clr, x, y);
        } else if (addr == "/ttui/gFillArea") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned w = getUnsignedArg(arg);
            unsigned h = getUnsignedArg(arg);
            // app_.device().gFillArea(clr, x, y, w, h);
        } else if (addr == "/ttui/gCircle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned r = getUnsignedArg(arg);
            // app_.device().gCircle(clr, x, y, r);
        } else if (addr == "/ttui/gFilledCircle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned r = getUnsignedArg(arg);
            // app_.device().gFilledCircle(clr, x, y, r);
        } else if (addr == "/ttui/gLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x1 = getUnsignedArg(arg);
            unsigned y1 = getUnsignedArg(arg);
            unsigned x2 = getUnsignedArg(arg);
            unsigned y2 = getUnsignedArg(arg);
            // app_.device().gLine(clr, x1, y1, x2, y2);
        } else if (addr == "/ttui/gRectangle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned w = getUnsignedArg(arg);
            unsigned h = getUnsignedArg(arg);
            // app_.device().gRectangle(clr, x, y, w, h);
        } else if (addr == "/ttui/gInvert") {
            // app_.device().gInvert();
        } else if (addr == "/ttui/gText") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            std::string str;
            char buf[128];
            while (arg != m.ArgumentsEnd()) {
                if (arg->IsString()) {
                    const char *v = (arg)->AsString();
                    str = str + v + " ";
                } else if (arg->IsInt32()) {
                    int v = (arg)->AsInt32();
                    sprintf(buf, "%d ", v);
                    str = str + buf;
                } else if (arg->IsFloat()) {
                    float v = (arg)->AsFloat();
                    sprintf(buf, "%g ", v);
                    str = str + buf;
                }
                arg++;
            }
            // app_.device().gText(clr, x, y, str);
        } else if (addr == "/ttui/gWaveform") {
            std::vector<unsigned> wave;
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            if (arg->IsBlob()) {
                const void *blob;
                osc::osc_bundle_element_size_t size;
                (arg++)->AsBlob(blob, size);
                for (int i = 0; i < size; i++) {
                    const uint8_t *values = static_cast<const uint8_t *>(blob);
                    wave.push_back(values[i]);
                }
            }
            // app_.device().gWaveform(clr, wave); // 128 values, of 0..64
        } else if (addr == "/ttui/gInvertArea") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned w = getUnsignedArg(arg);
            unsigned h = getUnsignedArg(arg);
            // app_.device().gInvertArea(x, y, w, h);
        } else if (addr == "/ttui/gPng") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            const char *filename = (arg++)->AsString();
            // app_.device().gPng(x, y, filename);
        } else if (addr == "/ttui/textLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned line = getUnsignedArg(arg);
            unsigned col = getUnsignedArg(arg);
            std::string str;
            char buf[128];
            while (arg != m.ArgumentsEnd()) {
                if (arg->IsString()) {
                    const char *v = (arg)->AsString();
                    str = str + v + " ";
                } else if (arg->IsInt32()) {
                    int v = (arg)->AsInt32();
                    sprintf(buf, "%d ", v);
                    str = str + buf;
                } else if (arg->IsFloat()) {
                    float v = (arg)->AsFloat();
                    sprintf(buf, "%g ", v);
                    str = str + buf;
                }
                arg++;
            }
            // app_.device().textLine(clr, line, col, str);
        } else if (addr == "/ttui/invertLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned line = getUnsignedArg(arg);
            // app_.device().invertLine(line);
        } else if (addr == "/ttui/clearLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned clr = getUnsignedArg(arg);
            unsigned line = getUnsignedArg(arg);
            // app_.device().clearLine(clr, line);
        } else {
            std::cerr << "unrecognied msg addr" << addr << std::endl;
        }


    } catch (osc::Exception &e) {
        // any parsing errors such as unexpected argument types, or
        // missing arguments get thrown as exceptions.
        std::cerr << "error while parsing message: " << m.AddressPattern() << ": " << e.what();
    }
}
