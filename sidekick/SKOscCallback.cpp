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
        } else if (addr == "/ttui/gate") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned n = getUnsignedArg(arg);
            unsigned v = getUnsignedArg(arg);
            app_.device().gateOut(n,v);
        } else if (addr == "/ttui/led") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned v = getUnsignedArg(arg);
            app_.device().ledOut(v);
        } else if (addr == "/ttui/displayClear") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            app_.device().displayClear(dId);
        } else if (addr == "/ttui/displayPaint") {
            app_.device().displayPaint();
        } else if (addr == "/ttui/gClear") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            app_.device().gClear(dId,clr);
        } else if (addr == "/ttui/gSetPixel") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            app_.device().gSetPixel(dId,clr, x, y);
        } else if (addr == "/ttui/gFillArea") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned w = getUnsignedArg(arg);
            unsigned h = getUnsignedArg(arg);
            app_.device().gFillArea(dId,clr, x, y, w, h);
        } else if (addr == "/ttui/gCircle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned r = getUnsignedArg(arg);
            app_.device().gCircle(dId,clr, x, y, r);
        } else if (addr == "/ttui/gFilledCircle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned r = getUnsignedArg(arg);
            app_.device().gFilledCircle(dId,clr, x, y, r);
        } else if (addr == "/ttui/gLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x1 = getUnsignedArg(arg);
            unsigned y1 = getUnsignedArg(arg);
            unsigned x2 = getUnsignedArg(arg);
            unsigned y2 = getUnsignedArg(arg);
            app_.device().gLine(dId,clr, x1, y1, x2, y2);
        } else if (addr == "/ttui/gRectangle") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            unsigned w = getUnsignedArg(arg);
            unsigned h = getUnsignedArg(arg);
            app_.device().gRectangle(dId,clr, x, y, w, h);
        } else if (addr == "/ttui/gInvert") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            app_.device().gInvert(dId);
        } else if (addr == "/ttui/gText") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
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
            app_.device().gText(dId,clr, x, y, str);
        } else if (addr == "/ttui/gBitmap") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned x = getUnsignedArg(arg);
            unsigned y = getUnsignedArg(arg);
            const char *filename = (arg++)->AsString();
            app_.device().gBitmap(dId,x, y, filename);
        } else if (addr == "/ttui/textLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
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
            app_.device().textLine(dId,clr, line, col, str);
        } else if (addr == "/ttui/clearLine") {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            unsigned dId = getUnsignedArg(arg);
            unsigned clr = getUnsignedArg(arg);
            unsigned line = getUnsignedArg(arg);
            app_.device().clearLine(dId,clr, line);
        } else {
            std::cerr << "unrecognied msg addr" << addr << std::endl;
        }


    } catch (osc::Exception &e) {
        // any parsing errors such as unexpected argument types, or
        // missing arguments get thrown as exceptions.
        std::cerr << "error while parsing message: " << m.AddressPattern() << ": " << e.what();
    }
}
