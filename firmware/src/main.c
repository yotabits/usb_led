#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"

#define LED     PB1

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
    set_led_to(data[0]);
    return 1;
}

void setup_led_as_output()
{
    DDRB |= (1 << LED); // set led pin as output
    set_led_to(0); // Turn on led
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

int main()
{
    setup_led_as_output();
    begin();
  while(1)
  {
      usbPoll();
  }
  return 0;
}
