

#include <stdint.h>

static constexpr unsigned ADC_NUM_CHANNELS=6;
static constexpr unsigned MAX_DIG_IN = 7;
static constexpr unsigned MAX_DIG_OUT = 3;

namespace TTgpio {
  uint16_t readADC(int _channel, uint16_t *adc_val);
  void initGPIO();

  bool digiRead(unsigned);
  void digiWrite(unsigned,bool);
}
