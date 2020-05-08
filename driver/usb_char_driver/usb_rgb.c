#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/usb.h>

#define VENDOR_ID 0x16c0
#define PRODUCT_ID 0x05dc
#define LED_MINOR_BASE 0

static struct usb_driver rgb_led_driver;

struct usb_led {
	struct usb_device *udev;
	/*
	 * rgb is the buffer we will end up sending to deice,
	 * Byte 0 --> led number
	 * Byte 1 --> R value
	 * Byte 2 --> G value
	 * Byte 3 --> B value
	 */
	uint8_t *rgb;
};


const static struct usb_device_id device_table[] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ }
};

static int led_open(struct inode *inode, struct file *file)
{
	struct usb_led *dev;
	struct usb_interface *interface;

	interface = usb_find_interface(&rgb_led_driver, iminor(inode));
	dev = usb_get_intfdata(interface);
	file->private_data = dev;
	return 0;
}

static ssize_t led_write(struct file *file, const char __user *user_buffer,
		size_t count, loff_t *ppos)
{
	char *data_in;
	long *user_rgb;
	int return_value;
	struct usb_led *dev;
	struct usb_device *udev;
	uint8_t to_send_size;
	int bytes_sent;

	to_send_size = 4 * sizeof(char);
	return_value = 0;
	data_in = kmalloc_array(count, sizeof(char), GFP_KERNEL);
	if (!data_in)
		return -ENOMEM;

	user_rgb = kmalloc(sizeof(long), GFP_KERNEL);
	if (!user_rgb) {
		kfree(data_in);
		return -ENOMEM;
	}
	dev = file->private_data;
	udev = dev->udev;

	if (!udev) {
		return_value = -ENODEV;
		goto exit;
	}

	if (copy_from_user(data_in, user_buffer, count) || count > 26) {
		return_value = -EFAULT;
		goto exit;
	}
	data_in[count - 1] = '\0';

	kstrtol(data_in, 16, user_rgb);
	dev->rgb[0] = 0xff & *user_rgb;
	dev->rgb[1] = 0xff & (*user_rgb >> 8);
	dev->rgb[2] = 0xff & (*user_rgb >> 16);
	dev->rgb[3] = 0xff & (*user_rgb >> 24);

	/* to replace with dev_dbg */
	printk(KERN_INFO "led_number %d\n", dev->rgb[0]);
	printk(KERN_INFO "r %d\n", dev->rgb[1]);
	printk(KERN_INFO "g %d\n", dev->rgb[2]);
	printk(KERN_INFO "b %d\n", dev->rgb[3]);

	/* wValue & wIndex are arbitrary */
	bytes_sent = usb_control_msg(udev, usb_sndctrlpipe(udev, 0), 2,
			USB_TYPE_VENDOR | USB_RECIP_DEVICE, 0xDEAD,
			0xBEEF, dev->rgb, to_send_size, 0);

	if (bytes_sent == to_send_size)
		return_value = count;
	else
		return_value = -EINVAL; /* is that correct ? */

	printk(KERN_INFO "Control message return %d\n", bytes_sent);

	/* to replace with dev_dbg */
	printk(KERN_INFO "int value %ld\n", *user_rgb);
	printk(KERN_INFO "data in %s\n", data_in);
	printk(KERN_INFO "Count %ld\n", count);

	return_value = count;
	goto exit;

exit:
	kfree(data_in);
	kfree(user_rgb);
	return return_value;
}

static struct file_operations led_fops = {
	.owner   =  THIS_MODULE,  /* Owner */
	.write   =  led_write,   /* Write method */
	.open    =  led_open,    /* Open method */
};

static struct usb_class_driver led_class = {
	.name       = "rgb_led",
	.fops       = &led_fops,      /* Connect with /dev/rgb_led%d */
	.minor_base = LED_MINOR_BASE, /* Minor number start */
};

static int rgb_led_probe(struct usb_interface *interface,
		const struct usb_device_id *id)
{
	struct usb_device *udev;
	struct usb_led *led_dev;

	udev = interface_to_usbdev(interface);
	if (!udev)
		return -ENODEV;

	led_dev = kmalloc(sizeof(struct usb_led), GFP_KERNEL);
	if (!led_dev)
		return -ENOMEM;
	led_dev->rgb = kmalloc(sizeof(uint8_t) * 4, GFP_KERNEL);
	if (!led_dev->rgb)
		return -ENOMEM;

	led_dev->udev = usb_get_dev(udev);
	usb_set_intfdata(interface, led_dev);
	usb_register_dev(interface, &led_class);
	dev_info(&interface->dev, "USB RGB led connected\n");
	return 0;
}

static void disconnect(struct usb_interface *interface)
{
	struct usb_led *dev;

	dev = usb_get_intfdata(interface); /* Get private data */
	usb_put_dev(dev->udev); /* release device structure */
	kfree(dev->rgb);
	kfree(dev);
	usb_deregister_dev(interface, &led_class);
	dev_info(&interface->dev, "USB RGB led disconnected\n");
}

static struct usb_driver rgb_led_driver = {
	.name       =  "rgb_led",           /* Unique name */
	.probe      =  rgb_led_probe,
	.disconnect =  disconnect,
	.id_table   =  device_table,
};

static int __init rgb_led_init(void)
{
	/* Register with the USB core */
	int registration_result;

	registration_result = usb_register(&rgb_led_driver);
	return registration_result;
}

static void __exit rgb_led_exit(void)
{
	usb_deregister(&rgb_led_driver);
	printk(KERN_INFO "usb_rgb_module exited\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Kostas <tkostas75@gmail.com>");
MODULE_DESCRIPTION("Write rgb values to your attiny85 driven usb led.");
MODULE_DEVICE_TABLE(usb, device_table);

module_init(rgb_led_init);
module_exit(rgb_led_exit);
