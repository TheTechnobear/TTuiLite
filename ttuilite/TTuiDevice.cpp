#include "TTuiDevice.h"

#include <memory>
#include <vector>
#include <thread>

#include <fstream>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>



// gpio
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/input.h>
#include <time.h>
#include <sys/poll.h>
#include <sys/stat.h>

#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"

#include <algorithm>
#include <readerwriterqueue.h>
#include "TTgpio.h"

static unsigned gpioPollTime_=1000;

#ifndef SCREEN_X
#define SCREEN_X 128
#endif

#ifndef SCREEN_Y
#define SCREEN_Y 64
#endif

namespace TTuiLite {

volatile bool digOutState[MAX_DIG_OUT] = {false,false,false};

struct TTuiEventMsg {
    enum {
        N_BUTTON,
        N_TRIG,
        N_POT
    } type_;
    unsigned id_;
    int value_;
};

// implementation class
class TTuiDeviceImpl_ {
public:
    TTuiDeviceImpl_();
    ~TTuiDeviceImpl_();

    void start();
    void stop();
    unsigned process(bool);
    void addCallback(std::shared_ptr<TTuiCallback>);

    bool buttonState(unsigned but);
    unsigned numPots();
    unsigned numButtons();
    unsigned numTrigs();
    unsigned numGateOut();

    void gateOut(unsigned,bool);
    void ledOut(bool);



    void displayPaint();

    void displayClear(unsigned d);
    // text functions 
    void textLine(unsigned d, unsigned clr, unsigned line, unsigned col, const std::string &str);
    void clearLine(unsigned d, unsigned clr, unsigned line);

    // draw functions
    void gPaint();
    void gClear(unsigned d, unsigned clr);
    void gSetPixel(unsigned d, unsigned clr, unsigned x, unsigned y);
    void gFillArea(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h);
    void gCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r);
    void gFilledCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r);
    void gLine(unsigned d, unsigned clr, unsigned x1, unsigned y1, unsigned x2, unsigned y2);
    void gRectangle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h);
    void gInvert(unsigned d);
    void gText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str);
    void gBitmap(unsigned d, unsigned x, unsigned y, const char *filename);

    // public but not part of interface!
    void processGPIO();

private:
    void initGPIO();
    void deinitGPIO();
    void initDisplay();
    void deinitDisplay();


    bool keepRunning_;



    static constexpr unsigned MAX_DISPLAYS = 2;
    ArduiPi_OLED display_[MAX_DISPLAYS];
    bool dirty_[MAX_DISPLAYS]= {true, true};

    std::thread gpioThread_;
    static constexpr unsigned MAX_EVENTS = 64;
    int keyFd_;
    std::vector<std::shared_ptr<TTuiCallback>> callbacks_;
    moodycamel::ReaderWriterQueue<TTuiEventMsg> eventQueue_;
};

//TTuiDevice proxy
TTuiDevice::TTuiDevice() {
    impl_ = new TTuiDeviceImpl_();
}

TTuiDevice::~TTuiDevice() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

void TTuiDevice::gpioPollTime(unsigned t) {
    gpioPollTime_=t;    
} 

void TTuiDevice::start() {
    impl_->start();
}

void TTuiDevice::stop() {
    impl_->stop();
}

unsigned TTuiDevice::process(bool paint) {
    return impl_->process(paint);
}

void TTuiDevice::addCallback(std::shared_ptr<TTuiCallback> cb) {
    impl_->addCallback(cb);

}

bool TTuiDevice::buttonState(unsigned but) {
    return impl_->buttonState(but);
}

unsigned TTuiDevice::numPots() {
    return impl_->numPots();
}

unsigned TTuiDevice::numButtons() {
    return impl_->numButtons();
}

unsigned TTuiDevice::numTrigs() {
    return impl_->numTrigs();
}

unsigned TTuiDevice::numGateOut() {
    return impl_->numGateOut();
}

void TTuiDevice::gateOut(unsigned n ,bool v) {
    impl_->gateOut(n,v);
}

void TTuiDevice::ledOut(bool v) {
    impl_->ledOut(v);
}


void TTuiDevice::displayPaint() {
    impl_->displayPaint();
}

void TTuiDevice::displayClear(unsigned d) {
    impl_->displayClear(d);
}

void TTuiDevice::textLine(unsigned d, unsigned clr, unsigned line, unsigned col, const std::string &str) {
    impl_->textLine(d,clr, line, col, str);
}

void TTuiDevice::clearLine(unsigned d, unsigned clr, unsigned line) {
    impl_->clearLine(d, clr, line);
}

void TTuiDevice::gClear(unsigned d, unsigned clr) {
    impl_->gClear(d, clr);
}

void TTuiDevice::gSetPixel(unsigned d, unsigned clr, unsigned x, unsigned y) {
    impl_->gSetPixel(d, clr,x,y);
}

void TTuiDevice::gFillArea(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    impl_->gFillArea(d, clr,x,y,w,h);
}

void TTuiDevice::gCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {
    impl_->gCircle(d, clr,x,y,r);
}

void TTuiDevice::gFilledCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {
    impl_->gFilledCircle(d, clr,x,y,r);
}

void TTuiDevice::gLine(unsigned d, unsigned clr, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
    impl_->gLine(d, clr,x1,y1,x2,y2);
}

void TTuiDevice::gInvert(unsigned d) {
    impl_->gInvert(d);
}

void TTuiDevice::gBitmap(unsigned d, unsigned x, unsigned y, const char *filename) {
    impl_->gBitmap(d, x, y, filename);
}

void TTuiDevice::gRectangle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    impl_->gRectangle(d,clr, x, y, w, h);
}

void TTuiDevice::gText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str) {
    impl_->gText(d, clr, x, y, str);
}


//// start of IMPLEMENTATION

TTuiDeviceImpl_::TTuiDeviceImpl_() : eventQueue_(MAX_EVENTS) {
}

TTuiDeviceImpl_::~TTuiDeviceImpl_() {
    ;
}

void TTuiDeviceImpl_::addCallback(std::shared_ptr<TTuiCallback> cb) {
    callbacks_.push_back(cb);
}

void TTuiDeviceImpl_::start() {
    keepRunning_ = true;
    initGPIO();
    initDisplay();
}

void TTuiDeviceImpl_::stop() {
    keepRunning_ = false;

    deinitDisplay();
    deinitGPIO();
}

unsigned TTuiDeviceImpl_::process(bool paint) {
    TTuiEventMsg msg;
    while (eventQueue_.try_dequeue(msg)) {
        for (auto cb: callbacks_) {
            switch (msg.type_) {
                case TTuiEventMsg::N_BUTTON : {
                    cb->onButton(msg.id_, msg.value_);
                    break;
                }
                case TTuiEventMsg::N_TRIG : {
                    cb->onTrig(msg.id_, msg.value_);
                    break;
                }
                case TTuiEventMsg::N_POT : {
                    cb->onPot(msg.id_, msg.value_);
                    break;
                }
                default:; // ignore
            }
        }
    }


    if (paint) displayPaint();
    return 0;
}


bool TTuiDeviceImpl_::buttonState(unsigned but) {
    return false;
    // char key = 0;
    // ioctl(keyFd_, EVIOCGKEY(1), &key);
    // return ((unsigned) key) & (1 << but + 1);
}


unsigned TTuiDeviceImpl_::numPots() {
    return 0;
}

unsigned TTuiDeviceImpl_::numButtons() {
    return 3;
}


unsigned TTuiDeviceImpl_::numTrigs() {
    return MAX_DIG_IN - numButtons();
}

unsigned TTuiDeviceImpl_::numGateOut() {
    return MAX_DIG_OUT - 1;
}

void TTuiDeviceImpl_::gateOut(unsigned n ,bool v) {
    if(n < (MAX_DIG_OUT-1)) { 
        digOutState[n+1]=v;
    }
}

void TTuiDeviceImpl_::ledOut(bool v) {
    digOutState[0]=v;
}


//// display functions

void TTuiDeviceImpl_::gPaint() {
    for(int d=0;d<2;d++) {
        auto& display=display_[d];
        if(dirty_[d]) display.display();
        dirty_[d] = false;
    }
}

void TTuiDeviceImpl_::gRectangle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.drawRect(x,y,w,h,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gClear(unsigned d, unsigned clr) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.fillScreen(colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gSetPixel(unsigned d, unsigned clr, unsigned x, unsigned y) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.drawPixel(x,y,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gFillArea(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.fillRect(x,y,w,h,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.drawCircle(x,y,r,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gFilledCircle(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned r) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.fillCircle(x,y,r,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gLine(unsigned d, unsigned clr, unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.drawLine(x1,y1,x2,y2,colour);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gInvert(unsigned d) {
    if(d>MAX_DISPLAYS) return;
    auto& display=display_[d];
    display.invertDisplay(true);
    dirty_[d] = true;
}

void TTuiDeviceImpl_::gText(unsigned d,unsigned clr, unsigned x, unsigned y, const std::string &str) {
    if(d>MAX_DISPLAYS) return;
    auto& display=display_[d];
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);

    uint16_t bg = (clr==WHITE ? BLACK : WHITE );
    display.setCursor(x,y);
    display.setTextColor(colour, bg );
    display.setTextSize(1);
    display.setTextWrap(false);
    display.print(str.c_str());
    dirty_[d] = true;
}

void TTuiDeviceImpl_::displayPaint() {
    gPaint();
}


void TTuiDeviceImpl_::displayClear(unsigned d) {
    if(d>MAX_DISPLAYS) return;
    auto& display=display_[d];
    display.clearDisplay();
    dirty_[d] = true;
}

void TTuiDeviceImpl_::textLine(unsigned d,unsigned clr, unsigned line, unsigned col, const std::string &str) {
    unsigned x = col * 4;
    unsigned y = line * 10 + 10;
    gText(d,clr,x,y,str);
}

void TTuiDeviceImpl_::clearLine(unsigned d,unsigned clr, unsigned line) {
    unsigned x = 0;
    unsigned y = (line * 10 + 10) - 1;
    gFillArea(d, clr, x, y, SCREEN_X, 10);
    dirty_[d] = true;
}


void TTuiDeviceImpl_::gBitmap(unsigned d,unsigned x, unsigned y, const char *filename) {
    if(d>MAX_DISPLAYS) return;
    auto& display=display_[d];
    int16_t img_w=SCREEN_X, img_h=SCREEN_Y;
    uint8_t* bitmap=nullptr;

    std::ifstream infile;
    infile.open(filename, std::ios::binary);
    infile.seekg(0, std::ios::end);
    size_t file_size_in_byte = infile.tellg();
    std::vector<char> data; // used to store text data
    data.resize(file_size_in_byte);
    infile.seekg(0, std::ios::beg);
    infile.read(&data[0], file_size_in_byte);

    int idx=0;
    for(int i=0;i<3;i++) {
        while(data[idx]!='\n' && idx<file_size_in_byte) {
            idx++;
        }
	idx++;
        if(idx>=file_size_in_byte) {
            return;
        }
    }
    //bitmap = static_cast<uint8_t*>(&data[idx]);
    for(int i=idx;i<file_size_in_byte;i++) {
	    data[i]=~data[i];
    }
    bitmap = (uint8_t*) (&data[idx]);
    if(bitmap!=nullptr) display.drawBitmap(x,y,bitmap,img_w,img_h,WHITE);
    dirty_[d] = true;
}



//////  POTS AND BUTTON HANDLING //////////////////////////////////////


void *process_gpio_func(void *x) {
    auto pThis = static_cast<TTuiDeviceImpl_ *>(x);
    pThis->processGPIO();
    return nullptr;
}


void TTuiDeviceImpl_::processGPIO() {
    bool digInState[MAX_DIG_IN] = {false,false,false,false,false,false,false};
    uint16_t adcState[ADC_NUM_CHANNELS]; // adc values
    bool sentDigOutState[MAX_DIG_OUT] = {false,false,false};
    for(unsigned i=0;i<ADC_NUM_CHANNELS;i++) {
        adcState[i] = 0;
    }
    const unsigned numB = numButtons();
    while (keepRunning_) {
        // digital in
        for(unsigned pin=0;pin<MAX_DIG_IN;pin++) {
            auto d= ! TTgpio::digiRead(pin);

            if(d!=digInState[pin]) {
                TTuiEventMsg msg;
                if(pin < numB) {
                    msg.type_ = TTuiEventMsg::N_BUTTON;
                    msg.id_ = pin; 
                } else {
                    msg.type_ = TTuiEventMsg::N_TRIG;
                    msg.id_ = pin-numB; 
                }
                msg.value_ = d;
                eventQueue_.enqueue(msg);
            }
            digInState[pin]=d;
        }

        // digital out 
        for(unsigned pin=0;pin<MAX_DIG_OUT;pin++) {
            if(digOutState[pin]!=sentDigOutState[pin]) {
                auto d=digOutState[pin];
                TTgpio::digiWrite(pin,d);
                sentDigOutState[pin]=d;
            }
        }

        // adc input
        for(unsigned adc=0;adc<ADC_NUM_CHANNELS;adc++) {
            auto prev=adcState[adc];
            TTgpio::readADC(adc,adcState);
            if(adcState[adc]!=prev) {
                TTuiEventMsg msg;
                msg.type_ = TTuiEventMsg::N_POT;
                msg.id_ = adc;
                msg.value_ = adcState[adc];
                eventQueue_.enqueue(msg);
            }
        }

        // sleep
        usleep(gpioPollTime_);
    }
}

void TTuiDeviceImpl_::initGPIO() {
    TTgpio::initGPIO();

    gpioThread_ = std::thread(process_gpio_func, this);
}

void TTuiDeviceImpl_::deinitGPIO() {
    keepRunning_ = false;
    if (gpioThread_.joinable()) gpioThread_.join();
}

////////////////////////    DISPLAY HANDLING ////////////////////////////////////////////



void TTuiDeviceImpl_::initDisplay() {


    if(! display_[0].init(OLED_I2C_RESET,OLED_SH1106_I2C_128x64,(uint8_t) 60 )) {
        fprintf(stderr,"unable to open display 0");
    }
    display_[0].begin();
    displayClear(0);
    display_[0].display();

    if( ! display_[1].init(OLED_I2C_RESET,OLED_SH1106_I2C_128x64, (uint8_t) 61 )) {
        fprintf(stderr,"unable to open display 1");
    }
    display_[1].begin();
    displayClear(1);
    display_[1].display();
  

    dirty_[0]=false; dirty_[1]=false;
}

void TTuiDeviceImpl_::deinitDisplay() {
    for(int d=0;d<2;d++) {
        auto& display=display_[d];
        display.close();
    }
}

}// namespace

