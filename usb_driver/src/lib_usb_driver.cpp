#include <iostream>
#include <libusb-1.0/libusb.h>
#include <usb.h>
#define VID 0x16c0
#define PID 0x05df
using namespace std;


void send_data(uint8_t request, unsigned char *to_send,
               libusb_device_handle* dev_handle, uint16_t data_length)
{
    uint8_t bmRequestType = USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT;
    uint8_t bRequest = request;
    uint16_t wValue = 0xDEAD; // Arbitrary vendor requests
    uint16_t wIndex = 0xBEEF; // Arbitrary vendor requests
    unsigned int timeout = 300;
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
        std::cout << "failed to open device" << std::endl;

    libusb_free_device_list(devs, 1);

    if (libusb_kernel_driver_active(dev_handle, 0))
        libusb_detach_kernel_driver(dev_handle, 0);

    r = libusb_set_configuration(dev_handle, 1);
    if (r != 0)
        std::cout << "Failed to set configuration with error: " << r << std::endl;

    r = libusb_claim_interface(dev_handle, 0);
    if (r != 0)
        std::cout << "Failed to claim interface with error: " << r << std::endl;

    flip_led(dev_handle);
    set_led_to(dev_handle, 1);
    uint8_t rgb[] = {1, 1, 1};
    set_rgb_led_to(dev_handle, rgb);

    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx); //close the session
    return 0;
}