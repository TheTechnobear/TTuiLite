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


#ifndef SCREEN_X
#define SCREEN_X 128
#endif

#ifndef SCREEN_Y
#define SCREEN_Y 64
#endif

namespace TTuiLite {

struct TTuiEventMsg {
    enum {
        N_BUTTON,
        N_ENCODER
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
    unsigned numEncoders();
    unsigned numButtons();


    void displayPaint();

    void displayClear(unsigned d);
    // text functions 
    void displayText(unsigned d, unsigned clr, unsigned line, unsigned col, const std::string &str);
    void clearText(unsigned d, unsigned clr, unsigned line);

    // draw functions
    void clearRect(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h);
    void drawText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str);
    void drawBitmap(unsigned d, unsigned x, unsigned y, const char *filename);

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

unsigned TTuiDevice::numEncoders() {
    return impl_->numEncoders();
}

unsigned TTuiDevice::numButtons() {
    return impl_->numButtons();
}

void TTuiDevice::displayPaint() {
    impl_->displayPaint();
}

void TTuiDevice::displayClear(unsigned d) {
    impl_->displayClear(d);
}

void TTuiDevice::displayText(unsigned d, unsigned clr, unsigned line, unsigned col, const std::string &str) {
    impl_->displayText(d,clr, line, col, str);
}


void TTuiDevice::clearText(unsigned d, unsigned clr, unsigned line) {
    impl_->clearText(d, clr, line);
}

void TTuiDevice::drawBitmap(unsigned d, unsigned x, unsigned y, const char *filename) {
    impl_->drawBitmap(d, x, y, filename);
}

void TTuiDevice::clearRect(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    impl_->clearRect(d,clr, x, y, w, h);
}

void TTuiDevice::drawText(unsigned d, unsigned clr, unsigned x, unsigned y, const std::string &str) {
    impl_->drawText(d, clr, x, y, str);
}

// fwd decl for helper functions
extern int opengpio(const char *pathname, int flags);


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
                case TTuiEventMsg::N_ENCODER : {
                    cb->onEncoder(msg.id_, msg.value_);
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


unsigned TTuiDeviceImpl_::numEncoders() {
    return 0;
}

unsigned TTuiDeviceImpl_::numButtons() {
    return 3;
}

//// display functions

void TTuiDeviceImpl_::displayPaint() {
    for(int d=0;d<2;d++) {
        auto& display=display_[d];
        if(dirty_[d]) display.display();
        dirty_[d] = false;
    }
}

void TTuiDeviceImpl_::clearRect(unsigned d, unsigned clr, unsigned x, unsigned y, unsigned w, unsigned h) {
    if(d>MAX_DISPLAYS) return;
    uint16_t colour =  (clr > 0 ? WHITE : BLACK);
    auto& display=display_[d];
    display.fillRect(x,y,w,h,colour);
    dirty_[d] = true;
}


void TTuiDeviceImpl_::drawText(unsigned d,unsigned clr, unsigned x, unsigned y, const std::string &str) {
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


void TTuiDeviceImpl_::displayClear(unsigned d) {
    if(d>MAX_DISPLAYS) return;
    auto& display=display_[d];
    display.clearDisplay();
    dirty_[d] = true;
}

void TTuiDeviceImpl_::displayText(unsigned d,unsigned clr, unsigned line, unsigned col, const std::string &str) {
    unsigned x = col * 4;
    unsigned y = line * 10 + 10;
    drawText(d,clr,x,y,str);
}

void TTuiDeviceImpl_::clearText(unsigned d,unsigned clr, unsigned line) {
    unsigned x = 0;
    unsigned y = (line * 10 + 10) - 1;
    clearRect(d, clr, x, y, SCREEN_X, 10);
    dirty_[d] = true;
}


void TTuiDeviceImpl_::drawBitmap(unsigned d,unsigned x, unsigned y, const char *filename) {
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



//////  ENCODER AND KEY HANDLING //////////////////////////////////////



void *process_gpio_func(void *x) {
    auto pThis = static_cast<TTuiDeviceImpl_ *>(x);
    pThis->processGPIO();
    return nullptr;
}


void TTuiDeviceImpl_::processGPIO() {

    struct input_event event;

    while (keepRunning_) {

        struct pollfd fds[1];

        //FIXME only works if order of fd doesnt change
        // i.e key/enc 1 to 3 must open
        unsigned fdcount = 0;
        if (keyFd_ > 0) {
            fds[fdcount].fd = keyFd_;
            fds[fdcount].events = POLLIN;
            fdcount++;
        }

        auto result = poll(fds, fdcount, 5000);

        if (result > 0) {
            if (fds[0].revents & POLLIN) {
                auto rd = read(keyFd_, &event, sizeof(struct input_event));
                if (rd < (int) sizeof(struct input_event)) {
                    fprintf(stderr, "ERROR (key) read error\n");
                } else {
                    if (event.type) { // make sure it's not EV_SYN == 0
//                        fprintf(stderr, "button %d = %d\n", event.code, event.value);
                        TTuiEventMsg msg;
                        msg.type_ = TTuiEventMsg::N_BUTTON;
                        msg.id_ = event.code - 1; // make zerp based
                        msg.value_ = event.value;
                        eventQueue_.enqueue(msg);
                    }
                }
            }
        }
    }
}


void TTuiDeviceImpl_::initGPIO() {
    //TODO implment GPIO, consider buttons!

    // keyFd_ = opengpio("/dev/input/by-path/platform-keys-event", O_RDONLY);
    // gpioThread_ = std::thread(process_gpio_func, this);
}

void TTuiDeviceImpl_::deinitGPIO() {
    keepRunning_ = false;
    // if (gpioThread_.joinable()) gpioThread_.join();
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


///////// helper functions

int opengpio(const char *pathname, int flags) {
    int fd;
    int open_attempts = 0, ioctl_attempts = 0;
    while (open_attempts < 200) {
        fd = open(pathname, flags);
        if (fd > 0) {
            if (ioctl(fd, EVIOCGRAB, 1) == 0) {
                ioctl(fd, EVIOCGRAB, (void *) 0);
                goto done;
            }
            ioctl_attempts++;
            close(fd);
        }
        open_attempts++;
        usleep(50000); // 50ms sleep * 200 = 10s fail after 10s
    };
    done:
    if (open_attempts > 0) {
        fprintf(stderr, "WARN opengpio GPIO '%s' required %d open attempts & %d ioctl attempts\n", pathname, open_attempts, ioctl_attempts);
    }
    return fd;
}

}// namespace

