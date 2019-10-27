#include "efm32gg.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>

static int __init drvr_init(void);
static void __exit drvr_cleanup(void);
static ssize_t driver_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);

dev_t dev_num;
struct cdev driver_cdev;
struct class* cl;

// Registration of driver functions
static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .read = driver_read
};


static int __init drvr_init(void)
{
	printk("LLP_GPad_Driver initializing process...\n");
	
	// IO port allocation
	if(request_mem_region(GPIO_PC_MODEL, 4, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_PC_DIN, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_PC_DOUT, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_EXTIPSELL, 4, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_EXTIFALL, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_IEN, 2, DRIVER_NAME) == NULL){
		
		printk(KERN_ALERT "IO port allocation failed!\n");
		return -1;
	}
	
	// Mapping into the vritual memory space
	ioremap(GPIO_PC_MODEL, 4);
	ioremap(GPIO_PC_DIN, 2);
	ioremap(GPIO_PC_DOUT, 2);
	ioremap(GPIO_EXTIPSELL, 4);
	ioremap(GPIO_EXTIFALL, 2);
	ioremap(GPIO_IEN, 2);
	
	// Device number allocation
	if(alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME) < 0){
		printk(KERN_ALERT "Error in device number allocation!\n");
		return -1;
	}
	
	// Allocation and inicialization of cdev structure
	cdev_init(&driver_cdev, &driver_fops);
	driver_cdev.owner = THIS_MODULE;
	cdev_add(&driver_cdev, dev_num, 1);
	
	// Making driver visible to user space
	cl = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(cl, NULL, dev_num, NULL, DRIVER_NAME);
	
	printk("LLP_GPad_Driver inicialization completed.\n");
	return 0;
}

static void __exit drvr_cleanup(void)
{
	 printk("Driver is removed!\n");
}

static ssize_t driver_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int buttons_state = ioread32(GPIO_PC_DIN);
	copy_to_user(buff, &buttons_state, 4);
	return 1;
}

module_init(drvr_init);
module_exit(drvr_cleanup);

MODULE_DESCRIPTION("LLP_GPad_Driver");
MODULE_LICENSE("GPL");

