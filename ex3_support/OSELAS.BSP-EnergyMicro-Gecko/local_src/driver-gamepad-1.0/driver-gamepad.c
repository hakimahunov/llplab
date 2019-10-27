#include "efm32gg.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/signal.h>
#include <asm/siginfo.h>

static int __init drvr_init(void);
static void __exit drvr_cleanup(void);
static ssize_t drvr_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
irqreturn_t btn_intrpt_hndlr(int irq, void* dev_id, struct pt_regs* regs);
static int drvr_fasync_helper(int fd, struct file *filp, int mode);

dev_t dev_num;
struct cdev driver_cdev;
struct class* cl;
struct fasync_struct* async_queue;

// Registration of driver functions
static struct file_operations driver_fops = {
    .owner = THIS_MODULE,
    .read = drvr_read,
    .fasync = drvr_fasync_helper
};


static int __init drvr_init(void)
{
	printk("LLP_GPad_Driver initializing process...\n");
	
	// IO port allocation (LDD3 Ch.9)
	if(request_mem_region(GPIO_PC_MODEL, 4, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_PC_DIN, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_PC_DOUT, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_EXTIPSELL, 4, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_EXTIFALL, 2, DRIVER_NAME) == NULL ||
		request_mem_region(GPIO_IEN, 2, DRIVER_NAME) == NULL){
		
		printk(KERN_ALERT "IO port allocation failed!\n");
		return -1;
	}
	
	// Mapping into the vritual memory space (LDD3 Ch.8, ch.9)
	ioremap(GPIO_PC_MODEL, 4);
	ioremap(GPIO_PC_DIN, 2);
	ioremap(GPIO_PC_DOUT, 2);
	ioremap(GPIO_EXTIPSELL, 4);
	ioremap(GPIO_EXTIFALL, 2);
	ioremap(GPIO_IEN, 2);
	
	// Device number allocation (LDD3 Ch.3)
	if(alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME) < 0){
		printk(KERN_ALERT "Error in device number allocation!\n");
		return -1;
	}
	
	// Allocation and initialization of cdev structure (LDD3 Ch.3)
	cdev_init(&driver_cdev, &driver_fops);
	driver_cdev.owner = THIS_MODULE;
	if(cdev_add(&driver_cdev, dev_num, 1) < 0){
		printk(KERN_ALERT "Cannot add the device to the system!\n");
		return -1;
	};
	
	// Making driver visible to user space (https://opensourceforu.com/2011/04/character-device-files-creation-operations/)
	if((cl = class_create(THIS_MODULE, DRIVER_NAME)) == NULL){
		printk(KERN_ALERT "Cannot create the struct class!\n");
		return -1;
	};
	if(device_create(cl, NULL, dev_num, NULL, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "Cannot create the device!\n");
		return -1;
	};
	
	// Registration interrupts in kernel (LDD3 Ch.10)
	request_irq(GPIO_IRQ_EVEN, btn_intrpt_hndlr, 0, DRIVER_NAME, &driver_cdev);
	request_irq(GPIO_IRQ_ODD, btn_intrpt_hndlr, 0, DRIVER_NAME, &driver_cdev);
	
	// GPIO initialization (LDD3 Ch.9)
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xFF, GPIO_PC_DOUT);
	// Enable interrupts
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	iowrite32(0xff, GPIO_EXTIFALL);
	iowrite32(0xff, GPIO_IEN);
	
	printk("LLP_GPad_Driver initialization completed.\n");
	return 0;
}

static void __exit drvr_cleanup(void)
{
	 printk("Driver is removed!\n");
}

static ssize_t drvr_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int buttons_state = ioread32(GPIO_PC_DIN);
	// (LDD3 Ch.6)
	copy_to_user(buff, &buttons_state, 4);
	return 1;
}

irqreturn_t btn_intrpt_hndlr(int irq, void* dev_id, struct pt_regs* regs)
{
	iowrite32(ioread32(GPIO_IF), GPIO_IFC);
	// (LDD3 Ch.6)
	if (async_queue) {
		kill_fasync(&async_queue, SIGIO, POLL_IN);
	}
    
    return IRQ_HANDLED;
}

static int drvr_fasync_helper(int fd, struct file *filp, int mode)
{
	//(LDD3 Ch.3, Ch.6)
	
	return fasync_helper(fd, filp, mode, &async_queue);
}

module_init(drvr_init);
module_exit(drvr_cleanup);

MODULE_DESCRIPTION("LLP_GPad_Driver");
MODULE_LICENSE("GPL");

