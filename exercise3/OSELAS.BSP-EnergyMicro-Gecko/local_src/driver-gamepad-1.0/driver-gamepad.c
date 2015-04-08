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
#include <linux/spinlock.h>



#define DRIVER_NAME "driver-gamepad"
#define DEVICE_NAME "gamepad"
#define CLASS_NAME  "gamepad"

// Length of the device in bytes
#define DEVICE_LENGTH 1

// Platform IRQ numbers
#define GPIO_EVEN_IRQ 17
#define GPIO_ODD_IRQ  18



// necessary function prototypes
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static int setup_gpio_input(void);
static int setup_gpio_interrupts(void);
static void cleanup_gpio_input(void);
static void cleanup_gpio_interrupts(void);



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
static int err = 0;

static struct button_buffer
{
  uint8_t *data;
  uint32_t size;
  uint32_t write_count;
  struct semaphore sem;
};
#define BT_BUFFER_SIZE 1024

static struct work_struct my_workqueue;

static void buffer_do_tasklet(unsigned long data)
{
  // retrieve next value from data pointer
  uint8_t value = *(uint8_t*)data;
  uint32_t write_index;

  // write value to buffer and increase write count atomically
  down(my_gamepad.buf.sem);
  write_index = my_gamepad.buf.write_count % my_gamepad.buf.size;
  my_gamepad.buf.data[write_index] = value;
  my_gamepad.buf.write_count++;
  up(my_gamepad.buf.sem);
}

// device structure containing global device data
static struct gamepad_device
{
  dev_t dev;     // first device number
  struct cdev cdev;     // character device
  struct semaphore sem;
  int open_count;       // open instances of this device
  struct button_buffer buf;
} my_gamepad;

// file operations structure
static struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .open = gamepad_open,
  .release = gamepad_release,
  .read = gamepad_read,
  .llseek = no_llseek,
};



// initialize device resources on open()
static int gamepad_open(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Opening device:\n");

  // initialize device on first open
  down(&my_gamepad.sem);
  if(my_gamepad.open_count == 0){
    dev_dbg(my_device, "No open instances. Initialize device data:\n");

    // initialize input buffer
    my_gamepad.buf.data = (uint8_t*) kmalloc(BT_BUFFER_SIZE, GFP_KERNEL);
    if(IS_ERR(my_gamepad.buf.data)){ return -1; } // TODO: Handle error
    my_gamepad.buf.size = BT_BUFFER_SIZE;
    my_gamepad.buf.write_count = 0;
    spin_lock_init(&my_gamepad.buf.lock);
    dev_dbg(my_device, "OK: Initialized input buffer\n");

    // setup GPIO to receive interrupts
    err = setup_gpio_input();
    if(err){ return -1; } // TODO: Handle error
    err = setup_gpio_interrupts();
    if(err){ return -1; } // TODO: Handle error
  }
  my_gamepad.open_count++;
  up(&my_gamepad.sem);

  // initialize private device data
  filp->private_data = kmalloc(sizeof(uint8_t), GFP_KERNEL);
  if(IS_ERR(filp->private_data)){ return -1; } // TODO: Handle error
  *((uint8_t*)filp->private_data) = my_gamepad.buf.write_count;
  dev_dbg(my_device, "OK: Initialized private device data\n");

  dev_dbg(my_device, "DONE: No errors opening device\n");
  return 0;
} 

// Release device resources
//
// Called when a user process closes the last open instance of a device file.
static int gamepad_release(struct inode *inode, struct file *filp)
{
  dev_dbg(my_device, "Releasing device:\n");

  // destroy private device data
  kfree(filp->private_data);
  dev_dbg(my_device, "OK: Destroyed private device data\n");

  down(&my_gamepad.sem);
  my_gamepad.open_count--;

  // shut down device on last close
  if(my_gamepad.open_count == 0){
    dev_dbg(my_device, "No other open instances. Shutting down device:\n");

    // release GPIO interrupt resources
    cleanup_gpio_interrupts();

    // release GPIO input resources
    cleanup_gpio_input();

    // destroy input buffer
    kfree(my_gamepad.buf.data);
    my_gamepad.buf.size = 0;
    my_gamepad.buf.write_count = 0;
    dev_dbg(my_device, "OK: Destroyed input buffer\n");

    dev_dbg(my_device, "DONE: No errors shutting down device\n");
  }
  up(&my_gamepad.sem);

  dev_dbg(my_device, "DONE: No errors releasing device\n");
  return 0;
}

// copy data from input buffer to user space
//
// TODO: support blocking IO
static int gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{ 
  uint32_t next_read;   // next value to read from the input buffer
  uint32_t read_index;  // index of value
  uint32_t next_write;  // next value to write to the input buffer
  uint32_t write_index; // index of value
  uint32_t contiguous_values; // contigous values
  uint32_t copy_count;        // values to copy

  down(&my_gamepad.buf.sem);
  next_read  = *(uint32_t*)filp->private_data;
  next_write = my_gamepad.buf.write_count; 

  // skip overwritten data
  if(next_read <= next_write - my_gamepad.buf.size){
    next_read = next_write - size + 1;
  }
  read_index  = next_read  % my_gamepad.buf.size;
  write_index = next_write % my_gamepad.buf.size;

  // number of contiguous unread values
  contiguous_values = (read_index < write_index)
      ? (write_index - read_index)
      : (my_gamepad.buf.size - read_index);
  
  // number of values to copy into user buffer
  copy_count = (contiguous_values < count)
      ? contiguous_values
      : count;
  
  // copy values into user buffer
  copy_to_user(copy_count, (uint8_t*)my_gamepad.buf.data, copy_count * sizeof(uint8_t));

  // increment read counter
  *((uint32_t*)filp->private_data) += copy_count;

  up(&my_gamepad.buf.sem);

  return copy_count;
} 

// interrupt handler
static irqreturn_t gpio_handler(int irq, void *dev_id)
{
  unsigned long flags;
  uint32_t write_index;
  dev_dbg(my_device, "INTERRUPT (irq = %d)\n", irq);

  // insert data and update write count atomically
  spin_lock_irqsave(&my_gamepad.buf.lock, flags);
  write_index = my_gamepad.buf.write_count % my_gamepad.buf.size;
  my_gamepad.buf.data[write_index] = ioread32(gpio_pc_ptr + GPIO_PC_DIN);
  my_gamepad.buf.write_count++;
  spin_unlock_irqrestore(&my_gamepad.buf.lock, flags);

  // but this approach is poor. Competing with read calls for the spin lock can stall the system for a long time.

  // clear all interrupts
  iowrite32(0xff, gpio_irq_ptr + GPIO_IFC);
  return IRQ_HANDLED;
}

// initialize the gamepad module and insert it into kernel space
static int __init gamepad_init(void)
{
  pr_debug("Initializing the module:\n");

  // allocate device numbers
  err = alloc_chrdev_region(&my_gamepad.dev, 0, 1, DEVICE_NAME);
  if(err){ return -1; } // TODO: Handle error
  pr_debug("OK: Allocated device numbers\n");

  // initialize cdev structure
  cdev_init(&my_gamepad.cdev, &my_fops);

  // initialize remaining device fields
  //
  // NOTE: the input buffer is initialized on first open()
  sema_init(&my_gamepad.sem, 1);
  my_gamepad.open_count = 0;

  // add cdev structure to kernel
  err = cdev_add(&my_gamepad.cdev, my_gamepad.dev, 1);
  if(err){ return -1; } // TODO: Handle error
  pr_debug("OK: Added cdev structure to kernel\n");

  // create a device file
  my_class = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(my_class)){ return -1; } // TODO: Handle error
  my_device = device_create(my_class, NULL, my_gamepad.dev, NULL, DEVICE_NAME);
  if(IS_ERR(my_device)){ return -1; } // TODO: Handle error
  pr_debug("OK: Device file created\n");

  pr_debug("DONE: No errors initializing the module\n");
	return 0;
}

// Perform cleanup and remove the gamepad module from kernel space
static void __exit gamepad_exit(void)
{
  pr_debug("Cleaning up the module:\n");

  // destroy structures used to create the device file
  device_destroy(my_class, my_gamepad.dev);
  class_destroy(my_class);
	pr_debug("OK: Destroyed structures used to create the device file\n");

  // remove cdev structure from the kernel
  cdev_del(&my_gamepad.cdev);
	pr_debug("OK: Removed cdev structure from the kernel\n");

  // deallocate device numbers
  unregister_chrdev_region(my_gamepad.dev, 1);
	pr_debug("OK: Deallocated device numbers\n");

  pr_debug("DONE: No errors cleaning up the module\n");
  return;
}



//-----------------------------
// GPIO setup and cleanup code
//-----------------------------
static int setup_gpio_input(void)
{
  dev_dbg(my_device, "Setting up GPIO input:\n");

  // allocate memory region for port C registers
  gpio_pc_region = request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_pc_region)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Allocated memory for port C registers\n");

  // map region into virtual memory
  gpio_pc_ptr = ioremap_nocache(GPIO_PC_BASE, GPIO_PC_LENGTH);
  if(IS_ERR(gpio_pc_ptr)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Mapped port C memory region to virtual memory\n");

  // configure input registers
  iowrite32(0x33333333, gpio_pc_ptr + GPIO_PC_MODEL); // set pins 0-7 to input
  iowrite32(0x000000ff, gpio_pc_ptr + GPIO_PC_DOUT);  // enable pull-up
  dev_dbg(my_device, "OK: Configured input registers\n");

  dev_dbg(my_device, "DONE: No errors setting up GPIO input\n");
  return 0;
}

static void cleanup_gpio_input(void)
{
  dev_dbg(my_device, "Cleaning up GPIO input:\n");

  // reset input registers
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_MODEL);
  iowrite32(0x00000000, gpio_pc_ptr + GPIO_PC_DOUT);
  dev_dbg(my_device, "OK: Reset input registers\n");

  // unmap memory region for input registers
  iounmap(gpio_pc_ptr);
  dev_dbg(my_device, "OK: Unmapped memory region for input registers\n");

  // deallocate memory region for input registers
  release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);
  dev_dbg(my_device, "OK: Deallocated memory region for input registers\n");

  dev_dbg(my_device, "DONE: No errors cleaning up GPIO input\n");
}

static int setup_gpio_interrupts(void)
{
  dev_dbg(my_device, "Setting up GPIO interrupts:\n");

  // allocate memory region for interrupt registers
  gpio_irq_region = request_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH, DEVICE_NAME);
  if(IS_ERR(gpio_irq_region)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Allocated memory for interrupt registers\n");

  // map region into virtual memory
  gpio_irq_ptr = ioremap_nocache(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);
  if(IS_ERR(gpio_irq_ptr)){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Mapped interrupt memory region to virtual memory\n");

  // configure interrupt registers
  iowrite32(0x22222222, gpio_irq_ptr + GPIO_EXTIPSELL); // Set port C as interrupt source
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIFALL);  // Set interrupt on 1->0
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_EXTIRISE);  // Set interrupt on 0->1
  dev_dbg(my_device, "OK: Configured interrupt registers\n");

  // register interrupt handler
  int flags = 0;
  err = request_irq(GPIO_EVEN_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){ return -1; } // TODO: Handle error
  err = request_irq(GPIO_ODD_IRQ, &gpio_handler, flags, DEVICE_NAME, NULL);
  if(err){ return -1; } // TODO: Handle error
  dev_dbg(my_device, "OK: Registered interrupt handler\n");

  // enable interrupts
  iowrite32(0x000000ff, gpio_irq_ptr + GPIO_IEN);
  dev_dbg(my_device, "OK: Enabled interrupts\n");

  dev_dbg(my_device, "DONE: No errors setting up GPIO interrupts\n");
  return 0;
}

static void cleanup_gpio_interrupts(void)
{
  dev_dbg(my_device, "Cleaning up GPIO interrupts:\n");

  // disable interrupts
  iowrite32(0x0, gpio_irq_ptr + GPIO_IEN);
  dev_dbg(my_device, "OK: Disabled interrupts\n");

  // unregister interrupt handler
  free_irq(GPIO_EVEN_IRQ, NULL);
  free_irq(GPIO_ODD_IRQ, NULL);
  dev_dbg(my_device, "OK: Unregistered interrupt handler\n");

  // reset interrupt registers
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIPSELL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIFALL);
  iowrite32(0x0, gpio_irq_ptr + GPIO_EXTIRISE);
  dev_dbg(my_device, "OK: Reset interrupt registers\n");

  // unmap memory region for interrupt registers
  iounmap(gpio_irq_ptr);
  dev_dbg(my_device, "OK: Unmapped memory region for interrupt registers\n");

  // deallocate memory region for interrupt registers
  release_mem_region(GPIO_IRQ_BASE, GPIO_IRQ_LENGTH);
  dev_dbg(my_device, "OK: Deallocated memory region for interrupt registers\n");

  dev_dbg(my_device, "DONE: No errors cleaning up GPIO interrupts\n");
}



// Tell kernel about init and exit functions
module_init(gamepad_init);
module_exit(gamepad_exit);



// Module metadata
MODULE_DESCRIPTION("Driver module for a gamepad with eight buttons.");
MODULE_LICENSE("GPL");
