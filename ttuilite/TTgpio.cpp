// this is based on terminal tedium externals implmentation
// https://github.com/mxmxmx/terminal_tedium/blob/master/software/externals

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

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>




namespace TTgpio {

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



struct spi_data {
    unsigned char mode;
    unsigned char bitsPerWord;
    unsigned int speed;
    unsigned int smooth;
    unsigned int smooth_shift;
    unsigned int deadband;
    int spifd;
    int _version;
    int a2d[ADC_NUM_CHANNELS];

    spi_data() {
        mode = SPI_MODE_0;
        bitsPerWord = 8;
        speed = 4000000;
        spifd = -1;
        _version = 0;
        smooth = 1;
        smooth_shift = 0;
        deadband = 0;
    }


} *spi_;

int adc_close(spi_data *spi) {
    int statusVal = -1;
    if (spi->spifd == -1) {
        fprintf(stderr, "terminal_tedium_adc: device not open");
        return (-1);
    }

    statusVal = close(spi->spifd);
    if (statusVal < 0) {
        fprintf(stderr, "terminal_tedium_adc: could not close SPI device");
        exit(1);
    }
    spi->spifd = -1;
    return (statusVal);
}


void adc_open(spi_data* spi, unsigned version = 0) {

    int statusVal = 0;

    if (version == 0) spi->_version = 0x0; // = wm8731 version
    else spi->_version = 0x1; // = pcm5102a version

    spi->spifd =  open("/dev/spidev0.1", O_RDWR);

    if (spi->spifd < 0) {
        statusVal = -1;
        fprintf(stderr, "could not open SPI device");
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_WR_MODE, &(spi->mode));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPIMode (WR)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_RD_MODE, &(spi->mode));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPIMode (RD)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_WR_BITS_PER_WORD, &(spi->bitsPerWord));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPI bitsPerWord (WR)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_RD_BITS_PER_WORD, &(spi->bitsPerWord));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPI bitsPerWord(RD)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(spi->speed));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPI speed (WR)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

    statusVal = ioctl (spi->spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(spi->speed));
    if (statusVal < 0) {
        fprintf(stderr, "Could not set SPI speed (RD)...ioctl fail");
        adc_close(spi);
        goto spi_output;
    }

spi_output:
    if (!statusVal) statusVal = 1;
    else statusVal = 0;
}




void initGPIO() {
    wiringPiSetupGpio();
    // wiringPiSPISetup(ADC_SPI_CHANNEL, ADC_SPI_SPEED);
    spi_ = new spi_data();
    adc_open(spi_, 0);

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


void smoothing(unsigned s) {
    unsigned int _shift, _smooth = s;

    if (_smooth < 2) {
        _smooth = 1;
        _shift  = 0;
    }
    else if (_smooth < 4) {
        _smooth = 2;
        _shift  = 1;
    }
    else if (_smooth < 8) {
        _smooth = 4;
        _shift  = 2;
    }
    else if (_smooth < 16) {
        _smooth = 8;
        _shift  = 3;
    }
    else {
        _smooth = 16;
        _shift  = 4;
    }

    spi_->smooth = _smooth;
    spi_->smooth_shift = _shift;
}

void deadband(unsigned d) {
    int _deadband = (int)d;

    if (_deadband < 0)
        _deadband = 0;
    else if (_deadband > 5)
        _deadband = 5;

    spi_->deadband = _deadband;
}


int adc_write_read(spi_data *spi, unsigned char *data, int length) {
    struct spi_ioc_transfer spid[length];
    int i = 0;
    int retVal = -1;
    // one spi transfer for each byte
    for (i = 0 ; i < length ; i++) {
        memset (&spid[i], 0x0, sizeof (spid[i]));
        spid[i].tx_buf        = (unsigned long)(data + i); // transmit from "data"
        spid[i].rx_buf        = (unsigned long)(data + i); // receive into "data"
        spid[i].len           = sizeof(*(data + i));
        spid[i].speed_hz      = spi->speed;
        spid[i].bits_per_word = spi->bitsPerWord;
    }

    retVal = ioctl(spi->spifd, SPI_IOC_MESSAGE(length), &spid);

    if (retVal < 0) {
        fprintf(stderr, "problem transmitting spi data..ioctl");
    }
    return retVal;
}

void readADC(int *a2dVal, unsigned n) {
    auto spi=spi_;

    if (spi->spifd == -1) {
        fprintf(stderr, "device not open %d", spi->spifd);
        return;
    }

    int a2dChannel = 0;
    unsigned char data[3];
    int numChannels = n;
    int SCALE = 4001; // make nice zeros ...

    unsigned int SMOOTH = spi->smooth;
    unsigned int SMOOTH_SHIFT = spi->smooth_shift;
    int DEADBAND = spi->deadband;

    for (unsigned int i = 0; i < SMOOTH; i++) {
        for (a2dChannel = 0; a2dChannel < numChannels; a2dChannel++) {
            data[0]  =  0x06 | ((a2dChannel >> 2) & 0x01);
            data[1]  =  a2dChannel << 6;
            data[2]  =  0x00;
            adc_write_read(spi, data, 3);
            a2dVal[a2dChannel] += (((data[1] & 0x0f) << 0x08) | data[2]);
        }
    }

    for (a2dChannel = 0; a2dChannel < numChannels; a2dChannel++) {
        if (DEADBAND) {
            int tmp  = SCALE - (a2dVal[a2dChannel] >> SMOOTH_SHIFT);
            int tmp2 = spi->a2d[a2dChannel];

            if ((tmp2 - tmp) > DEADBAND || (tmp - tmp2) > DEADBAND)  {
                a2dVal[a2dChannel] = tmp < 0 ? 0 : tmp;
            }
            else {
                a2dVal[a2dChannel] = tmp2;
            }

            spi->a2d[a2dChannel] = a2dVal[a2dChannel];
        }
        else {
            int tmp = SCALE - (a2dVal[a2dChannel] >> SMOOTH_SHIFT);
            a2dVal[a2dChannel] = tmp < 0 ? 0 : tmp;
        }
    }
}

// digital IO

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

} //namespace