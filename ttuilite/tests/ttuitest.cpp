#include "TTuiDevice.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <signal.h>

static volatile bool keepRunning = 1;


TTuiLite::TTuiDevice device;


#define DISPLAY 0

void intHandler(int dummy) {
    std::cerr << "LibTTuiTest intHandler called" << std::endl;
    if (!keepRunning) {
        sleep(1);
        exit(-1);
    }
    keepRunning = false;
}


class TestCallback : public TTuiLite::TTuiCallback {
public:
    void onButton(unsigned id, unsigned value) override {
        char buf[100];
        sprintf(buf, "Button %d : %d", id, value);
        device.clearText(DISPLAY,0,0);
        device.displayText(DISPLAY,15, 0, 1, buf);
        fprintf(stderr, "button %d : %d\n", id, value);
        if (value) {
            switch (id) {
                case 0 : {
                    device.clearText(DISPLAY,0, 2);
                    device.displayText(DISPLAY,15, 2, 0, "hello");
                    device.displayText(DISPLAY,15, 2, 25, "world");
                    break;
                }
                case 1 :
                    device.clearText(DISPLAY,0, 2);
                    break;
                default:
                    break;

            }
        }
    }

    void onEncoder(unsigned id, int value) override {
        char buf[100];
        sprintf(buf, "Encoder %d : %d ", id, value);
        device.clearText(DISPLAY,0,0);
        device.displayText(DISPLAY,15, 0, 1, buf);
        fprintf(stderr, "encoder %d : %d\n", id, value);
    }
};


// note: we are only painting every second to avoid tight loop here 
void funcParam(unsigned display, unsigned row, unsigned col, const std::string &name, const std::string &value, bool selected = false) {
    unsigned x = col * 64;
    unsigned y1 = (row + 1) * 20;
    unsigned y2 = y1 + 10;
    device.clearRect(display,0, x, y1, 62 + (col * 2), 10);
    device.drawText(display,15, x + 1, y1 - 1, name);
    device.clearRect(display,0, x, y2, 62 + (col * 2), 10);
    device.drawText(display,15, x + 1, y2 - 1, value);
}

int main(int argc, const char *argv[]) {
    std::cout << "starting test" << std::endl;
    device.addCallback(std::make_shared<TestCallback>());

    device.start();

    signal(SIGINT, intHandler);
    //device.displayClear(0);
    //device.displayClear(1);
    //device.displayPaint();


    std::cout << "draw bitmap" << std::endl;
    device.drawBitmap(0,0, 0, "./orac.pbm");

    std::cout << "draw some text" << std::endl;
    //device.clearText(1, 1, 0);
    device.displayText(1, 15, 0, 0, "display 1");
    device.displayPaint();
    sleep(1);
    device.displayClear(0);
    device.displayText(0, 15, 0, 0, "finished 0");
    device.displayClear(1);
    device.displayText(1, 15, 0, 0, "finished 1");
    device.displayPaint();
    sleep(1);

    
    device.displayClear(0);
    //device.clearRect(0,0,0, 128,10,1);
    device.drawText(0,15,0,8,"a1 : BasicPoly > main");
    funcParam(0,0,0,"Transpose","12     st");
    funcParam(0,1,0,"Cutoff","15000 hz");
    funcParam(0,0,1,"Shape","33",true);
    funcParam(0,1,1,"Envelope","58     %");
    device.displayPaint();
    sleep(1);

    funcParam(0,1,0,"Cutoff","25000 hz");
    device.displayPaint();


    /*
    std::cout << "started test" << std::endl;

    std::cout << "buttons : "
              << device.buttonState(0)
              << device.buttonState(1)
              << device.buttonState(2)
              << std::endl;

    while (keepRunning) {
        device.process();
        sleep(1);
    }
    */
    std::cout << "stopping test" << std::endl;
    device.stop();
    return 0;
}
