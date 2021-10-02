#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>

//Max minor devices
#define MAX_DEV 2;


// Initialize file_operations
static const struct file_operations huydev_fops = {
    .owner      = THIS_MODULE,
    .open       = mychardev_open,
    .release    = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read       = mychardev_read,
    .write       = mychardev_write
};

// Device data holder. May be extend to hold additional data
struct huy_device_data {
    struct cdev cdev;
};

//Global variable to store device's Major number
static int dev_major = 0;

// sysfs class structure
static struct class *huydev_class = NULL;

static struct huy_device_data huydev_data[MAX_DEV];


static int __init hello_start(void) {
    printk(KERN_INFO "Hellom I'm here to help\n");
    int err, i;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "huy");

    dev_major = MAJOR(dev);

    if (err) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return err;
    }

    huydev_class = class_create(THIS_MODULE, "huy");

    for (i = 0, i < MAX_DEV, i++) {
        //Init new device
        cdev_init(&huydev_data[i].cdev, &huydev_fops);
        huydev_data[i].cdev.owner = THIS_MODULE;

        // Add device to the system. 'i' - minor device's number
        cdev_add(&huydev_data[i].cdev, MKDEV(dev_major, i ), 1);

        //create device node /dev/huyx where x is minor number of the device
        device_create(huydev_class, NULL, MKDEV(devmajor, i), NULL, "huy%d", i);
    }

    return 0;
}

static void __exit hello_end(void) {
    printk(KERN_INFO "Goodbye, pidor\n");
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(mychardev_class, MKDEV(dev_major, i));
    }

    class_unregister(mychardev_class);
    class_destroy(mychardev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

module_init(hello_start);
module_exit(hello_end);

MODULE_LICENSE("GPL v2");
