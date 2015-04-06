#define DEBUG   // enable pr_debug() and dev_dbg()

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>     // alloc_crdev_region, unregister_chrdev_region
#include <linux/cdev.h>   // cdev_alloc, cdev_init, cdev_add
#include <linux/err.h>    // IS_ERR
#include <linux/device.h> // class_create, device_create
#include <linux/ioport.h> // request_mem_region, ioremap_nocache
#include <linux/io.h>
#include <asm-generic/uaccess.h>  // ioread32
#include "efm32gg.h"

#define DRIVER_NAME "driver-gamepad"
#define DEVICE_NAME "gamepad"

// Length of the device in bytes
#define DEVICE_LENGTH 1

// necessary function prototypes
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static loff_t gamepad_llseek(struct file *filp, loff_t offp, int whence);


// the first device number
static dev_t dev;

// character device
static struct cdev my_cdev;

// file operations structure
static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .open = gamepad_open,
  .release = gamepad_release,
  .read = gamepad_read,
  .llseek = gamepad_llseek,
};

// class and device structure used when creating device structure
static struct class *my_class;
static struct device *my_device;

// GPIO memory regions
struct resource *gpio_pc_region;
struct resource *gpio_irq_region;

// GPIO memory pointers
void *gpio_pc_ptr;
void *gpio_irq_ptr;

// last error
static int err;


// Read from the device. Currently, this function can only read register GPIO_PC_DIN.
// 
// NOTE: To read the same value repeatedly, the read/write offset needs to be
// reset for each read. This is done automatically when using pread() (see: man
// 2 pread).
//
// Called when a user process calls read()
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  if(*offp < DEVICE_LENGTH){

    // copy the first byte of GPIO_PC_DIN into the user buffer
    uint8_t value = ioread32(gpio_pc_ptr + GPIO_Px_DIN) ^ 0xff;
    copy_to_user((void*)buff, (void*)&value, 1);
    (*offp)++;
    return 1;
  }else{

    // file offset beyond EOF
    return 0;
  }
} 


// Release resources associated with a device.
//
// Called when a user process opens a device file.
static int gamepad_open(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Device opened\n");
  return 0;
} 


// Release the resources associated with a device.
//
// Called when a user process closes the last open instance of a device file.
static int gamepad_release(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Device released\n");

  // TODO
  return 0;
}

// Reposition the read/write offset of the device.
//
// Called when a user process calls lseek() or pread().
static loff_t gamepad_llseek(struct file *filp, loff_t offp, int whence)
{
  // calculate new offset
  loff_t newpos;
  switch(whence)
  {
    case SEEK_SET:
      newpos = offp;
      break;

    case SEEK_CUR:
      newpos = filp->f_pos + offp;
      break;
      
    case SEEK_END:
      newpos = DEVICE_LENGTH + offp;
      break;
    
    default: // invalid whence
      return EINVAL;
  }

  if(newpos < 0) {
    return EINVAL;
  }
  filp->f_pos = newpos;
  return newpos;
}

// GPIO setup
static int setup_gpio(void)
{
  pr_debug("Setting up GPIO ...\n");

  // Allocate memory region for port C registers
  gpio_pc_region = request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_pc_region)){
    pr_err("Error allocating memory region for port C: %ld\n", PTR_ERR(gpio_pc_region));
    return -1; // TODO: Handle error
  }

  // Map memory region into virtual memory space
  gpio_pc_ptr = ioremap_nocache(GPIO_PC_BASE, GPIO_PC_LENGTH);
  if(IS_ERR(gpio_pc_ptr)){
    pr_err("Error mapping memory region for port C: %ld\n", PTR_ERR(gpio_pc_ptr));
    return -1; // TODO: Handle error
  }

  // Set up port C registers
  iowrite32(0x33333333, gpio_pc_ptr + GPIO_Px_MODEL); // Set pins 0-7 to input
  iowrite32(0x000000ff, gpio_pc_ptr + GPIO_Px_DOUT);  // Enable pull-up

  // Allocate memory region for interrupt registers
  gpio_irq_region = request_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_irq_region)){
    pr_err("Error allocating memory region for interrupt registers: %ld\n", PTR_ERR(gpio_irq_region));
    return -1; // TODO: Handle error
  }

  // Map memory region into virtual memory space
  gpio_irq_ptr = ioremap_nocache(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);
  if(IS_ERR(gpio_irq_ptr)){
    pr_err("Error mapping memory region for interrupt registers: %ld\n", PTR_ERR(gpio_irq_ptr));
    return -1; // TODO: Handle error
  }

  // Set up interrupt registers
  iowrite32(0x22222222, gpio_irq_ptr + GPIO_EXTIPSELL); // Set port C as interrupt source
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIFALL);  // Set interrupt on 1->0
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIRISE);  // Set interrupt on 0->1
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_IEN);       // Enable interrupt generation

  // No errors
  pr_debug("OK: No errors setting up GPIO.\n");
  return 0;
}

// GPIO cleanup
static void cleanup_gpio(void)
{
  pr_debug("Releasing GPIO resources ... ");

  // Reset interrupt registers
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIPSELL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIFALL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIRISE);
  iowrite32(0x0, gpio_irq_ptr + GPIO_IEN);

  // Unmap memory region for interrupt registers
  iounmap(gpio_irq_ptr);

  // Deallocate memory region for interrupt registers
  release_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);

  // Reset port C registers
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_Px_MODEL);
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_Px_DOUT);

  // Unmap memory region for port C registers
  iounmap(gpio_pc_ptr);

  // Deallocate memory region for port C registers
  release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);
}

// Initialize the gamepad module and insert it into kernel space
//
// Returns 0 if successfull, otherwise -1
static int __init gamepad_init(void)
{
  pr_debug("Initializing the module:\n");

  err = setup_gpio();
  if(err){
    return -1; // TODO: Handle error
  }

  // Allocate device numbers
  pr_debug("allocating device numbers ...\n");
  err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
  if(err){
    pr_err("Error allocating device numbers: %d\n", err);
    return -1; // TODO: Handle error
  }

  // Set up a character device
  pr_debug("setting up character device ...\n");
  cdev_init(&my_cdev, &my_fops);
  err = cdev_add(&my_cdev, dev, 1);
  if(err){
    pr_err("Error adding char device to the kernel: %d\n", err);
    return -1; // TODO: Handle error
  }

  // Create a device file
  pr_debug("creating device file...\n");
  my_class = class_create(THIS_MODULE, DEVICE_NAME);
  if(IS_ERR(my_class)){
    pr_err("Error creating class structure for device file: %ld\n", PTR_ERR(my_class));
    return -1; // TODO: Handle error
  }
  my_device = device_create(my_class, NULL, dev, NULL, DEVICE_NAME);
  if(IS_ERR(my_device)){
    pr_err("Error creating device structure for device file: %ld\n", PTR_ERR(my_device));
    return -1; // TODO: Handle error
  }

  // Print OK and return
  pr_debug("OK: No errors during module initialization.\n");
	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
  pr_debug("Cleaning up the module:\n");

  // Destroy structures used to create the device file
	pr_debug("Destroying structures used to create device file ...\n");
  device_destroy(my_class, dev);
  class_destroy(my_class);

  // Remove the character device from the kernel
	pr_debug("Removing the character device from the kernel ...\n");
  cdev_del(&my_cdev);

  // Deallocate device numbers
	pr_debug("Deallocating device numbers ...\n");
  unregister_chrdev_region(dev, 1);

  cleanup_gpio();

  pr_debug("OK: No errors during module cleanup.\n");
  return;
}


// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);


// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
