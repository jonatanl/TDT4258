#define DEBUG   // enable pr_debug(), dev_dbg() and friends

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
#include <linux/interrupt.h>
#include <linux/slab.h>   // kmalloc, kfree



#define DRIVER_NAME "driver-gamepad"
#define DEVICE_NAME "gamepad"

// Length of the device in bytes
#define DEVICE_LENGTH 1

// Platform IRQ numbers
#define GPIO_EVEN_IRQ 17
#define GPIO_ODD_IRQ  18



// necessary function prototypes
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);



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
  .llseek = no_llseek,
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

// a cyclic buffer of button states
static uint8_t *bt_buffer;
static uint32_t head; // next element to write
static uint32_t tail; // next element to read
#define BT_BUFFER_SIZE 1024

// Read from the device. Currently, this function can only read register GPIO_PC_DIN.
// 
// NOTE: To read the same value repeatedly, the read/write offset needs to be
// reset for each read. This is done automatically when using pread() (see: man
// 2 pread).
//
// Called when a user process calls read()
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  if(tail != head){ // data is available

    // copy one byte into user buffer
    copy_to_user((void*)buff, (void*)&bt_buffer[tail], 1);

    // increment read index
    tail = (tail + 1) % BT_BUFFER_SIZE;

    // return number of bytes read
    return 1;
  }else{
    return 0;
  }
} 

// Initialize device resources
//
// Called when a user process opens a device file.
static int gamepad_open(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Device opened\n");

  // initialize button buffer
  bt_buffer = kmalloc(BT_BUFFER_SIZE, GFP_KERNEL);
  head = 0;
  tail = 0;

  // No resources to initialize
  return 0;
} 

// Release device resources
//
// Called when a user process closes the last open instance of a device file.
static int gamepad_release(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Device released\n");

  // destroy button buffer
  kfree(bt_buffer);
  bt_buffer = NULL;
  head = 0;
  tail = 0;

  return 0;
}

// GPIO interrupt handler for testing purposes
static irqreturn_t gpio_handler(int irq, void *dev_id)
{
  // read one element into buffer
  bt_buffer[head] = ioread8(gpio_pc_ptr + GPIO_PC_DIN) ^ 0xff;

  // increment write index
  head = (head + 1) % BT_BUFFER_SIZE;

  // if full, one element is overwritten
  if(head == tail)
    tail = (tail + 1) % BT_BUFFER_SIZE;

  // clear all interrupt flags
  iowrite32(0xff, gpio_irq_ptr + GPIO_IFC);

  return IRQ_HANDLED;
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
  iowrite32(0x33333333, gpio_pc_ptr + GPIO_PC_MODEL); // Set pins 0-7 to input
  iowrite32(0x000000ff, gpio_pc_ptr + GPIO_PC_DOUT);  // Enable pull-up

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

  // Register interrupt handler
  int flags = 0;
  err = request_irq(GPIO_EVEN_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){
    pr_err("Error requesting EVEN interrupts: %d\n", err);
  }
  err = request_irq(GPIO_ODD_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){
    pr_err("Error requesting ODD interrupts: %d\n", err);
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
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_MODEL);
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_DOUT);

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
