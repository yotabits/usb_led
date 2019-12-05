#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"
#include "WS2811.h"


#define LED             PB1
#define RGB_LED         PB2
#define RGB_LED_NUMBER  1

DEFINE_WS2811_FN(WS2811RGB, PORTB, RGB_LED)

#if F_CPU != 16500000L
  #error "Wrong F_CPU"
#endif

PROGMEM const char usbHidReportDescriptor[22] = {   /* USB report descriptor */
  0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
  0x09, 0x01,                    // USAGE (Vendor Usage 1)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x09, 0x00,                    //   USAGE (Undefined)
  0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
  0xc0                           // END_COLLECTION
};

typedef struct leds_status
{
    RGB_t rgb_led[RGB_LED_NUMBER];
    uint8_t led_status;
} leds_status_t;
leds_status_t led_status_g = {{{0}}};

void set_led_to(uchar value)
{
    if (value)
        PORTB |= (1 << LED);
    else
        PORTB &= ~(1 << LED);
}

void flip_led()
{
    PORTB ^= (1  << LED);
}

// bRequest = 1 ---> flip led
// bRequest = 2 ---> set led to value of lsb of received data
//                   we expect exactly 1 byte
// bRequest = 3 ---> TBD (probably rgb) we expect exactly 3 bytes

usbMsgLen_t usbFunctionSetup(uchar setupData[8])
{
    usbRequest_t *rq = (void *)setupData;
    if (rq->bRequest == 1)
        flip_led();
    else if (rq->bRequest == 2)
        return USB_NO_MSG;
    return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
    //set_led_to(data[0]);
    if (len == 1)
    {
        led_status_g.led_status = data[0];
    }
    else if (len == 3)
    {
        led_status_g.rgb_led[0].r = data[0];
        led_status_g.rgb_led[0].g = data[1];
        led_status_g.rgb_led[0].b = data[2];
    }
    update_leds(led_status_g);
    return 1;
}

void setup_led_as_output()
{
    DDRB |= (1 << LED); // set led pin as output
    set_led_to(0); // Turn off led

    DDRB |= (1 << RGB_LED); // set rgb led pin as output
}


void begin()
{
  cli();

  usbInit();
  usbDeviceDisconnect();
  uchar   i;
  i = 0;
  while(--i)
  {             /* fake USB disconnect for > 250 ms */
    _delay_ms(10);
  }
  usbDeviceConnect();
  sei();
}
void update_leds(leds_status_t status)
{
    WS2811RGB(status.rgb_led, ARRAYLEN(status.rgb_led));
    set_led_to(status.led_status);
}
int main()
{
    //RGB_t rgb[1];
    setup_led_as_output();
    begin();
    while(1)
    {
      usbPoll();
    }
  return 0;
}
