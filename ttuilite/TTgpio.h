

#include <stdint.h>

static constexpr unsigned ADC_NUM_CHANNELS=6;
static constexpr unsigned MAX_DIG_IN = 7;
static constexpr unsigned MAX_DIG_OUT = 3;

namespace TTgpio { 
    void initGPIO();
    void smoothing(unsigned);
    void deadband(unsigned);

    unsigned readADC(unsigned *adcarray,unsigned n);
    bool digiRead(unsigned);
    void digiWrite(unsigned,bool);
}
