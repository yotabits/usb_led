#include <iostream>
#include <libusb-1.0/libusb.h>
#include <usb.h>
#include <cmath>
#include <stdio.h>
#include <thread>
#include "cpu_color.h"
#define VID 0x16c0
#define PID 0x05df


void send_data(uint8_t request, unsigned char *to_send,
               libusb_device_handle* dev_handle, uint16_t data_length)
{
    uint8_t bmRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT;
    uint8_t bRequest = request;
    uint16_t wValue = 0xDEAD; // Arbitrary vendor requests
    uint16_t wIndex = 0xBEEF; // Arbitrary vendor requests
    unsigned int timeout = 0;
    int sent = libusb_control_transfer(dev_handle, bmRequestType, bRequest,
                            wValue, wIndex, to_send, data_length, timeout);
    std::cout << sent << " bytes sent" << std::endl;
}

void flip_led(libusb_device_handle* dev_handle)
{
    send_data(1, NULL, dev_handle, 0);
}

void set_led_to(libusb_device_handle* dev_handle, unsigned char value)
{
    send_data(2, &value, dev_handle, 1);
}

void set_rgb_led_to(libusb_device_handle* dev_handle, unsigned char *value)
{
    send_data(2, value, dev_handle, 3);
}

void set_led_to_hue(libusb_device_handle* dev_handle, uint16_t hue)
{
    uint8_t to_show[3];
    HSVtoRGB(hue, 1, 1, to_show);
    set_rgb_led_to(dev_handle, to_show);
}

void show_variation(libusb_device_handle* dev_handle,
                    uint16_t start_hue, uint16_t end_hue, uint32_t time_micro_s)
{
    int8_t moving = 1;
    uint32_t sleep_time = 0;

    if (start_hue > end_hue)
        moving = -1;

    if (start_hue != end_hue)
        sleep_time = time_micro_s / (abs(start_hue - end_hue));

    // If start == end we at least set the led
    // to the correct value.
    set_led_to_hue(dev_handle, start_hue);
    while (start_hue != end_hue)
    {
        usleep(sleep_time);
        start_hue += moving;
        std::cout << "HUE " << start_hue << std::endl;
        set_led_to_hue(dev_handle, start_hue);
    }
}

int main()
{
    libusb_device **devs;
    libusb_context *ctx = NULL;
    ssize_t nb_devices; //holding number of devices in list
    int r = libusb_init(&ctx); //initialize a library session
    if (r < 0)
    {
        std::cout << "Init Error " << r << std::endl;
        return 1;
    }

    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    nb_devices = libusb_get_device_list(ctx, &devs); //get the list of devices
    if (nb_devices < 0)
        std::cout << "Get Device Error" << std::endl; //there was an error

    std::cout << nb_devices << " Devices found." << std::endl; //print total number of usb devices

    libusb_device_handle* dev_handle = libusb_open_device_with_vid_pid(ctx, VID, PID);
    if (!dev_handle)
        std::cout << "Failed to open device" << std::endl;

    libusb_free_device_list(devs, 1);

    if (libusb_kernel_driver_active(dev_handle, 0))
        libusb_detach_kernel_driver(dev_handle, 0);

    r = libusb_set_configuration(dev_handle, 1);
    if (r != 0)
        std::cout << "Failed to set configuration with error: " << r << std::endl;

    r = libusb_claim_interface(dev_handle, 0);
    if (r != 0)
        std::cout << "Failed to claim interface with error: " << r << std::endl;

    set_led_to(dev_handle, 1);
    uint16_t last_hue = 230;
    show_variation(dev_handle, last_hue, 0, 1000000);
    uint16_t target_hue;
    std::thread led_thread;
    get_cpu_color(target_hue);
    while (true)
    {
        led_thread = std::thread(show_variation, dev_handle, last_hue, target_hue, 1000000);
        last_hue = target_hue;
        get_cpu_color(target_hue);
        std::cout << target_hue << std::endl;
        led_thread.join();
    }
}