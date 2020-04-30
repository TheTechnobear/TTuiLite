// this is based on implemention from
// https://github.com/mxmxmx/terminal_tedium/blob/master/software/OSC%20client/main.c

#include "TTgpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>


static unsigned SMOOTHING = 1;
static unsigned DEADBAND = 2;



static constexpr unsigned ADC_SPI_CHANNEL = 1;
static constexpr unsigned ADC_SPI_SPEED = 4000000;
static constexpr unsigned RESOLUTION = 4095;
static constexpr unsigned SCALE = 4000;
static constexpr float SCALE_IN = 1.0f / (float)SCALE;


static constexpr unsigned B1 = 23;
static constexpr unsigned B2 = 25;
static constexpr unsigned B3 = 24;
static constexpr unsigned TR1 = 4;
static constexpr unsigned TR2 = 17;
static constexpr unsigned TR3 = 14;
static constexpr unsigned TR4 = 27;

static constexpr unsigned LED = 26;
static constexpr unsigned GATE1 = 16;
static constexpr unsigned GATE2 = 12;

namespace TTgpio {

void smoothing(unsigned v) {
    SMOOTHING = v;
}

void deadband(unsigned v) {
    DEADBAND = v;
}


void initGPIO() {
    wiringPiSetupGpio();
    wiringPiSPISetup(ADC_SPI_CHANNEL, ADC_SPI_SPEED);

    pinMode(B1, INPUT);
    pullUpDnControl(B1, PUD_UP);
    pinMode(B2, INPUT);
    pullUpDnControl(B2, PUD_UP);
    pinMode(B3, INPUT);
    pullUpDnControl(B3, PUD_UP);

    pinMode(TR1, INPUT);
    pullUpDnControl(TR1, PUD_UP);
    pinMode(TR2, INPUT);
    pullUpDnControl(TR2, PUD_UP);
    pinMode(TR3, INPUT);
    pullUpDnControl(TR3, PUD_UP);
    pinMode(TR4, INPUT);
    pullUpDnControl(TR4, PUD_UP);

    pinMode(LED, OUTPUT);
    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);
}


unsigned readADC(unsigned *adc_val, unsigned n) {
    uint8_t spi_data[3];
    unsigned count=0;
    for (int i=0;i<n;i++) {

        uint16_t result, tmp = adc_val[i]; // previous.

        spi_data[0] = 0x06 | (i >> 2) & 0x01;  // single ended
        spi_data[1] = i << 6;
        spi_data[2] = 0x00;

        wiringPiSPIDataRW(ADC_SPI_CHANNEL, spi_data, 3);

        // invert + limit result:
        result = SCALE - (((spi_data[1] & 0x0f) << 8) | spi_data[2]);
        result = result > RESOLUTION ? 0 : result;

        if ( (result - tmp) > DEADBAND || (tmp - result) > DEADBAND ) {
            adc_val[i] = result ;
            count++;
        }
        else {
            adc_val[i]  = tmp;
        }
    }
    return count;
}


static uint8_t digiInPins[MAX_DIG_IN] = {B1, B2, B3, TR1, TR2, TR3, TR4};
static uint8_t digiOutPins[MAX_DIG_OUT] = {LED, GATE1, GATE2};


bool digiRead(unsigned pin) {
    if (pin < MAX_DIG_IN) {
        return digitalRead(digiInPins[pin]);
    }
    return 0;
}

void digiWrite(unsigned pin, bool v) {
    if (pin < MAX_DIG_OUT) {
        digitalWrite(digiOutPins[pin], v);
    }
}

}